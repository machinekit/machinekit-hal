/** This file, 'meter.c', is a GUI program that serves as a simple
    meter to look at HAL signals.  It is a user space component and
    uses GTK 1.2 for the GUI code.  It allows you to view one pin,
    signal, or parameter, and updates its display about 10 times
    per second.  (It is not a realtime program, and heavy loading
    can temporarily slow or stop the update.)  Clicking on the 'Select'
    button pops up a dialog that allows you to select what pin/signal/
    parameter you want to monitor.  Multiple instances of the program
    can be started if you want to monitor more than one item.  If you
    add "pin|sig|par[am] <name>" to the command line, the meter will
    initially display the pin/signal/parameter <name>, otherwise it
    will initially display nothing.
*/
/* Added ability to specify initial window positon on command line
    -g Xposition Yposiion    Chris Morley June 2009
*/

/** Copyright (C) 2003 John Kasunich
                       <jmkasunich AT users DOT sourceforge DOT net>
	Adapted for new Machinekit halobject-headers 2015 by Mick Grant
						<arceye AT mgware DOT co DOT uk>
*/

/** This program is free software; you can redistribute it and/or
    modify it under the terms of version 2 of the GNU General
    Public License as published by the Free Software Foundation.
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111 USA

    THE AUTHORS OF THIS LIBRARY ACCEPT ABSOLUTELY NO LIABILITY FOR
    ANY HARM OR LOSS RESULTING FROM ITS USE.  IT IS _EXTREMELY_ UNWISE
    TO RELY ON SOFTWARE ALONE FOR SAFETY.  Any machinery capable of
    harming persons must have provisions for completely removing power
    from all motors, etc, before persons enter any danger area.  All
    machinery must be designed to comply with local and national safety
    codes, and the authors of this software can not, and do not, take
    any responsibility for such compliance.

    This code is part of the Machinekit HAL project.  For more
    information, go to https://github.com/machinekit.
*/

#include "runtime/config.h"
#include <locale.h>
#include <libintl.h>
#define _(x) gettext(x)

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>
#include <string.h>

#include "runtime/rtapi.h"		/* RTAPI realtime OS API */
#include "hal/hal.h"		/* HAL public API decls */
#include "hal/hal_priv.h"	/* private HAL decls */

#include <gtk/gtk.h>
#include "misc_gtk/miscgtk.h"		/* generic GTK stuff */

/***********************************************************************
*                            TYPEDEFS                                  *
************************************************************************/

/** a 'probe' is an object that references a HAL pin, signal, or
    parameter.  The user may select the item that is to be probed.
*/

#define PROBE_NAME_LEN 63

typedef struct {
    int listnum;		/* 0 = pin, 1 = signal, 2 = parameter */
    char *pickname;		/* name from list, not validated */
    hal_pin_t *pin;		/* metadata (if it's a pin) */
    hal_sig_t *sig;		/* metadata (if it's a signal) */
    hal_param_t *param;		/* metadata (if it's a parameter) */
    GtkWidget *window;		/* selection dialog window */
    GtkWidget *notebook;	/* pointer to the notebook */
    GtkWidget *lists[3];	/* lists for pins, sigs, and params */
    char probe_name[PROBE_NAME_LEN + 1];	/* name of this probe */
} probe_t;

typedef struct {
    probe_t *probe;		/* probe that locates the data */
    GtkWidget *value_label;	/* label object to display value */
    GtkWidget *name_label;	/* label object to display name */
    GtkWidget *button_select;	/* invokes the select dialog */
} meter_t;

/***********************************************************************
*                  GLOBAL VARIABLES DECLARATIONS                       *
************************************************************************/

int comp_id;			/* HAL component ID */

GtkWidget *main_window;
int small;

/***********************************************************************
*                  LOCAL FUNCTION PROTOTYPES                           *
************************************************************************/

static meter_t *meter_new(void);

/** 'probe_new()' creates a new probe structure.  It also creates
    a dialog window for the probe that allows the user to pick the
    pin, signal, or parameter that the probe will attach to.  It
    should be called during the init phase of the program, before
    the main event loop is started.
*/
static probe_t *probe_new(char *probe_name);

/** 'popup_probe_window()' is an event handler function that opens
    the selection dialog for a probe.  'data' must be a pointer to
    a probe_t structure that was allocated by 'probe_new'.
*/
static void popup_probe_window(GtkWidget * widget, gpointer data);

static void quit(int sig);
static void exit_from_hal(void);
static int refresh_value(gpointer data);
static char *data_value(int type, void *valptr);

static void create_probe_window(probe_t * probe);
static void apply_selection(GtkWidget * widget, gpointer data);
static void close_selection(GtkWidget * widget, gpointer data);
static void selection_made(GtkWidget * clist, gint row, gint column,
    GdkEventButton * event, gpointer data);
static void page_switched(GtkNotebook *notebook, GtkNotebookPage *page,
		guint page_num, gpointer user_data);
/***********************************************************************
*                        MAIN() FUNCTION                               *
************************************************************************/

int main(int argc, gchar * argv[])
{
    GtkWidget *vbox, *hbox;
    GtkWidget *button_select, *button_exit;
    char buf[30];
    int initial_type = 0, n, i, height, geometryflag = 0, xposition = 0, yposition = 0, width = 270;
    char *initial_name = NULL , *win_name;
    meter_t *meter;

    bindtextdomain("hal", HAL_PO_DIR);
    setlocale(LC_MESSAGES,"");
    setlocale(LC_CTYPE,"");
    textdomain("hal");

    /* process and remove any GTK specific command line args */
    gtk_init(&argc, &argv);

    /* process my own command line args (if any) here */
    small = 0;
    n = 1;
    while ( argc > n ) {
        if ( strcmp (argv[n], "-g") == 0 ) {
            /* This sets up the variables for initial position of window*/
            /* The last check is for the optional width request*/
	        geometryflag = 1;
	        n++;
            xposition =  atoi(argv[n]);
            n++;
            yposition =  atoi(argv[n]);
            n++;
            if ( argc > n ){
                strcpy(buf,argv[n]);
                for (i=0; i< strlen(argv[n]); i++) {
                    if (isdigit(buf[i]) == 0) { break; }
                }
                if (strlen(argv[n]) == i){
                    width =  atoi(argv[n]);
                    n++;
                }
            }
	    }
        if ((argc > n) && ( strcmp (argv[n], "-s") == 0 )) {
	        small = 1;
	        n++;
        }
        if (argc > n) {
	        /* check for user specified initial probe point */
	            if (strncmp(argv[n], "pin", 3) == 0) {
	                /* initial probe is a pin */
	                initial_type = 0;
	            } else if (strncmp(argv[n], "sig", 3) == 0) {
	                /* initial probe is a signal */
	                initial_type = 1;
	            } else if (strncmp(argv[n], "par", 3) == 0) {
	                /* initial probe is a parameter */
	                initial_type = 2;
	            } else {
	                printf(_("ERROR: '%s' is not a valid probe type\n"), argv[n]);
	                return -1;
	            }
	            n++;
	            if ( argc > n ) {
	                initial_name = argv[n];
                    n++;
	            } else {
	                printf(_("ERROR: no pin/signal/parameter name\n"));
	                return -1;
	            }
        }
    }
    if ((initial_name == NULL) && (small == 1)) {
        printf(_("ERROR: -s option requires a probe type and a pin/signal/parameter name\n"));
        return -1;
    }

    /* create a unique module name */
    snprintf(buf, 29, "halmeter-%d", getpid());
    /* connect to the HAL */
    comp_id = hal_init(buf);
    if (comp_id < 0) {
	return -1;
    }
    hal_ready(comp_id);
    /* register an exit function to disconnect from the HAL */
    atexit(exit_from_hal);
    /* capture INT (ctrl-C) and TERM signals */
    signal(SIGINT, quit);
    signal(SIGTERM, quit);

    /* create main window, set it's size, and lock the size */
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    /* ideally this wouldn't be fixed size in pixels */
    if ( small ) {
	height = 22;
	win_name = initial_name;
    } else {
	height = 80;
	win_name = _("Hal Meter");
    }
    gtk_widget_set_usize(GTK_WIDGET(main_window), width, height);
    gtk_window_set_policy(GTK_WINDOW(main_window), FALSE, FALSE, FALSE);
    gtk_window_set_keep_above(GTK_WINDOW(main_window),TRUE);
    /* set main window title */
    gtk_window_set_title(GTK_WINDOW(main_window), win_name);
    /* this makes the application exit when the window is closed */
    gtk_signal_connect(GTK_OBJECT(main_window), "destroy",
	GTK_SIGNAL_FUNC(gtk_main_quit), NULL);

    /* a vbox to hold the displayed value and the pin/sig/param name */
    vbox = gtk_vbox_new(FALSE, 3);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 2);
    /* add the vbox to the main window */
    gtk_container_add(GTK_CONTAINER(main_window), vbox);
    gtk_widget_show(vbox);

    /* create a meter object */
    meter = meter_new();
    if (meter == NULL) {
    printf("null meter\n");
	exit(-1);
    }

    /* set up for initial probe, if any */
    if (initial_name != NULL) {
	meter->probe->pickname = initial_name;
	meter->probe->listnum = initial_type;
	apply_selection(NULL, meter->probe);
    }

    /* add the meter's value label to the vbox */
    gtk_box_pack_start(GTK_BOX(vbox), meter->value_label, TRUE, TRUE, 0);
    gtk_widget_show(meter->value_label);

    /* add the meter's name label to the vbox */
    if ( !small ) {
	gtk_box_pack_start(GTK_BOX(vbox), meter->name_label, TRUE, TRUE, 0);
	gtk_widget_show(meter->name_label);
    }

    /* arrange for periodic refresh of the value */
    gtk_timeout_add(100, refresh_value, meter);

    /* an hbox to hold the select and exit buttons */
    if ( !small ) {
	hbox = gtk_hbox_new_in_box(FALSE, 0, 0, vbox, FALSE, TRUE, 0);

	/* create the buttons and add them to the hbox */
	button_select = gtk_button_new_with_label(_("_Select"));
	button_exit = gtk_button_new_with_label(_("E_xit"));
	gtk_button_set_use_underline((GtkButton *)button_select, TRUE);
	gtk_button_set_use_underline((GtkButton *)button_exit, TRUE);

	gtk_box_pack_start(GTK_BOX(hbox), button_select, TRUE, TRUE, 4);
	gtk_box_pack_start(GTK_BOX(hbox), button_exit, TRUE, TRUE, 4);

	/* make the application exit when the 'exit' button is clicked */
	gtk_signal_connect(GTK_OBJECT(button_exit), "clicked",
	    GTK_SIGNAL_FUNC(gtk_main_quit), NULL);

	/* activate selection window when the 'select' button is clicked */
	gtk_signal_connect(GTK_OBJECT(button_select), "clicked",
	    GTK_SIGNAL_FUNC(popup_probe_window), meter->probe);

	/* save reference to select button */
	meter->button_select = button_select;

	gtk_widget_show(button_select);
	gtk_widget_show(button_exit);
    }

    /* The interface is now set up so we show the window and
       enter the gtk_main loop. */
    gtk_widget_show(main_window);
    /* If the -g option was invoked: set position */
    if (geometryflag == 1) {
        gtk_window_move(GTK_WINDOW(main_window),xposition,yposition);
    }
    gtk_main();

    return (0);
}

/***********************************************************************
*                         LOCAL FUNCTION CODE                          *
************************************************************************/

static meter_t *meter_new(void)
{
    meter_t *new;

    /* allocate a meter object for the display */
    new = malloc(sizeof(meter_t));
    if (new == NULL) {
	return NULL;
    }
    /* define a probe for the display item */
    new->probe = probe_new(_("Select Item to Probe"));
    if (new->probe == NULL) {
	free(new);
	return NULL;
    }
    /* create a label widget to hold the value */
    new->value_label = gtk_label_new("----");
    /* center justify text, no wordwrap */
    gtk_label_set_justify(GTK_LABEL(new->value_label), GTK_JUSTIFY_CENTER);
    gtk_label_set_line_wrap(GTK_LABEL(new->value_label), FALSE);

    /* create a label widget to hold the name */
    if ( !small ) {
	new->name_label = gtk_label_new("------");
	/* center justify text, no wordwrap */
	gtk_label_set_justify(GTK_LABEL(new->name_label),
	    GTK_JUSTIFY_CENTER);
	gtk_label_set_line_wrap(GTK_LABEL(new->name_label), FALSE);
    }
    return new;
}

probe_t *probe_new(char *probe_name)
{
    probe_t *new;

    if (probe_name == NULL) {
	/* no name specified, fake it */
	probe_name = _("Select Item to Probe");
    }
    /* allocate a new probe structure */
    new = malloc(sizeof(probe_t));
    if (new == NULL) {
	return NULL;
    }
    /* init the fields */
    new->pickname = NULL;
    new->listnum = -1;
    new->pin = NULL;
    new->sig = NULL;
    new->param = NULL;
    strncpy(new->probe_name, probe_name, HAL_NAME_LEN);
    new->probe_name[HAL_NAME_LEN] = '\0';
    /* window will be created just before it is displayed */
    new->window = NULL;
    /* done */
    return new;
}

static int fill_list(hal_object_ptr o, foreach_args_t *args)
{
gchar *name;
int row = 0;
probe_t *probe = (probe_t *) args->user_ptr1;

	if(args->type == HAL_PIN)
		{
		hal_pin_t *pin = o.pin;
		name = (char *)ho_name(pin);
		gtk_clist_append(GTK_CLIST(probe->lists[0]), &name);
		// if we have a pin selected, and it matches the current one
		// mark this row
		if ((probe->listnum == 0) && (probe->pin == pin))
			{
			gtk_clist_select_row(GTK_CLIST(probe->lists[0]), row, 0);
			// Get the text from the list
			gtk_clist_get_text(GTK_CLIST(probe->lists[0]), row, 0, &(probe->pickname));
			args->user_arg1 = 1;
			}
		}
	else if(args->type == HAL_SIGNAL)
		{
		hal_sig_t *sig = o.sig;
		name = (char *)ho_name(sig);
		gtk_clist_append(GTK_CLIST(probe->lists[1]), &name);
		// if we have a signal selected, and it matches the current
		// one, mark this row
		if ((probe->listnum == 1) && (probe->sig == sig))
			{
			gtk_clist_select_row(GTK_CLIST(probe->lists[1]), row, 0);
			// Get the text from the list
			gtk_clist_get_text(GTK_CLIST(probe->lists[1]), row, 0, &(probe->pickname));
			args->user_arg1 = 1;
			}
		}
	else if(args->type == HAL_PARAM)
		{
		hal_param_t *param = o.param;
		name =  (char *)ho_name(param);
		gtk_clist_append(GTK_CLIST(probe->lists[2]), &name);

		// if we have a param selected, and it matches the current
		// one, mark this row)
		if ((probe->listnum == 2) && (probe->param == param)) {
			gtk_clist_select_row(GTK_CLIST(probe->lists[2]), row, 0);
			// Get the text from the list
			gtk_clist_get_text(GTK_CLIST(probe->lists[2]), row, 0, &(probe->pickname));
			args->user_arg1 = 1;
			}
		}

	return 0;
}

void popup_probe_window(GtkWidget * widget, gpointer data)
{
probe_t *probe;
    /* get a pointer to the probe data structure */
    probe = (probe_t *) data;

    /* create window if needed */
    if (probe->window == NULL) {
	create_probe_window(probe);
    }else{
    gtk_window_present(GTK_WINDOW(probe->window));
    }

    gtk_clist_clear(GTK_CLIST(probe->lists[0]));
    gtk_clist_clear(GTK_CLIST(probe->lists[1]));
    gtk_clist_clear(GTK_CLIST(probe->lists[2]));

// pins
    foreach_args_t args1 =  {
	.type = HAL_PIN,
	.user_ptr1 = probe,
	.user_arg1 = 0
    };
    gtk_clist_freeze(GTK_CLIST(probe->lists[0]));

    halg_foreach(true, &args1, fill_list);

    gtk_clist_thaw(GTK_CLIST(probe->lists[0]));
    // if no match, unselect the first row, otherwise it will stay selected
    // and confuse the user
    if (!args1.user_arg1)
    	gtk_clist_unselect_row(GTK_CLIST(probe->lists[0]), 0, 0);

// signals
    foreach_args_t args2 =  {
	.type = HAL_SIGNAL,
	.user_ptr1 = probe,
	.user_arg1 = 0
    };
    gtk_clist_freeze(GTK_CLIST(probe->lists[1]));

    halg_foreach(true, &args2, fill_list);

    gtk_clist_thaw(GTK_CLIST(probe->lists[1]));
    // if no match, unselect the first row, otherwise it will stay selected
    // and confuse the user
    if (!args2.user_arg1)
    	gtk_clist_unselect_row(GTK_CLIST(probe->lists[1]), 0, 0);

// params
    foreach_args_t args3 =  {
	.type = HAL_PARAM,
	.user_ptr1 = probe,
	.user_arg1 = 0
    };
    gtk_clist_freeze(GTK_CLIST(probe->lists[2]));

    halg_foreach(true, &args3, fill_list);

    gtk_clist_thaw(GTK_CLIST(probe->lists[2]));
    // if no match, unselect the first row, otherwise it will stay selected
    // and confuse the user
    if (!args3.user_arg2)
    	gtk_clist_unselect_row(GTK_CLIST(probe->lists[2]), 0, 0);

    if (probe->listnum >= 0) {
	gtk_notebook_set_page(GTK_NOTEBOOK(probe->notebook), probe->listnum);
    } else { //nothing selected, select the first (pin page)
	gtk_notebook_set_page(GTK_NOTEBOOK(probe->notebook), 0);
    }

    gtk_widget_show_all(probe->window);

}

static void quit(int sig)
{
    gtk_main_quit();
}

static void exit_from_hal(void)
{
    hal_exit(comp_id);
}

/* this function refreshes the value display */
static int refresh_value(gpointer data)
{
meter_t *meter;
probe_t *probe;
const char *value_str, *name_str;
//hal_sig_t *sig;
static int first = 1;

    meter = (meter_t *) data;
    probe = meter->probe;

    if ( first ) {
		first = 0;
		if ( probe->pickname == NULL )
			g_signal_emit_by_name(meter->button_select, "clicked");
		}

    if (probe->pin != NULL) {
		if(! ho_valid(probe->pin))
			{
			/* pin has been deleted, can't display it any more */
			probe->pin = NULL;
			return 1;
			}
		name_str = ho_name(probe->pin);
		value_str = data_value(pin_type(probe->pin), pin_value(probe->pin));
		/* if (!probe->pin->signal == 0) */
		/* 	/\* pin is unlinked, get data from dummysig *\/ */
		/* 	value_str = data_value(probe->pin->type, &(probe->pin->dummysig)); */
		/* else { */
		/* 	/\* pin is linked to a signal *\/ */
		/* 	sig = SHMPTR(probe->pin->signal); */
		/* 	value_str = data_value(probe->pin->type, SHMPTR(sig->data_ptr)); */
		/* 	} */
		}
	else if (probe->sig != NULL) {
		if(! ho_valid(probe->sig))
		{
	    /* signal has been deleted, can't display it any more */
	    probe->sig = NULL;
	    return 1;
		}
		name_str = ho_name(probe->sig);
		value_str = data_value(sig_type(probe->sig),sig_value(probe->sig));
		}
	else if (probe->param != NULL) {
		if(! ho_valid(probe->param))
			{
			/* parameter has been deleted, can't display it any more */
			probe->param = NULL;
	        return 1;
		}
		name_str = ho_name(probe->param);
		value_str = data_value(param_type(probe->param),param_value(probe->param));

		//value_str = data_value(probe->param->type, SHMPTR(probe->param->data_ptr));
		}
	else {
		name_str = "-----";
		value_str = "---";
		}
    gtk_label_set_text(GTK_LABEL(meter->value_label), value_str);
    if (!small)
		gtk_label_set_text(GTK_LABEL(meter->name_label), name_str);

    return 1;
}

/* Switch function to return var value for the print_*_list functions  */
static char *data_value(int type, void *valptr)
{
    char *value_str;
    static char buf[100];

    switch (type) {
    case HAL_BIT:
	if (*((char *) valptr) == 0)
	    value_str = "FALSE";
	else
	    value_str = "TRUE";
	break;
    case HAL_FLOAT:
	snprintf(buf, sizeof(buf), "%.7g", (double)*((hal_float_t *) valptr));
	value_str = buf;
	break;
    case HAL_S32:
	snprintf(buf, sizeof(buf), "%10ld", (long)*((hal_s32_t *) valptr));
	value_str = buf;
	break;
    case HAL_U32:
	snprintf(buf, sizeof(buf), "%10lu", (unsigned long)*((hal_u32_t *) valptr));
	value_str = buf;
	break;
    case HAL_S64:
	snprintf(buf, sizeof(buf), "%lld", (long long)*((hal_s64_t *) valptr));
	value_str = buf;
	break;
    case HAL_U64:
	snprintf(buf, sizeof(buf), "%llu", (unsigned long long)*((hal_u64_t *) valptr));
	value_str = buf;
	break;
    default:
	/* Shouldn't get here, but just in case... */
	value_str = "";
    }
    return value_str;
}

static void create_probe_window(probe_t * probe)
{
    GtkWidget *vbox, *hbox, *notebk;
    GtkWidget *button_close;
    GtkWidget *scrolled_window;
    gchar *tab_label_text[3];
    gint n;

    /* create window, set it's size */
    probe->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_usize(GTK_WIDGET(probe->window), -2, 400);
    /* allow user to grow but not shrink the window */
    gtk_window_set_policy(GTK_WINDOW(probe->window), FALSE, TRUE, FALSE);
    /* set set_probe window title */
    gtk_window_set_title(GTK_WINDOW(probe->window), probe->probe_name);

    /* a vbox to hold everything */
    vbox = gtk_vbox_new(FALSE, 3);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 2);
    /* add the vbox to the window */
    gtk_container_add(GTK_CONTAINER(probe->window), vbox);
    gtk_widget_show(vbox);

    /* create a notebook to hold pin, signal, and parameter lists */
    notebk = gtk_notebook_new();
    /* remember the notebook so we can change the pages later */
    probe->notebook = notebk;
    /* add the notebook to the window */
    gtk_box_pack_start(GTK_BOX(vbox), notebk, TRUE, TRUE, 0);
    /* set overall notebook parameters */
    gtk_notebook_set_homogeneous_tabs(GTK_NOTEBOOK(notebk), TRUE);
    /* text for tab labels */
    tab_label_text[0] = _(" _Pins ");
    tab_label_text[1] = _(" _Signals ");
    tab_label_text[2] = _(" Para_meters ");
    /* loop to create three identical tabs */
    for (n = 0; n < 3; n++) {
	/* Create a scrolled window to display the list */
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
	    GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_widget_show(scrolled_window);
	/* create a list to hold the data */
	probe->lists[n] = gtk_clist_new(1);
	/* set up a callback for when the user selects a line */
	gtk_signal_connect(GTK_OBJECT(probe->lists[n]), "select_row",
	    GTK_SIGNAL_FUNC(selection_made), probe);
	/* It isn't necessary to shadow the border, but it looks nice :) */
	gtk_clist_set_shadow_type(GTK_CLIST(probe->lists[n]), GTK_SHADOW_OUT);
	/* set list for single selection only */
	gtk_clist_set_selection_mode(GTK_CLIST(probe->lists[n]),
	    GTK_SELECTION_BROWSE);
	/* put the list into the scrolled window */
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW
	    (scrolled_window), probe->lists[n]);
	gtk_widget_show(probe->lists[n]);
	/* create a box for the tab label */
	hbox = gtk_hbox_new(TRUE, 0);
	/* create a label for the page */
	gtk_label_new_in_box(tab_label_text[n], hbox, TRUE, TRUE, 0);
	gtk_widget_show(hbox);
	/* add page to the notebook */
	gtk_notebook_append_page(GTK_NOTEBOOK(notebk), scrolled_window, hbox);
	gtk_signal_connect(GTK_OBJECT(notebk), "switch-page",
	    GTK_SIGNAL_FUNC(page_switched), probe);
	/* set tab attributes */
	gtk_notebook_set_tab_label_packing(GTK_NOTEBOOK(notebk), hbox,
	    TRUE, TRUE, GTK_PACK_START);
    }
    /* this selects the page holding the current selected probe */
    gtk_widget_show(notebk);
    probe->listnum=0;

    /* an hbox to hold the OK, apply, and cancel buttons */
    hbox = gtk_hbox_new_in_box(TRUE, 0, 0, vbox, FALSE, TRUE, 0);

    /* create the close button and add it to the hbox */
    button_close = gtk_button_new_with_label(_("_Close"));
    gtk_button_set_use_underline((GtkButton *)button_close, TRUE);
    gtk_box_pack_start(GTK_BOX(hbox), button_close, TRUE, TRUE, 4);

    /* make the window disappear if 'close' button is clicked */
    gtk_signal_connect(GTK_OBJECT(button_close), "clicked",
	GTK_SIGNAL_FUNC(close_selection), probe);
    gtk_widget_show(button_close);

    /* set probe->window to NULL if window is destroyed */
    gtk_signal_connect(GTK_OBJECT(probe->window), "destroy",
	GTK_SIGNAL_FUNC(gtk_widget_destroyed), &(probe->window));

    /* done */
}

static void apply_selection(GtkWidget * widget, gpointer data)
{
    probe_t *probe;

    /* get a pointer to the probe data structure */
    probe = (probe_t *) data;
    /* discard info about previous item */
    probe->pin = NULL;
    probe->sig = NULL;
    probe->param = NULL;
    if (probe->pickname == NULL) {
	/* not a valid selection */
	/* should pop up a message or something here, instead we ignore it */
	return;
    }
    if (probe->listnum == 0) {
	/* search the pin list */
	probe->pin = halpr_find_pin_by_name(probe->pickname);
    } else if (probe->listnum == 1) {
	/* search the signal list */
	probe->sig = halpr_find_sig_by_name(probe->pickname);
    } else if (probe->listnum == 2) {
	/* search the parameter list */
	probe->param = halpr_find_param_by_name(probe->pickname);
    }
    /* at this point, the probe structure contain a pointer to the item we
       wish to display, or all three are NULL if the item doesn't exist */
}

static void close_selection(GtkWidget * widget, gpointer data)
{
    probe_t *probe;

    /* get a pointer to the probe data structure */
    probe = (probe_t *) data;
    /* destroy the window, hiding it causes problems when showing again */
    // it wouldn't always switch to the same tab, which causes confusion
    // we need to rebuild the lists anyways,
    // so rebuilding it doesn't take that longer
    gtk_widget_destroy(probe->window);
}

/* If we come here, then the user has selected a row in the list. */
static void selection_made(GtkWidget * clist, gint row, gint column,
    GdkEventButton * event, gpointer data)
{
    probe_t *probe;

    /* get a pointer to the probe data structure */
    probe = (probe_t *) data;

    if (clist == NULL) {
	/* We get spurious events when the lists are populated I don't know
	   why.  If clist is null, it's a bad one! */
	return;
    }

    if (event) {
        /* If we get here, it should be a valid selection */
        /* Get the text from the list */
        gtk_clist_get_text(GTK_CLIST(clist), row, 0, &(probe->pickname));
        apply_selection(GTK_WIDGET(probe->window), probe);
        if (event->type == GDK_2BUTTON_PRESS) {
            close_selection(GTK_WIDGET(probe->window), probe);
        }
        return;
    }

}

static void page_switched(GtkNotebook *notebook, GtkNotebookPage *page,
		guint page_num, gpointer user_data)
{
	// update the listnum in probe data, because
	((probe_t *)user_data)->listnum=page_num;
}
