#include "rtapi_flavor.h"
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>

/******************************************************************/
// Tests for flavor_names

// Number of flavors
#ifdef RTAPI
#  ifdef HAVE_XENOMAI_THREADS
#   define FLAVOR_NAMES_COUNT 3
static char* expected_flavor_names[] = { "posix", "rt-preempt", "xenomai" };
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

    for (f=NULL; (fname=flavor_names(&f)); count++)
        assert_string_equal(fname, *(expected_name_ptr++));
    assert_int_equal(count, FLAVOR_NAMES_COUNT);
}

/******************************************************************/
// Tests for flavor_byname, flavor_byid

// Test inputs
typedef struct { char* name; int id; } flavor_by_test_data_t;
#ifdef RTAPI
static flavor_by_test_data_t flavor_by_test_data[] = {
    {"posix", 2},
    {"rt-preempt", 3},
#  ifdef HAVE_XENOMAI_THREADS
    {"xenomai", 4},
#  else
    {"xenomai", 0},
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
    int id;
    flavor_by_test_data_t * td_ptr = flavor_by_test_data;
    for (; td_ptr->id >= 0; td_ptr++) {
        id = flavor_byname(td_ptr->name);
        /* printf("name=%s; id=%d; res id is %d\n", td_ptr->name, td_ptr->id, id); */
        if (td_ptr->id > 0 && strcmp(td_ptr->name,"bogus") != 0)
            assert_int_equal(id, td_ptr->id);
        else
            assert_int_equal(id, 0);
    }
}

static void test_flavor_byid(void **state)
{
    flavor_descriptor_ptr f;
    flavor_by_test_data_t * td_ptr = flavor_by_test_data;
    for (; td_ptr->id >= 0; td_ptr++) {
        f = flavor_byid(td_ptr->id);
        /* printf("name=%s; id=%d; f is %p\n", td_ptr->name, td_ptr->id, f); */
        if (td_ptr->id > 0 && strcmp(td_ptr->name,"bogus") != 0) {
            assert_non_null(f);
            /* if (f) */
            /*     printf("f->flavor_id\n", f->flavor_id); */
            assert_int_equal(f->flavor_id, td_ptr->id);
            assert_string_equal(f->name, td_ptr->name);
        } else
            // Bogus tests
            assert_null(f);
    }
}


int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_flavor_names),
        cmocka_unit_test(test_flavor_byname),
        cmocka_unit_test(test_flavor_byid),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);    
}
