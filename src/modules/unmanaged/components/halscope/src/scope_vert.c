/** This file, 'halsc_vert.c', contains the portion of halscope
    that deals with vertical stuff - signal sources, scaling,
    position and such.
*/

/** Copyright (C) 2003 John Kasunich
                       <jmkasunich AT users DOT sourceforge DOT net>
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
#include <ctype.h>
#include <string.h>

#include "runtime/rtapi.h"		/* RTAPI realtime OS API */
#include "hal/hal.h"		/* HAL public API decls */
#include "hal/hal_priv.h"	/* private HAL decls */

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "misc_gtk/miscgtk.h"		/* generic GTK stuff */
#include "scope_usr.h"		/* scope related declarations */

#define BUFLEN 80		/* length for sprintf buffers */

/***********************************************************************
*                  GLOBAL VARIABLES DECLARATIONS                       *
************************************************************************/

#define VERT_POS_RESOLUTION 100.0

/* The channel select buttons sometimes need to be toggled by
   the code rather than the user, without causing any action.
   This global is used for that */
static int ignore_click = 0;

/***********************************************************************
*                  LOCAL FUNCTION PROTOTYPES                           *
************************************************************************/

struct offset_data {
    char buf[BUFLEN];
    int ac_coupled;
};

static void init_chan_sel_window(void);
static void init_chan_info_window(void);
static void init_vert_info_window(void);

static gboolean dialog_select_source(int chan_num);
static void selection_made(GtkWidget * clist, gint row, gint column,
    GdkEventButton * event, dialog_generic_t * dptr);
static void change_source_button(GtkWidget * widget, gpointer gdata);
static void channel_off_button(GtkWidget * widget, gpointer gdata);
static void offset_button(GtkWidget * widget, gpointer gdata);
static gboolean dialog_set_offset(int chan_num);
static void scale_changed(GtkAdjustment * adj, gpointer gdata);
static void offset_changed(GtkEditable * editable, struct offset_data *);
static void offset_activated(GtkEditable * editable, gchar * button);
static void pos_changed(GtkAdjustment * adj, gpointer gdata);
static void chan_sel_button(GtkWidget * widget, gpointer gdata);

/* helper functions */
static void write_chan_config(FILE *fp, scope_chan_t *chan);

/***********************************************************************
*                       PUBLIC FUNCTIONS                               *
************************************************************************/

void init_vert(void)
{
    scope_chan_t *chan;
    int n;

    /* stop sampling */
    ctrl_shm->state = IDLE;
    /* init non-zero members of the vertical structure */
    invalidate_all_channels();
    /* init non-zero members of the channel structures */
    for (n = 1; n <= 16; n++) {
	chan = &(ctrl_usr->chan[n - 1]);
	chan->position = 0.5;
    }
    /* set up the windows */
    init_chan_sel_window();
    init_chan_info_window();
    init_vert_info_window();
}

int set_active_channel(int chan_num)
{
    int n, count;
    scope_vert_t *vert;
    scope_chan_t *chan;
    if (( chan_num < 1 ) || ( chan_num > 16 )) {
	return -1;
    }
    vert = &(ctrl_usr->vert);
    chan = &(ctrl_usr->chan[chan_num - 1]);

    if (vert->chan_enabled[chan_num - 1] == 0 ) {
	/* channel is disabled, want to enable it */
	if (ctrl_shm->state != IDLE) {
	    /* acquisition in progress, must restart it */
            prepare_scope_restart();
	}
	count = 0;
	for (n = 0; n < 16; n++) {
	    if (vert->chan_enabled[n]) {
		count++;
	    }
	}
	if (count >= ctrl_shm->sample_len) {
	    /* max number of channels already enabled */
	    return -2;
	}
	if (chan->name == NULL) {
	    /* no signal source */
	    return -3;
	}
	/* "push" the button in to indicate enabled channel */
	ignore_click = 1;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(vert->chan_sel_buttons[chan_num-1]), TRUE);
	vert->chan_enabled[chan_num - 1] = 1;
    }
    if (vert->selected != chan_num) {
	/* make chan_num the selected channel */
	vert->selected = chan_num;
	channel_changed();
    }
    return 0;
}

int set_channel_off(int chan_num)
{
    scope_vert_t *vert;
    int n;

    if ((chan_num < 1) || (chan_num > 16)) {
	return -1;
    }
    vert = &(ctrl_usr->vert);
    if ( vert->chan_enabled[chan_num - 1] == 0 ) {
	/* channel is already off, nothing to do */
	return -1;
    }
    vert->chan_enabled[chan_num - 1] = 0;
    /* force the button to pop out */
    ignore_click = 1;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(vert->
	    chan_sel_buttons[chan_num - 1]), FALSE);
    if ( chan_num == vert->selected ) {
	/* channel was selected, pick new selected channel */
	n = 0;
	vert->selected = 0;
	do {
	    chan_num++;
	    if (chan_num > 16) {
		chan_num = 1;
	    }
	    if (vert->chan_enabled[chan_num - 1] != 0) {
		vert->selected = chan_num;
	    }
	} while ((++n < 16) && (vert->selected == 0));
    }
    channel_changed();
    return 0;
}

int set_channel_source(int chan_num, int type, char *name)
{
    scope_vert_t *vert;
    scope_chan_t *chan;
    hal_pin_t *pin;
    hal_sig_t *sig;
    hal_param_t *param;

    vert = &(ctrl_usr->vert);
    chan = &(ctrl_usr->chan[chan_num - 1]);
    /* locate the selected item in the HAL */
    if (type == 0) {
	/* search the pin list */
	pin = halpr_find_pin_by_name(name);
	if (pin == NULL) {
	    /* pin not found */
	    return -1;
	}
	chan->data_source_type = 0;
	chan->data_source = SHMOFF(pin);
	chan->data_type = pin->type;
	chan->name = (char *)ho_name(pin);
    } else if (type == 1) {
	/* search the signal list */
	sig = halpr_find_sig_by_name(name);
	if (sig == NULL) {
	    /* signal not found */
	    return -1;
	}
	chan->data_source_type = 1;
	chan->data_source = SHMOFF(sig);
	chan->data_type = sig->type;
	chan->name = (char *)ho_name(sig);
    } else if (type == 2) {
	/* search the parameter list */
	param = halpr_find_param_by_name(name);
	if (param == NULL) {
	    /* parameter not found */
	    return -1;
	}
	chan->data_source_type = 2;
	chan->data_source = SHMOFF(param);
	chan->data_type = param->type;
	chan->name = (char *)ho_name(param);
    }
    switch (chan->data_type) {
    case HAL_BIT:
	chan->data_len = sizeof(hal_bit_t);
	chan->min_index = -2;
	chan->max_index = 2;
	break;
    case HAL_FLOAT:
	chan->data_len = sizeof(hal_float_t);
	chan->min_index = -36;
	chan->max_index = 36;
	break;
    case HAL_S32:
	chan->data_len = sizeof(hal_s32_t);
	chan->min_index = -2;
	chan->max_index = 30;
	break;
    case HAL_U32:
	chan->data_len = sizeof(hal_u32_t);
	chan->min_index = -2;
	chan->max_index = 30;
	break;
    case HAL_S64:
	chan->data_len = sizeof(hal_s64_t);
	chan->min_index = -2;
	chan->max_index = 30;
	break;
    case HAL_U64:
	chan->data_len = sizeof(hal_u64_t);
	chan->min_index = -2;
	chan->max_index = 30;
	break;
    default:
	/* Shouldn't get here, but just in case... */
	chan->data_len = 0;
	chan->min_index = -1;
	chan->max_index = 1;
    }
    /* invalidate any data in the buffer for this channel */
    vert->data_offset[chan_num - 1] = -1;
    /* set scale and offset to nominal values */
    chan->vert_offset = 0.0;
    chan->scale_index = 0;
    /* return success */
    return 0;
}

int set_vert_scale(int setting)
{
    scope_vert_t *vert;
    scope_chan_t *chan;
    GtkAdjustment *adj;
    int chan_num, index;
    double scale;
    gchar buf[BUFLEN];

    vert = &(ctrl_usr->vert);
    chan_num = vert->selected;
    if ((chan_num < 1) || (chan_num > 16)) {
	return -1;
    }
    chan = &(ctrl_usr->chan[chan_num - 1]);
    if ((setting > chan->max_index) || (setting < chan->min_index)) {
	/* value out of range for this data type */
	return -1;
    }
    /* save new index */
    chan->scale_index = setting;
    /* set scale slider based on new setting */
    adj = GTK_ADJUSTMENT(vert->scale_adj);
    gtk_adjustment_set_value(adj, setting);

    /* compute scale factor */
    scale = 1.0;
    index = chan->scale_index;
    while (index >= 3) {
	scale *= 10.0;
	index -= 3;
    }
    while (index <= -3) {
	scale *= 0.1;
	index += 3;
    }
    switch (index) {
    case 2:
	scale *= 5.0;
	break;
    case 1:
	scale *= 2.0;
	break;
    case -1:
	scale *= 0.5;
	break;
    case -2:
	scale *= 0.2;
	break;
    default:
	break;
    }
    chan->scale = scale;
    format_scale_value(buf, BUFLEN - 1, scale);
    gtk_label_set_text_if(vert->scale_label, buf);
    if (chan_num == ctrl_shm->trig_chan) {
	refresh_trigger();
    }
    request_display_refresh(1);
    return 0;
}

int set_vert_pos(double setting)
{
    scope_vert_t *vert;
    scope_chan_t *chan;
    int chan_num;
    GtkAdjustment *adj;

    /* range check setting */
    if (( setting < 0.0 ) || ( setting > 1.0 )) {
	return -1;
    }
    /* point to data */
    vert = &(ctrl_usr->vert);
    chan_num = vert->selected;
    if ((chan_num < 1) || (chan_num > 16)) {
	return -1;
    }
    chan = &(ctrl_usr->chan[chan_num - 1]);
    chan->position = setting;
    /* set position slider based on new setting */
    adj = GTK_ADJUSTMENT(vert->pos_adj);
    gtk_adjustment_set_value(adj, chan->position * VERT_POS_RESOLUTION);
    /* refresh other stuff */
    if (chan_num == ctrl_shm->trig_chan) {
	refresh_trigger();
    }
    request_display_refresh(1);
    return 0;
}

int set_vert_offset(double setting, int ac_coupled)
{
    scope_vert_t *vert;
    scope_chan_t *chan;
    int chan_num;
    gchar buf1[BUFLEN + 1], buf2[BUFLEN + 1];

    /* point to data */
    vert = &(ctrl_usr->vert);
    chan_num = vert->selected;
    if ((chan_num < 1) || (chan_num > 16)) {
	return -1;
    }
    chan = &(ctrl_usr->chan[chan_num - 1]);
    /* set the new offset */
    chan->vert_offset = setting;
    chan->ac_offset = ac_coupled;
    /* update the offset display */
    if (chan->data_type == HAL_BIT) {
	snprintf(buf1, BUFLEN, "----");
    } else {
        if(chan->ac_offset) {
            snprintf(buf1, BUFLEN, "(AC)");
        } else {
            format_signal_value(buf1, BUFLEN, chan->vert_offset);
        }
    }
    snprintf(buf2, BUFLEN, _("Offset\n%s"), buf1);
    gtk_label_set_text_if(vert->offset_label, buf2);
    /* refresh other stuff */
    if (chan_num == ctrl_shm->trig_chan) {
	refresh_trigger();
    }
    request_display_refresh(1);
    return 0;
}

void format_signal_value(char *buf, int buflen, double value)
{
    char *units;
    int decimals;
    char sign, symbols[] = "pnum KMGT";

    if (value < 0) {
	value = -value;
	sign = '-';
    } else {
	sign = '+';
    }
    if (value <= 1.0e-24) {
	/* pretty damn small, call it zero */
	snprintf(buf, buflen, "0.000");
	return;
    }
    if (value <= 1.0e-12) {
	/* less than pico units, use scientific notation */
	snprintf(buf, buflen, "%c%10.3e", sign, value);
	return;
    }
    if (value >= 1.0e+12) {
	/* greater than tera-units, use scientific notation */
	snprintf(buf, buflen, "%c%10.3e", sign, value);
	return;
    }
    units = &(symbols[4]);
    while (value < 1.0) {
	value *= 1000.0;
	units--;
    }
    while (value >= 1000.0) {
	value /= 1000.0;
	units++;
    }
    decimals = 3;
    if (value >= 9.999) {
	decimals--;
    }
    if (value >= 99.99) {
	decimals--;
    }
    snprintf(buf, buflen, "%c%0.*f%c", sign, decimals, value, *units);
}

void write_vert_config(FILE *fp)
{
    int n;
    scope_vert_t *vert;
    scope_chan_t *chan;

    vert = &(ctrl_usr->vert);
    /* first write disabled channels */
    for ( n = 1 ; n <= 16 ; n++ ) {
	if ( vert->chan_enabled[n-1] != 0 ) {
	    // channel enabled, do it later
	    continue;
	}
	chan = &(ctrl_usr->chan[n-1]);
	if ( chan->name == NULL ) {
	    // no source for this channel, skip it
	    continue;
	}
	fprintf(fp, "CHAN %d\n", n);
	write_chan_config(fp, chan);
	fprintf(fp, "CHOFF\n");
    }
    /* next write enabled channels */
    for ( n = 1 ; n <= 16 ; n++ ) {
	if ( vert->chan_enabled[n-1] == 0 ) {
	    // channel disabled, skip it
	    continue;
	}
	if ( vert->selected == n ) {
	    // channel selected, do it last
	    continue;
	}
	chan = &(ctrl_usr->chan[n-1]);
	if ( chan->name == NULL ) {
	    // no source for this channel, skip it
	    continue;
	}
	fprintf(fp, "CHAN %d\n", n);
	write_chan_config(fp, chan);
    }
    /* write selected channel last */
    if ((vert->selected < 1) || (vert->selected > 16)) {
	return;
    }
    chan = &(ctrl_usr->chan[vert->selected-1]);
    fprintf(fp, "CHAN %d\n", vert->selected);
    write_chan_config(fp, chan);
}


/***********************************************************************
*                       LOCAL FUNCTIONS                                *
************************************************************************/

void set_color(GdkColor *color, unsigned char red,
		unsigned char green, unsigned char blue) {
    color->red = ((unsigned long) red) << 8;
    color->green = ((unsigned long) green) << 8;
    color->blue = ((unsigned long) blue) << 8;
    color->pixel =
	((unsigned long) red) << 16 | ((unsigned long) green) << 8 |
	((unsigned long) blue);
}

extern int normal_colors[16][3], selected_colors[16][3];
static void init_chan_sel_window(void)
{
    scope_vert_t *vert;
    GtkWidget *button;
    long n;
    gchar buf[5];
    GdkColor c;

    vert = &(ctrl_usr->vert);
    for (n = 0; n < 16; n++) {
	snprintf(buf, 4, "%ld", n + 1);
	/* define the button */
	button = gtk_toggle_button_new_with_label(buf);

	/* set up colors of the label */
	set_color(&c, normal_colors[n][0],
			normal_colors[n][1], normal_colors[n][2]);
	gtk_widget_modify_bg(button, GTK_STATE_ACTIVE, &c);
	gtk_widget_modify_bg(button, GTK_STATE_SELECTED, &c);

	set_color(&c, selected_colors[n][0],
			selected_colors[n][1], selected_colors[n][2]);
	gtk_widget_modify_bg(button, GTK_STATE_PRELIGHT, &c);

	set_color(&c, 0, 0, 0);
	gtk_widget_modify_fg(button, GTK_STATE_ACTIVE, &c);
	gtk_widget_modify_fg(button, GTK_STATE_SELECTED, &c);
	gtk_widget_modify_fg(button, GTK_STATE_PRELIGHT, &c);

	/* put it in the window */
	gtk_box_pack_start(GTK_BOX(ctrl_usr->chan_sel_win), button, TRUE,
	    TRUE, 0);
	gtk_widget_show(button);
	/* hook a callback function to it */
	gtk_signal_connect(GTK_OBJECT(button), "clicked",
	    GTK_SIGNAL_FUNC(chan_sel_button), (gpointer) n + 1);
	/* save the button pointer */
	vert->chan_sel_buttons[n] = button;
    }
}

static void init_chan_info_window(void)
{
    scope_vert_t *vert;
    char dummyname[HAL_NAME_LEN+1];
    int n;

    vert = &(ctrl_usr->vert);

    vert->chan_num_label =
	gtk_label_new_in_box("--", ctrl_usr->chan_info_win, FALSE, FALSE, 5);
    gtk_label_size_to_fit(GTK_LABEL(vert->chan_num_label), "99");
    gtk_vseparator_new_in_box(ctrl_usr->chan_info_win, 3);

    /* a button to change the source */
    vert->source_name_button = gtk_button_new_with_label("------");
    gtk_box_pack_start(GTK_BOX(ctrl_usr->chan_info_win),
	vert->source_name_button, FALSE, FALSE, 3);

    vert->source_name_label = (GTK_BIN(vert->source_name_button))->child;
    gtk_label_set_justify(GTK_LABEL(vert->source_name_label),
	GTK_JUSTIFY_LEFT);
    /* longest source name we ever need to display */
    for ( n = 0 ; n < HAL_NAME_LEN ; n++) dummyname[n] = 'x';
    dummyname[n] = '\0';
    gtk_label_size_to_fit(GTK_LABEL(vert->source_name_label), dummyname);
    /* activate the source selection dialog if button is clicked */
    gtk_signal_connect(GTK_OBJECT(vert->source_name_button), "clicked",
	GTK_SIGNAL_FUNC(change_source_button), NULL);
    gtk_widget_show(vert->source_name_button);


    vert->readout_label = gtk_label_new_in_box("",
		    ctrl_usr->chan_info_win, FALSE, FALSE, 0);
    gtk_misc_set_alignment(GTK_MISC(vert->readout_label), 0, 0);
    gtk_label_set_justify(GTK_LABEL(vert->readout_label), GTK_JUSTIFY_LEFT);
    gtk_label_size_to_fit(GTK_LABEL(vert->readout_label),
		    "f(99999.9999) = 99999.9999 (ddt 99999.9999)");
}

static void init_vert_info_window(void)
{
    scope_vert_t *vert;
    GtkWidget *hbox, *vbox;
    GtkWidget *button;

    vert = &(ctrl_usr->vert);

    /* box for the two sliders */
    hbox =
	gtk_hbox_new_in_box(TRUE, 0, 0, ctrl_usr->vert_info_win, TRUE, TRUE,
	0);
    /* box for the scale slider */
    vbox = gtk_vbox_new_in_box(FALSE, 0, 0, hbox, TRUE, TRUE, 0);
    gtk_label_new_in_box(_("Gain"), vbox, FALSE, FALSE, 0);
    vert->scale_adj = gtk_adjustment_new(0, -5, 5, 1, 1, 0);
    vert->scale_slider = gtk_vscale_new(GTK_ADJUSTMENT(vert->scale_adj));
    gtk_scale_set_digits(GTK_SCALE(vert->scale_slider), 0);
    gtk_scale_set_draw_value(GTK_SCALE(vert->scale_slider), FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), vert->scale_slider, TRUE, TRUE, 0);
    /* connect the slider to a function that re-calcs vertical scale */
    gtk_signal_connect(GTK_OBJECT(vert->scale_adj), "value_changed",
	GTK_SIGNAL_FUNC(scale_changed), NULL);
    gtk_widget_show(vert->scale_slider);
    /* box for the position slider */
    vbox = gtk_vbox_new_in_box(FALSE, 0, 0, hbox, TRUE, TRUE, 0);
    gtk_label_new_in_box(_("Pos"), vbox, FALSE, FALSE, 0);
    vert->pos_adj =
	gtk_adjustment_new(VERT_POS_RESOLUTION / 2, 0, VERT_POS_RESOLUTION, 1,
	1, 0);
    vert->pos_slider = gtk_vscale_new(GTK_ADJUSTMENT(vert->pos_adj));
    gtk_scale_set_digits(GTK_SCALE(vert->pos_slider), 0);
    gtk_scale_set_draw_value(GTK_SCALE(vert->pos_slider), FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), vert->pos_slider, TRUE, TRUE, 0);
    /* connect the slider to a function that re-calcs vertical pos */
    gtk_signal_connect(GTK_OBJECT(vert->pos_adj), "value_changed",
	GTK_SIGNAL_FUNC(pos_changed), NULL);
    gtk_widget_show(vert->pos_slider);
    /* Scale display */
    gtk_hseparator_new_in_box(ctrl_usr->vert_info_win, 3);
    gtk_label_new_in_box(_("Scale"), ctrl_usr->vert_info_win, FALSE, FALSE, 0);
    vert->scale_label =
	gtk_label_new_in_box(" ---- ", ctrl_usr->vert_info_win, FALSE, FALSE,
	0);
    /* Offset control */
    vert->offset_button = gtk_button_new_with_label("Offset\n----");
    vert->offset_label = (GTK_BIN(vert->offset_button))->child;
    gtk_box_pack_start(GTK_BOX(ctrl_usr->vert_info_win),
	vert->offset_button, FALSE, FALSE, 0);
    gtk_signal_connect(GTK_OBJECT(vert->offset_button), "clicked",
	GTK_SIGNAL_FUNC(offset_button), NULL);
    gtk_widget_show(vert->offset_button);
    /* a button to turn off the channel */
    button = gtk_button_new_with_label(_("Chan Off"));
    gtk_box_pack_start(GTK_BOX(ctrl_usr->vert_info_win), button, FALSE, FALSE,
	0);
    /* turn off the channel if button is clicked */
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
	GTK_SIGNAL_FUNC(channel_off_button), NULL);
    gtk_widget_show(button);
}

static void scale_changed(GtkAdjustment * adj, gpointer gdata)
{
    set_vert_scale(adj->value);
}

static void pos_changed(GtkAdjustment * adj, gpointer gdata)
{
    set_vert_pos(adj->value / VERT_POS_RESOLUTION);
}

static void offset_button(GtkWidget * widget, gpointer gdata)
{
    scope_vert_t *vert;
    scope_chan_t *chan;
    int chan_num;

    vert = &(ctrl_usr->vert);
    chan_num = vert->selected;
    if ((chan_num < 1) || (chan_num > 16)) {
	return;
    }
    chan = &(ctrl_usr->chan[chan_num - 1]);
    if (chan->data_type == HAL_BIT) {
	/* no offset for bits */
	return;
    }
    if (dialog_set_offset(chan_num)) {
	if (chan_num == ctrl_shm->trig_chan) {
	    refresh_trigger();
	}
	channel_changed();
	request_display_refresh(1);
    }
}

static gboolean dialog_set_offset(int chan_num)
{
    scope_vert_t *vert;
    scope_chan_t *chan;
    dialog_generic_t dialog;
    gchar *title, msg[BUFLEN], *cptr;
    struct offset_data data;
    GtkWidget *label, *button;
    double tmp;

    vert = &(ctrl_usr->vert);
    chan = &(ctrl_usr->chan[chan_num - 1]);
    title = _("Set Offset");
    snprintf(msg, BUFLEN - 1, _("Set the vertical offset\n"
	"for channel %d."), chan_num);
    /* create dialog window, disable resizing */
    dialog.retval = 0;
    dialog.window = gtk_dialog_new();
    dialog.app_data = &data;
    /* allow user to grow but not shrink the window */
    gtk_window_set_policy(GTK_WINDOW(dialog.window), FALSE, TRUE, FALSE);
    /* window should appear in center of screen */
    gtk_window_set_position(GTK_WINDOW(dialog.window), GTK_WIN_POS_CENTER);
    /* set title */
    gtk_window_set_title(GTK_WINDOW(dialog.window), title);
    /* display message */
    label = gtk_label_new(msg);
    gtk_misc_set_padding(GTK_MISC(label), 15, 5);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog.window)->vbox), label, FALSE,
	TRUE, 0);
    /* a separator */
    gtk_hseparator_new_in_box(GTK_DIALOG(dialog.window)->vbox, 0);
    /* a checkbox: AC coupled */
    vert->offset_ac = gtk_check_button_new_with_label(_("AC Coupled"));
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog.window)->vbox),
        vert->offset_ac, FALSE, TRUE, 0);
    /* react to changes to the checkbox */
    gtk_signal_connect(GTK_OBJECT(vert->offset_ac), "toggled",
	GTK_SIGNAL_FUNC(offset_changed), &data);
    /* the entry */
    vert->offset_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog.window)->vbox),
	vert->offset_entry, FALSE, TRUE, 0);
    snprintf(data.buf, BUFLEN, "%f", chan->vert_offset);
    gtk_entry_set_text(GTK_ENTRY(vert->offset_entry), data.buf);
    gtk_entry_set_max_length(GTK_ENTRY(vert->offset_entry), BUFLEN-1);
    /* point at first char */
    gtk_entry_set_position(GTK_ENTRY(vert->offset_entry), 0);
    /* select all chars, so if the user types the original value goes away */
    gtk_entry_select_region(GTK_ENTRY(vert->offset_entry), 0, strlen(data.buf));
    /* make it active so user doesn't have to click on it */
    gtk_widget_grab_focus(GTK_WIDGET(vert->offset_entry));
    gtk_widget_show(vert->offset_entry);
    /* capture entry data to the buffer whenever the user types */
    gtk_signal_connect(GTK_OBJECT(vert->offset_entry), "changed",
	GTK_SIGNAL_FUNC(offset_changed), data.buf);
    /* set up a callback function when the window is destroyed */
    gtk_signal_connect(GTK_OBJECT(dialog.window), "destroy",
	GTK_SIGNAL_FUNC(dialog_generic_destroyed), &dialog);
    /* make OK and Cancel buttons */
    button = gtk_button_new_with_label(_("OK"));
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog.window)->action_area),
	button, TRUE, TRUE, 4);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
	GTK_SIGNAL_FUNC(dialog_generic_button1), &dialog);
    /* hit the "OK" button if the user hits enter */
    gtk_signal_connect(GTK_OBJECT(vert->offset_entry), "activate",
	GTK_SIGNAL_FUNC(offset_activated), button);
    button = gtk_button_new_with_label(_("Cancel"));
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog.window)->action_area),
	button, TRUE, TRUE, 4);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
	GTK_SIGNAL_FUNC(dialog_generic_button2), &dialog);
    /* make window transient and modal */
    gtk_window_set_transient_for(GTK_WINDOW(dialog.window),
	GTK_WINDOW(ctrl_usr->main_win));
    gtk_window_set_modal(GTK_WINDOW(dialog.window), TRUE);
    gtk_widget_show_all(dialog.window);
    gtk_main();
    /* we get here when the user makes a selection, hits Cancel, or closes
       the window */
    if ((dialog.retval == 0) || (dialog.retval == 2)) {
	/* user either closed dialog, or hit cancel */
	return FALSE;
    }
    tmp = strtod(data.buf, &cptr);
    if (cptr == data.buf) {
	return FALSE;
    }
    set_vert_offset(tmp, data.ac_coupled);
    return TRUE;
}

static void offset_changed(GtkEditable * editable, struct offset_data *data)
{
    const char *text;

    /* maybe user hit "ac coupled" button" */
    data->ac_coupled =
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ctrl_usr->vert.offset_ac));
    gtk_entry_set_editable(GTK_ENTRY(ctrl_usr->vert.offset_entry),
                !data->ac_coupled);

    /* maybe user typed something, save it in the buffer */
    text = gtk_entry_get_text(GTK_ENTRY(ctrl_usr->vert.offset_entry));
    strncpy(data->buf, text, BUFLEN);
}

static void offset_activated(GtkEditable * editable, gchar * button)
{
    /* user hit enter, generate a "clicked" event for the OK button */
    gtk_button_clicked(GTK_BUTTON(button));
}


static void chan_sel_button(GtkWidget * widget, gpointer gdata)
{
    long chan_num;
    int n, count;
    scope_vert_t *vert;
    scope_chan_t *chan;
    char *title, *msg;

    vert = &(ctrl_usr->vert);
    chan_num = (long) gdata;
    chan = &(ctrl_usr->chan[chan_num - 1]);

    if (ignore_click != 0) {
	ignore_click = 0;
	return;
    }
    if (vert->chan_enabled[chan_num - 1] == 0 ) {
	/* channel is disabled, want to enable it */
	if (ctrl_shm->state != IDLE) {
	    /* acquisition in progress, must restart it */
            prepare_scope_restart();
	}
	count = 0;
	for (n = 0; n < 16; n++) {
	    if (vert->chan_enabled[n]) {
		count++;
	    }
	}
	if (count >= ctrl_shm->sample_len) {
	    /* max number of channels already enabled */
	    /* force the button to pop back out */
	    ignore_click = 1;
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), FALSE);
	    title = _("Too many channels");
	    msg = _("You cannot add another channel.\n\n"
		"Either turn off one or more channels, or shorten\n"
		"the record length to allow for more channels");
	    dialog_generic_msg(ctrl_usr->main_win, title, msg, _("OK"), NULL,
		NULL, NULL);
	    return;
	}
	if (chan->name == NULL) {
	    /* need to assign a source */
	    if (dialog_select_source(chan_num) != TRUE) {
		/* user failed to assign a source */
		/* force the button to pop back out */
		ignore_click = 1;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget),
		    FALSE);
		return;
	    }
	}
	vert->chan_enabled[chan_num - 1] = 1;
    } else {
	/* channel was already enabled, user wants to select it */
	/* button should stay down, so we force it */
	ignore_click = 1;
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), TRUE);
    }
    if (vert->selected != chan_num) {
	/* make chan_num the selected channel */
	vert->selected = chan_num;
	channel_changed();
    }
}

static void channel_off_button(GtkWidget * widget, gpointer gdata)
{
    scope_vert_t *vert;
    int chan_num;

    vert = &(ctrl_usr->vert);
    chan_num = vert->selected;
    set_channel_off(chan_num);
}

static void change_source_button(GtkWidget * widget, gpointer gdata)
{
    int chan_num;

    chan_num = ctrl_usr->vert.selected;
    if ((chan_num < 1) || (chan_num > 16)) {
	return;
    }
    if (ctrl_shm->state != IDLE) {
        /* acquisition in progress, must restart it */
        prepare_scope_restart();
    }
    invalidate_channel(chan_num);
    dialog_select_source(chan_num);
    channel_changed();
}

static char search_target[HAL_NAME_LEN+1];
static guint32 search_time = 0;
static int search_row = -1;
#define SEARCH_RESET_TIME 1000 /* ms */

static void selection_made_common(GtkWidget *clist, gint row, dialog_generic_t *dptr) {
    gint n, listnum;
    gchar *name;
    int rv, chan_num;

    scope_vert_t *vert;
    /* If we get here, it should be a valid selection */
    vert = &(ctrl_usr->vert);
    chan_num = *((int *)(dptr->app_data));
    /* figure out which notebook tab it was */
    listnum = -1;
    for (n = 0; n < 3; n++) {
	if (clist == vert->lists[n]) {
	    listnum = n;
	}
    }
    /* Get the text from the list */
    gtk_clist_get_text(GTK_CLIST(clist), row, 0, &name);
    /* try to set up the new source */
    rv = set_channel_source(chan_num, listnum, name);
    if ( rv == 0 ) {
	/* set return value of dialog to indicate success */
	dptr->retval = 1;
    } else {
	/* new source invalid, return as if user hit cancel */
	dptr->retval = 2;
    }
    /* destroy window to cause dialog_generic_destroyed() to be called */
    gtk_widget_destroy(dptr->window);
    return;
}


static gboolean search_for_entry(GtkWidget *widget, GdkEventKey *event, dialog_generic_t *dptr)
{
    GtkCList *clist = GTK_CLIST(widget);
    int z, wrapped;

    if(event->keyval == GDK_Return) {
	selection_made_common(widget, clist->focus_row, dptr);
    }

    if(!isprint(event->string[0])) {
	strcpy(search_target, "");
	search_row = clist->focus_row;
	return 0;
    }

    if(event->time - search_time > SEARCH_RESET_TIME) {
	strcpy(search_target, "");
	search_row = clist->focus_row;
    }

    search_time = event->time;
    if(strcmp(event->string, " ") == 0) {
	char *text;
	search_row = search_row + 1;
	if(!gtk_clist_get_text(clist, search_row, 0, &text))
	    search_row = 0;
	printf("next search: %d\n", search_row);
    } else {
	strcat(search_target, event->string);
    }

    for(z = search_row, wrapped=0; z != search_row || !wrapped; z ++) {
	char *text;

	printf("search: %d (wrapped=%d)\n", z, wrapped);
	if(!gtk_clist_get_text(clist, z, 0, &text)) {
	    if(wrapped) break; // wrapped second time (why?)
	    z = 0;
	    wrapped = 1;
	}

	if(strstr(text, search_target)) {
	    double pos = (z+.5) / (clist->rows-1);
	    if(pos > 1) pos = 1;

	    GTK_CLIST_GET_CLASS(clist)->scroll_vertical(clist, GTK_SCROLL_JUMP, pos);
	    gtk_clist_select_row(clist, z, 0);
	    search_row = z;
	    return 1;
	}
    }
    return 0;
}

static gboolean change_page(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer user_data) {
    scope_vert_t *vert;

    vert = &(ctrl_usr->vert);
    if(page_num  < 3)
	gtk_widget_grab_focus(GTK_WIDGET(vert->lists[page_num]));
    return 0;
}

static int fill_list(hal_object_ptr o, foreach_args_t *args)
{
gchar *name;
int row = 0;
int chan_num = args->user_arg1;

scope_vert_t *vert;
scope_chan_t *chan;

	vert = &(ctrl_usr->vert);
	chan = &(ctrl_usr->chan[chan_num - 1]);

	if(args->type == HAL_PIN)
		{
		hal_pin_t *pin = o.pin;
		name =(char *) ho_name(pin);
		row = gtk_clist_append(GTK_CLIST(vert->lists[0]), &name);
		}
	else if(args->type == HAL_SIGNAL)
		{
		hal_sig_t *signal = o.sig;
		name = (char *)ho_name(signal);
		row = gtk_clist_append(GTK_CLIST(vert->lists[1]), &name);
		}
	else if(args->type == HAL_PARAM)
		{
		hal_param_t *param = o.param;
		name = (char *)ho_name(param);
		row = gtk_clist_append(GTK_CLIST(vert->lists[2]), &name);
		}

	if ( args->user_arg2 == 0 )
		{
	    if ( strcmp(name, chan->name) == 0 )
			args->user_arg3 = row;
	    }
	args->user_arg4 = row;

	return 0;
}

static gboolean dialog_select_source(int chan_num)
{
    scope_vert_t *vert;
    scope_chan_t *chan;
    dialog_generic_t dialog;
    gchar *title, msg[BUFLEN];
    int n, initial_page = 0, initial_row = 0, max_row = 0;
    gchar *tab_label_text[3];
    GtkWidget *hbox, *label, *notebk, *button;
    GtkAdjustment *adj;

    vert = &(ctrl_usr->vert);
    chan = &(ctrl_usr->chan[chan_num - 1]);
    title = _("Select Channel Source");
    snprintf(msg, BUFLEN - 1, _("Select a pin, signal, or parameter\n"
	"as the source for channel %d."), chan_num);
    /* create dialog window, disable resizing */
    dialog.retval = 0;
    dialog.window = gtk_dialog_new();
    dialog.app_data = &chan_num;
    /* set initial height of window */
    gtk_widget_set_usize(GTK_WIDGET(dialog.window), -2, 300);
    /* allow user to grow but not shrink the window */
    gtk_window_set_policy(GTK_WINDOW(dialog.window), FALSE, TRUE, FALSE);
    /* window should appear in center of screen */
    gtk_window_set_position(GTK_WINDOW(dialog.window), GTK_WIN_POS_CENTER);
    /* set title */
    gtk_window_set_title(GTK_WINDOW(dialog.window), title);
    /* display message */
    label = gtk_label_new(msg);
    gtk_misc_set_padding(GTK_MISC(label), 15, 5);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog.window)->vbox), label, FALSE,
	TRUE, 0);

    /* a separator */
    gtk_hseparator_new_in_box(GTK_DIALOG(dialog.window)->vbox, 0);

    /* create a notebook to hold pin, signal, and parameter lists */
    notebk = gtk_notebook_new();
    /* add the notebook to the dialog */
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog.window)->vbox), notebk, TRUE,
	TRUE, 0);
    /* set overall notebook parameters */
    gtk_notebook_set_homogeneous_tabs(GTK_NOTEBOOK(notebk), TRUE);
    gtk_signal_connect(GTK_OBJECT(notebk), "switch-page", GTK_SIGNAL_FUNC(change_page), &dialog);
    /* text for tab labels */
    tab_label_text[0] = _("Pins");
    tab_label_text[1] = _("Signals");
    tab_label_text[2] = _("Parameters");
    /* loop to create three identical tabs */
    for (n = 0; n < 3; n++) {
	/* Create a scrolled window to display the list */
	vert->windows[n] = gtk_scrolled_window_new(NULL, NULL);
	vert->adjs[n] = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(vert->windows[n]));
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(vert->windows[n]),
	    GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_widget_show(vert->windows[n]);
	/* create a list to hold the data */
	vert->lists[n] = gtk_clist_new(1);
	/* set up a callback for when the user selects a line */
	gtk_signal_connect(GTK_OBJECT(vert->lists[n]), "select_row",
	    GTK_SIGNAL_FUNC(selection_made), &dialog);
	gtk_signal_connect(GTK_OBJECT(vert->lists[n]), "key-press-event",
	    GTK_SIGNAL_FUNC(search_for_entry), &dialog);
	/* It isn't necessary to shadow the border, but it looks nice :) */
	gtk_clist_set_shadow_type(GTK_CLIST(vert->lists[n]), GTK_SHADOW_OUT);
	/* set list for single selection only */
	gtk_clist_set_selection_mode(GTK_CLIST(vert->lists[n]),
	    GTK_SELECTION_BROWSE);
	/* put the list into the scrolled window */
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW
	    (vert->windows[n]), vert->lists[n]);
	/* another way to do it - not sure which is better
	gtk_container_add(GTK_CONTAINER(vert->windows[n]), vert->lists[n]); */
	gtk_widget_show(vert->lists[n]);
	/* create a box for the tab label */
	hbox = gtk_hbox_new(TRUE, 0);
	/* create a label for the page */
	gtk_label_new_in_box(tab_label_text[n], hbox, TRUE, TRUE, 0);
	gtk_widget_show(hbox);
	/* add page to the notebook */
	gtk_notebook_append_page(GTK_NOTEBOOK(notebk), vert->windows[n], hbox);
	/* set tab attributes */
	gtk_notebook_set_tab_label_packing(GTK_NOTEBOOK(notebk), hbox,
	    TRUE, TRUE, GTK_PACK_START);
    }
    /* determine initial page: pin, signal, or parameter */
    if (( chan->data_source_type >= 0 ) && ( chan->data_source_type <= 2 )) {
	initial_page = chan->data_source_type;
	gtk_notebook_set_page(GTK_NOTEBOOK(notebk), initial_page);
    } else {
	initial_page = -1;
	gtk_notebook_set_page(GTK_NOTEBOOK(notebk), 0);
    }
    gtk_widget_show(notebk);

    /* populate the pin, signal, and parameter lists */
    gtk_clist_clear(GTK_CLIST(vert->lists[0]));
    gtk_clist_clear(GTK_CLIST(vert->lists[1]));
    gtk_clist_clear(GTK_CLIST(vert->lists[2]));

// pins
    foreach_args_t args1 =  {
	.type = HAL_PIN,
    .user_arg1 = chan_num,
    .user_arg2 = initial_page,
    .user_arg3 = initial_row,
    .user_arg4 = max_row
    };

    halg_foreach(true, &args1, fill_list);

// signals
    foreach_args_t args2 =  {
	.type = HAL_SIGNAL,
    .user_arg1 = chan_num,
	.user_arg2 = args1.user_arg2,
    .user_arg3 = args1.user_arg3,
    .user_arg4 = args1.user_arg4
    };

    halg_foreach(true, &args2, fill_list);

// params
   foreach_args_t args3 =  {
	.type = HAL_PARAM,
    .user_arg1 = chan_num,
    .user_arg2 = args2.user_arg2,
    .user_arg3 = args2.user_arg3,
    .user_arg4 = args2.user_arg4
    };

    halg_foreach(true, &args3, fill_list);

    initial_page = args3.user_arg2;
    initial_row = args3.user_arg3;
    max_row = args3.user_arg4;

    // if initial_page was not set, ensure a valid page
    if(initial_page == -1) initial_page = 0;

    if ( initial_row >= 0 ) {
	/* highlight the currently selected name */
	gtk_clist_select_row(GTK_CLIST(vert->lists[initial_page]), initial_row, -1);
	/* set scrolling window to show the highlighted name */
	/* FIXME - I can't seem to get this to work */
	adj = vert->adjs[initial_page];
	adj->value = adj->lower + (adj->upper - adj->lower)*((double)(initial_row)/(double)(max_row+1));
	gtk_adjustment_value_changed(vert->adjs[initial_page]);
    }
    /* set up a callback function when the window is destroyed */
    gtk_signal_connect(GTK_OBJECT(dialog.window), "destroy",
	GTK_SIGNAL_FUNC(dialog_generic_destroyed), &dialog);
    /* make Cancel button */
    button = gtk_button_new_with_label(_("Cancel"));
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog.window)->action_area),
	button, TRUE, TRUE, 4);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
	GTK_SIGNAL_FUNC(dialog_generic_button2), &dialog);
    /* make window transient and modal */
    gtk_window_set_transient_for(GTK_WINDOW(dialog.window),
	GTK_WINDOW(ctrl_usr->main_win));
    gtk_window_set_modal(GTK_WINDOW(dialog.window), TRUE);
    gtk_widget_show_all(dialog.window);
    gtk_main();
    /* we get here when the user makes a selection, hits Cancel, or closes
       the window */
    vert->lists[0] = NULL;
    vert->lists[1] = NULL;
    vert->lists[2] = NULL;
    if ((dialog.retval == 0) || (dialog.retval == 2)) {
	/* user either closed dialog, or hit cancel */
	return FALSE;
    }
    /* user made a selection */
    channel_changed();
    return TRUE;
}

/* If we come here, then the user has clicked a row in the list. */
static void selection_made(GtkWidget * clist, gint row, gint column,
    GdkEventButton * event, dialog_generic_t * dptr)
{
    GdkEventType type;

    if ((event == NULL) || (clist == NULL)) {
	/* We get spurious events when the lists are populated I don't know
	   why.  If either clist or event is null, it's a bad one! */
	return;
    }
    type = event->type;
    if (type != 4) {
	/* We also get bad callbacks if you drag the mouse across the list
	   with the button held down.  They can be distinguished because
	   their event type is 3, not 4. */
	return;
    }
    selection_made_common(clist, row, dptr);
}

void channel_changed(void)
{
    scope_vert_t *vert;
    scope_chan_t *chan;
    GtkAdjustment *adj;
    gchar *name;
    gchar buf1[BUFLEN + 1], buf2[BUFLEN + 1];

    vert = &(ctrl_usr->vert);
    if ((vert->selected < 1) || (vert->selected > 16)) {
	gtk_label_set_text_if(vert->scale_label, "----");
	gtk_label_set_text_if(vert->chan_num_label, "--");
	gtk_label_set_text_if(vert->source_name_label, "------");
	request_display_refresh(1);
	return;
    }
    chan = &(ctrl_usr->chan[vert->selected - 1]);
    /* set position slider based on new channel */
    gtk_adjustment_set_value(GTK_ADJUSTMENT(vert->pos_adj),
	chan->position * VERT_POS_RESOLUTION);
    /* set scale slider based on new channel */
    adj = GTK_ADJUSTMENT(vert->scale_adj);
    adj->lower = chan->min_index;
    adj->upper = chan->max_index;
    adj->value = chan->scale_index;
    gtk_adjustment_changed(adj);
    gtk_adjustment_value_changed(adj);
    /* update the channel number and name display */
    snprintf(buf1, BUFLEN, "%2d", vert->selected);
    name = chan->name;
    gtk_label_set_text_if(vert->chan_num_label, buf1);
    gtk_label_set_text_if(vert->source_name_label, name);
    /* update the offset display */
    if (chan->data_type == HAL_BIT) {
	snprintf(buf1, BUFLEN, "----");
    } else {
        if(chan->ac_offset) {
            snprintf(buf1, BUFLEN, "(AC)");
        } else {
            format_signal_value(buf1, BUFLEN, chan->vert_offset);
        }
    }
    snprintf(buf2, BUFLEN, _("Offset\n%s"), buf1);
    gtk_label_set_text_if(vert->offset_label, buf2);
    request_display_refresh(1);
}

void format_scale_value(char *buf, int buflen, double value)
{
    char *units;
    char symbols[] = "pnum KMGT";

    if (value < 0.9e-12) {
	/* less than pico units, shouldn't happen */
	snprintf(buf, buflen, "tiny");
	return;
    }
    if (value > 1.1e+12) {
	/* greater than tera-units, shouldn't happen */
	snprintf(buf, buflen, "huge");
	return;
    }
    units = &(symbols[4]);
    while (value < 1.0) {
	value *= 1000.0;
	units--;
    }
    while (value >= 999.99) {
	value *= 0.001;
	units++;
    }
    snprintf(buf, buflen, "%0.0f%c/div", value, *units);
}

static void write_chan_config(FILE *fp, scope_chan_t *chan)
{
    if ( chan->data_source_type == 0 ) {
	// pin
	fprintf(fp, "PIN %s\n", chan->name);
    } else if ( chan->data_source_type == 1 ) {
	// signal
	fprintf(fp, "SIG %s\n", chan->name);
    } else if ( chan->data_source_type == 2 ) {
	// pin
	fprintf(fp, "PARAM %s\n", chan->name);
    } else {
	// not configured
	return;
    }
    fprintf(fp, "VSCALE %d\n", chan->scale_index);
    fprintf(fp, "VPOS %f\n", chan->position);
    if(chan->ac_offset) {
        fprintf(fp, "VAC %e\n", chan->vert_offset);
    } else {
        fprintf(fp, "VOFF %e\n", chan->vert_offset);
    }
}
