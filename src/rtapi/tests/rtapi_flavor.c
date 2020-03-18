#include "rtapi_flavor.h"
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>

// Set this to print verbose debug messages
int debug_tests = 0;
#define DEBUG(args...)                                         \
    do { if (debug_tests) fprintf(stderr, args); } while (0)

/******************************************************************/
// Mock functions

int __wrap_flavor_can_run_flavor(flavor_descriptor_ptr f)
{
    int ret = mock();
    if (f == NULL)
        DEBUG("mock:  flavor_can_run_flavor(NULL) = %d\n", ret);
    else
        DEBUG("mock:  flavor_can_run_flavor('%s') = %d\n", f->name, ret);
    function_called();
    /* check_expected(f->flavor_id); */
    check_expected_ptr(f);
    return ret;
}

char *__wrap_getenv(const char *name)
{
    // From <stdlib.h>
    function_called();
    check_expected_ptr(name);
    char* ret = mock_ptr_type(char *);
    DEBUG("mock:  getenv(%s) = '%s'\n", name, ret);
    return ret;
}

/******************************************************************/
// Tests for flavor_names

// Number of flavors
#ifdef RTAPI
#  ifdef HAVE_XENOMAI2_THREADS
#   define FLAVOR_NAMES_COUNT 3
static char* expected_flavor_names[] = { "posix", "rt-preempt", "xenomai2" };
#  else
#   define FLAVOR_NAMES_COUNT 2
static char* expected_flavor_names[] = { "posix", "rt-preempt" };
#  endif
#else // ULAPI
#   define FLAVOR_NAMES_COUNT 1
static char* expected_flavor_names[] = { "ulapi" };
#endif

static void test_flavor_names(void **state)
{
    flavor_descriptor_ptr * f = NULL;
    char** expected_name_ptr = expected_flavor_names;
    const char* fname;
    int count = 0;

    // Set up mock calls to flavor_can_run_flavor():  all runnable
    expect_function_calls(__wrap_flavor_can_run_flavor, FLAVOR_NAMES_COUNT);
    will_return_count(__wrap_flavor_can_run_flavor, 1, FLAVOR_NAMES_COUNT);
    for (count = 0; count < FLAVOR_NAMES_COUNT; count++)
        expect_value(__wrap_flavor_can_run_flavor, f,
                     flavor_byname(expected_flavor_names[count]));

    for (count=0; (fname=flavor_names(&f)); count++)
        assert_string_equal(fname, *(expected_name_ptr++));
    assert_int_equal(count, FLAVOR_NAMES_COUNT);
}

static void test_flavor_names_unrunnable(void **state)
{
    flavor_descriptor_ptr * f = NULL;
    int count = 0;

    // Set up mock calls to flavor_can_run_flavor():  all unrunnable
    expect_function_calls(__wrap_flavor_can_run_flavor, FLAVOR_NAMES_COUNT);
    will_return_count(__wrap_flavor_can_run_flavor, 0, FLAVOR_NAMES_COUNT);
    for (count = 0; count < FLAVOR_NAMES_COUNT; count++)
        expect_value(__wrap_flavor_can_run_flavor, f,
                     flavor_byname(expected_flavor_names[count]));

    // Shouldn't get any names
    for (count=0; flavor_names(&f); count++)
        fail();
    assert_int_equal(count, 0);
}

/******************************************************************/
// Tests for flavor_byname, flavor_byid

// Test inputs
typedef struct { char* name; int id; } flavor_by_test_data_t;
#ifdef RTAPI
static flavor_by_test_data_t flavor_by_test_data[] = {
    {"posix", 2},
    {"rt-preempt", 3},
#  ifdef HAVE_XENOMAI2_THREADS
    {"xenomai2", 4},
#  else
    {"xenomai2", 0},
    {"bogus", 4},
#  endif
    {"ulapi", 0},
    {"bogus", 0},
    {"bogus", -10000},
    {"END OF TESTS", -1},
};
#endif
#ifdef ULAPI
static flavor_by_test_data_t flavor_by_test_data[] = {
    {"ulapi", 1},
    {"posix", 0},
    {"rt-preempt", 0},
    {"bogus", 2},
    {"bogus", 3},
    {"bogus", 4},
    {"bogus", 10000},
    {"END OF TESTS", -1},
};
#endif

static void test_flavor_byname(void **state)
{
    flavor_descriptor_ptr flavor;
    flavor_by_test_data_t * td_ptr = flavor_by_test_data;
    for (; td_ptr->id >= 0; td_ptr++) {
        DEBUG("name=%s; id=%d\n", td_ptr->name, td_ptr->id);
        flavor = flavor_byname(td_ptr->name);

        if (td_ptr->id == 0 || strcmp(td_ptr->name, "bogus") == 0) {
            // Bogus name, or not compiled in
            assert_null(flavor);
            continue;
        }

        assert_non_null(flavor);
        if (td_ptr->id > 0 && strcmp(td_ptr->name,"bogus") != 0)
            assert_int_equal(flavor->flavor_id, td_ptr->id);
        else
            assert_int_equal(flavor->flavor_id, 0);
    }
}

static void test_flavor_byid(void **state)
{
    flavor_descriptor_ptr f;
    flavor_by_test_data_t * td_ptr = flavor_by_test_data;
    for (; td_ptr->id >= 0; td_ptr++) {
        f = flavor_byid(td_ptr->id);
        if (td_ptr->id > 0 && strcmp(td_ptr->name,"bogus") != 0) {
            assert_non_null(f);
            assert_int_equal(f->flavor_id, td_ptr->id);
            assert_string_equal(f->name, td_ptr->name);
        } else
            // Bogus tests
            assert_null(f);
    }
}

/******************************************************************/
// Tests for flavor_default

// Test inputs
typedef struct {
    char* getenv_ret; // Return value of getenv("FLAVOR")
    int ret;  // Returned flavor ID
    int exit;  // Exit error code
    // Return values of flavor_can_run_flavor(flavor) functions
    int ulapi_cannot_run; // Just to test the flow
    int posix_cannot_run; // Just to test the flow
    int rtpreempt_can_run;
    int xenomai2_can_run;
} flavor_default_test_data_t;

static flavor_default_test_data_t flavor_default_test_data[] = {
#ifdef RTAPI
    // Flavor from environment variable:  Success
    { .getenv_ret = "posix", .ret = 2 },
    { .getenv_ret = "rt-preempt", .rtpreempt_can_run = 1, .ret = 3 },
    // Flavor from environment variable:  No such flavor exit 100
    { .getenv_ret = "ulapi", .exit = 100 }, // No such flavor
    { .getenv_ret = "bogus", .exit = 100 },
    // Flavor from environment variable:  Cannot run exit 101
    { .getenv_ret = "rt-preempt", .rtpreempt_can_run = 0, .exit = 101 },
    { .getenv_ret = "posix", .posix_cannot_run = 1, .exit = 101 },
    // Choose best default:  Success
    { .ret = 2 }, // Worst case scenario:  posix
    { .getenv_ret = "", .ret = 2 }, // $FLAVOR set to empty
    { .rtpreempt_can_run = 1, .ret = 3 },  // RT_PREEMPT can_run
#  ifdef HAVE_XENOMAI2_THREADS
    { .xenomai2_can_run = 1, .ret = 4 },  // Xenomai 2 can_run
    { .rtpreempt_can_run = 1, .xenomai2_can_run = 1, .ret = 4 }, // Both can_run
#  endif
    // Choose best default:  No runnable flavors exit 102
    { .posix_cannot_run = 1, .exit = 102 }, // Impossible

#else // ULAPI
    // Flavor from environment variable:  Success
    { .getenv_ret = "ulapi", .ret = 1 },
    // Flavor from environment variable:  No such flavor exit 100
    { .getenv_ret = "posix", .exit = 100 },
    { .getenv_ret = "bogus", .exit = 100 },
    // Flavor from environment variable:  Cannot run exit 101
    { .getenv_ret = "ulapi", .ulapi_cannot_run = 1, .exit = 101 },
    // Choose best default:  Success
    { .ret = 1 },
    { .getenv_ret = "", .ret = 1 }, // $FLAVOR set to empty
    // Choose best default:  No runnable flavors exit 102
    { .ulapi_cannot_run = 1, .exit = 102 },
#endif
    { .getenv_ret = "END" }, // Marks end of tests
};

static void test_flavor_default_runner(flavor_default_test_data_t *td)
{
    flavor_descriptor_ptr flavor;
    DEBUG("test:  Setting up ret = %d, exit = %d\n", td->ret, td->exit);

    // getenv("FLAVOR") should always be called; returns test data
    DEBUG("test:  mock getenv(FLAVOR) = '%s'\n", td->getenv_ret);
    expect_function_call(__wrap_getenv);
    expect_string(__wrap_getenv, name, "FLAVOR");
    will_return(__wrap_getenv, td->getenv_ret);

    if (td->getenv_ret && td->getenv_ret[0]) {
        // $FLAVOR is set in environment
        if (td->ret > 0 || td->exit == 101)
            // Get flavor_descriptor for tests where $FLAVOR is valid 
            flavor = flavor_byname(td->getenv_ret);

        if (td->ret > 0) {
            DEBUG("test:  mock flavor_can_run_flavor(%s) = 1\n", flavor->name);
            expect_function_call(__wrap_flavor_can_run_flavor);
            expect_value(__wrap_flavor_can_run_flavor, f, flavor);
            will_return(__wrap_flavor_can_run_flavor, 1);
        }
        if (td->exit == 100) {
            // We don't care about these calls, for printing valid flavors
            // before exit; not critical to function and hard to mock
            DEBUG("test:  mock flavor_can_run_flavor('%s') = 0 (any)\n",
                  td->getenv_ret);
            expect_function_call_any(__wrap_flavor_can_run_flavor);
            expect_any_count(__wrap_flavor_can_run_flavor, f,
                             FLAVOR_NAMES_COUNT);
            will_return_always(__wrap_flavor_can_run_flavor, 1);
        }
        if (td->exit == 101) {
            DEBUG("test:  mock flavor_can_run_flavor('%s') = 0\n",
                  td->getenv_ret);
            expect_function_call(__wrap_flavor_can_run_flavor);
            expect_value(__wrap_flavor_can_run_flavor, f, flavor);
            will_return(__wrap_flavor_can_run_flavor, 0);
        }
    } else {
        // No $FLAVOR set; pick best available
#       ifdef ULAPI
        expect_function_call(__wrap_flavor_can_run_flavor);
        DEBUG("test:  mock flavor_can_run_flavor('ulapi') = %d\n",
              !td->ulapi_cannot_run);
        expect_value(__wrap_flavor_can_run_flavor, f, flavor_byname("ulapi"));
        will_return(__wrap_flavor_can_run_flavor, !td->ulapi_cannot_run);
#       else // RTAPI
        expect_function_calls(__wrap_flavor_can_run_flavor, 2);
        DEBUG("test:  mock flavor_can_run_flavor('posix') = %d\n",
              !td->posix_cannot_run);
        expect_value(__wrap_flavor_can_run_flavor, f, flavor_byname("posix"));
        will_return(__wrap_flavor_can_run_flavor, !td->posix_cannot_run);
        DEBUG("test:  mock flavor_can_run_flavor('rt-preempt') = %d\n",
              td->rtpreempt_can_run);
        expect_value(__wrap_flavor_can_run_flavor, f, flavor_byname("rt-preempt"));
        will_return(__wrap_flavor_can_run_flavor, td->rtpreempt_can_run);
#         ifdef HAVE_XENOMAI2_THREADS
        expect_function_call(__wrap_flavor_can_run_flavor);
        DEBUG("test:  mock flavor_can_run_flavor('xenomai2') = %d\n",
              td->xenomai2_can_run);
        expect_value(__wrap_flavor_can_run_flavor, f, flavor_byname("xenomai2"));
        will_return(__wrap_flavor_can_run_flavor, td->xenomai2_can_run);
#         endif
#       endif
    }

    // Run the function
    flavor = flavor_default();

    // Check result
    if (flavor) // Success
        assert_int_equal(flavor->flavor_id, td->ret);
    else  // Failure
        assert_int_equal(flavor_mocking_err, td->exit);
}

// This increments over the tests
typedef flavor_default_test_data_t * flavor_default_test_data_ptr;
flavor_default_test_data_ptr td = flavor_default_test_data;

static void test_flavor_default(void **state)
{
    // Walk through the list of tests and call runner
    if (td->getenv_ret != NULL && strcmp(td->getenv_ret, "END") == 0)
        // End of tests
        skip();
    else
        // Run the next test & increment test case
        test_flavor_default_runner(td++);
}

/******************************************************************/
// Tests for flavor_install

#ifdef RTAPI
char * flav_req = "posix";
#else // ULAPI
char * flav_req = "ulapi";
#endif

static void test_flavor_install_already_configured(void **state)
{
    // Simulate flavor already configured
    flavor_descriptor = flavor_byname(flav_req);

    // Request to install a flavor
    flavor_install(flavor_byname(flav_req));

    // Check error
    assert_int_equal(flavor_mocking_err, 103);
}

static void test_flavor_install_unrunnable(void **state)
{
    // Simulate unconfigured flavor
    flavor_descriptor = NULL;

    // Mock can_run_flavor() return value
    expect_function_call(__wrap_flavor_can_run_flavor);
    expect_value(__wrap_flavor_can_run_flavor, f, flavor_byname(flav_req));
    will_return(__wrap_flavor_can_run_flavor, 0);

    // Request unrunnable flavor
    flavor_install(flavor_byname(flav_req));

    // Check error
    assert_int_equal(flavor_mocking_err, 104);
}

static void test_flavor_install_success(void **state)
{
    // Simulate unconfigured flavor
    flavor_descriptor = NULL;

    // Mock can_run_flavor() return value
    expect_function_call(__wrap_flavor_can_run_flavor);
    expect_value(__wrap_flavor_can_run_flavor, f, flavor_byname(flav_req));
    will_return(__wrap_flavor_can_run_flavor, 1);

    // Request runnable flavor
    flavor_install(flavor_byname(flav_req));

    // Check flavor
    assert_non_null(flavor_descriptor);
    assert_int_equal(strcmp(flavor_descriptor->name, flav_req), 0);
}

/******************************************************************/
// Test runner

int main(void)
{
    // Tell functions we're in test mode
    flavor_mocking = 1;

    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_flavor_names),
        cmocka_unit_test(test_flavor_names_unrunnable),
        cmocka_unit_test(test_flavor_byname),
        cmocka_unit_test(test_flavor_byid),
#       ifdef RTAPI // Ugh
        cmocka_unit_test(test_flavor_default),
        cmocka_unit_test(test_flavor_default),
        cmocka_unit_test(test_flavor_default),
        cmocka_unit_test(test_flavor_default),
        cmocka_unit_test(test_flavor_default),
        cmocka_unit_test(test_flavor_default),
        cmocka_unit_test(test_flavor_default),
        cmocka_unit_test(test_flavor_default),
        cmocka_unit_test(test_flavor_default),
#         ifdef HAVE_XENOMAI2_THREADS
        cmocka_unit_test(test_flavor_default),
        cmocka_unit_test(test_flavor_default),
#         endif
        cmocka_unit_test(test_flavor_default),
#       else // ULAPI
        cmocka_unit_test(test_flavor_default),
        cmocka_unit_test(test_flavor_default),
        cmocka_unit_test(test_flavor_default),
        cmocka_unit_test(test_flavor_default),
        cmocka_unit_test(test_flavor_default),
        cmocka_unit_test(test_flavor_default),
        cmocka_unit_test(test_flavor_default),
#       endif
        cmocka_unit_test(test_flavor_install_already_configured),
        cmocka_unit_test(test_flavor_install_unrunnable),
        cmocka_unit_test(test_flavor_install_success),
    };

    return cmocka_run_group_tests_name("rtapi_flavor tests", tests, NULL, NULL);    
}
