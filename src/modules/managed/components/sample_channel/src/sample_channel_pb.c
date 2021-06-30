/*****************************************************************************

    Instantiatiable recorder with configurable pins
    
    This recorder puts pin values with a timestamp in a protobuf encoded message
    in a HAL ringbuffer.
    
    See also:
    * sample_channel.hal
    * show_sample.py
    * sample.proto
    
    Usage:
    newinst sample_channel_pb sampler --- samples=bbfusUS cycles=10
    
    This will create an instance "sampler" with pins and a ring.
    The ring can contain at least 10 cycles of the record_sample.funct
    Pins:
    * 2 bits
    * 1 float
    * 1 unsigned 32
    * 1 signed 32
    * 1 unsigned 64
    * 1 signed 64
    Ring:
    * sampler.ring

    Optionally:
    * you can name the pins at instantiation time like so:
      newinst sample_channel_pb sampler --- samples=bff pinnames=foo,bar,baz cycles=10
    
      this will than create the following pins of types:
      sampler.foo -> bit
      sampler.bar -> float
      sampler.baz -> float

    Author: Bas de Bruijn <bdebruijnATluminizeDOTnl>
    License: GPL Version 2

    Copyright (c) 2019 All rights reserved.
    
    This program is free software; you can redistribute it and/or
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

*****************************************************************************/


#include "rtapi.h"
#include "rtapi_app.h"
#include "rtapi_string.h"
#include "hal/hal.h"
#include "hal/hal_priv.h"
#include "hal/hal_ring.h"

#include <stdlib.h>  /* for atoi() */
#include <stdio.h>   // sprintf()
#include <machinetalk/nanopb/pb_encode.h>
#include <machinetalk/build/machinetalk/protobuf/sample.npb.h>

MODULE_AUTHOR("Bas de Bruijn");
MODULE_DESCRIPTION("Instantiable configurable recorder to a HAL ring");
MODULE_LICENSE("GPLv2");

RTAPI_TAG(HAL,HC_INSTANTIABLE);

#define MAX_PINS    64        // maximum amount of pins, see also sample.proto
#define RB_HEADROOM 1.2       // ringbuffer size overallocation

static int comp_id;
static char *compname = "sample_channel_pb";

struct inst_data{
    // pins
    hal_bit_t		*record;			// pin: when high -> start recording
    hal_data_u		*pins_in[MAX_PINS];	// pin: array of sample values
    hal_u32_t 		*send_fail;		    // error counter on the send side
    hal_u32_t       *dropped;           // dropped sampled due to full ring;
    char            pinnames[MAX_PINS][HAL_NAME_LEN];  //contains names
  
    // the ringbuffer
    ringbuffer_t    sample_ring;
    size_t          sample_size;

    // other instance data
    char            samples_cfg[MAX_PINS];      // the samples config string
    int             nr_samples;                 // the number of samples
    int             cycles;                     // amount of cycles

    const char      *name[HAL_NAME_LEN + 1];    // the name of this instance
};

// copied from pbring.c
enum {
    NBP_SIZE_FAIL   = -100,
    NBP_ENCODE_FAIL = -101,
    NBP_DECODE_FAIL = -102,
    RB_RESERVE_FAIL = -103,
    RB_WRITE_FAIL   = -104,
} rtmsg_errors_t;


// copied from pbring.c and adapted where needed
//
// encode message struct in msg according to 'fields', and send off via rb
// return 0 on success or < 0 on failure; see rtmsg_errors_t
//
static int npb_send_msg(const void *msg, const pb_field_t *fields,
			            ringbuffer_t *rb, const hal_funct_args_t *fa)
{
    void *buffer;
    int retval;
    size_t size;

    // determine size
    pb_ostream_t sstream = PB_OSTREAM_SIZING;
    if (!pb_encode(&sstream, fields,  msg)) {
	    rtapi_print_msg(RTAPI_MSG_ERR,
			"%s: sizing pb_encode(): %s written=%zu\n",
			fa_funct_name(fa), PB_GET_ERROR(&sstream), sstream.bytes_written);
	    return NBP_SIZE_FAIL;
    }
    size = sstream.bytes_written;

    // preallocate memory in ringbuffer
    if ((retval = record_write_begin(rb, (void **)&buffer, size)) != 0) {
	    rtapi_print_msg(RTAPI_MSG_ERR,
            "%s: record_write_begin(%zu) failed: %d\n", fa_funct_name(fa),
            size, retval);
	    return RB_RESERVE_FAIL;
    }

    // zero-copy encode directly into ringbuffer
    pb_ostream_t rstream = pb_ostream_from_buffer(buffer, size);
    if (!pb_encode(&rstream, fields,  msg)) {
	    rtapi_print_msg(RTAPI_MSG_ERR,
			"%s: pb_encode failed: %s, size=%zu written=%zu\n",
            fa_funct_name(fa), PB_GET_ERROR(&rstream), size,
            rstream.bytes_written);
	    return NBP_ENCODE_FAIL;
    }

    // send it off
    if ((retval = record_write_end(rb, buffer, rstream.bytes_written))) {
	    rtapi_print_msg(RTAPI_MSG_ERR,
			"%s: record_write_end(%zu) failed: %d\n", fa_funct_name(fa),
            size, retval);
	    return RB_WRITE_FAIL;
    }
    return 0;
}


static int record_sample(void *arg, const hal_funct_args_t *fa)
{
    int i=0;
    int retval;
    // get hold of the instance
    struct inst_data *ip = arg;
    if (*(ip->record) == true) {
        // read the values of the pins ip->pins_in[i], up to nr_samples
        // depending on the type, put value in ip->sample->arr_sample
        // pins are made in order of the config string
        for (i = 0; i < ip->nr_samples; i++) {
            machinetalk_Sample tx_sample = machinetalk_Sample_init_zero;
            tx_sample.has_timestamp = true;
            tx_sample.timestamp = fa_thread_start_time(fa);
            tx_sample.has_v_string = true;
            strcpy(tx_sample.v_string, ip->pinnames[i]);
            switch (ip->samples_cfg[i])
            {
            case 'b':
            case 'B':
                tx_sample.has_v_bool = true;
                tx_sample.v_bool = get_bit_value(ip->pins_in[i]);
                break;
            case 'f':
            case 'F':
                tx_sample.has_v_double = true;
                tx_sample.v_double = get_float_value(ip->pins_in[i]);
                break;
            case 'u':
                tx_sample.has_v_uint32 = true;
                tx_sample.v_uint32 = get_u32_value(ip->pins_in[i]);
                break;
            case 's':
                tx_sample.has_v_int32 = true;
                tx_sample.v_int32 = get_s32_value(ip->pins_in[i]);
                break;
            case 'U':
                tx_sample.has_v_uint64 = true;
                tx_sample.v_uint64 = get_u64_value(ip->pins_in[i]);
                break;
            case 'S':
                tx_sample.has_v_int64 = true;
                tx_sample.v_int64 = get_s64_value(ip->pins_in[i]);
                break;
            }
            // copied from pbring.c and adapted as needed
            // sample message has been filled and can now be sent off.
            if ((retval = npb_send_msg(&tx_sample, machinetalk_Sample_fields,
                        &ip->sample_ring, fa))) {
                    rtapi_print_msg(RTAPI_MSG_ERR,
                    "%s: sending of message failed = %d", compname, retval);
                *(ip->send_fail) +=1;
            } 
        }
    }
    return 0;
}


static int calc_sampling_config(struct inst_data *ip)
{
    int i=0, validchar=0;
    char character;
    // track the amount of valid characters
    validchar = 0;
    for (i = 0; (character = ip->samples_cfg[i]); i++) {
        switch (character) {
            case 'b':
            case 'B':
                validchar++;
                break;
            case 'f':
            case 'F':
                validchar++;
                break;
            case 'u':
                validchar++;
                break;
            case 's':
                validchar++;
                break;
            case 'U':
                validchar++;
                break;
            case 'S':
                validchar++;
                break;
            default:
                rtapi_print_msg(RTAPI_MSG_ERR, "invalid character in "
                        "\"samples=\" string. Needs to be b,f,s,u,S or U\n");
                hal_exit(comp_id);
                return -1;
        }
        ip->nr_samples = validchar;
    }
    return 0;
}


static int export_pins(struct inst_data *ip, const char *name)
{
    int retval=0, i=0, idx=0;
    int indexes[6] = {0,0,0,0,0,0};
    char *pin_type;
    char character = '-';
    char pname[HAL_MAX_NAME_LEN];
    hal_type_t type = HAL_TYPE_UNSPECIFIED;

    // some debug output
    for (i = 0; (i < ip->nr_samples); i++) {
        hal_print_msg(RTAPI_MSG_DBG,"%s: ip->samples_cfg[%d] = %c",
            compname, i, ip->samples_cfg[i]);
    }

    // traverse ip->samples_cfg array
    // create and name the pin depending on the type and consecutive number
    for (i = 0; i < ip->nr_samples; i++) {

        character = ip->samples_cfg[i];
        hal_print_msg(RTAPI_MSG_DBG,"%s: ip->samples_cfg[%d] = %c", compname,
            i, character);
        switch (character) {
            case 'b':
            case 'B':
                pin_type = "bit";
                type = HAL_BIT;
                idx = 0;
                break;
            case 'f':
            case 'F':
                pin_type = "flt";
                type = HAL_FLOAT;
                idx = 1;
                break;
            case 'u':
                pin_type = "u32";
                type = HAL_U32;
                idx = 2;
                break;
            case 's':
                pin_type = "s32";
                type = HAL_S32;
                idx = 3;
                break;
            case 'U':
                pin_type = "u64";
                type = HAL_U64;
                idx = 4;
                break;
            case 'S':
                pin_type = "s64";
                type = HAL_S64;
                idx = 5;
                break;
            case '-':
            default:
                type = HAL_TYPE_UNSPECIFIED;
                hal_print_msg(RTAPI_MSG_ERR,
                    "%s: invalid type '%c' for pin %d\n", compname, type, i);
                hal_exit(comp_id);
                return -1;
        }
        indexes[idx]++;
        // create sample pins names
        if ( *ip->pinnames[i] == '\0' )
        {
            sprintf(pname, "%s.in-%s.%d", name, pin_type, indexes[idx]);
            strcpy(ip->pinnames[i], pname);
            hal_print_msg(RTAPI_MSG_DBG,
                "%s: new pin name %s", name, pname);
        }
        if ((retval = hal_pin_newf(type, HAL_IN, (void **) &ip->pins_in[i],
                comp_id, "%s", ip->pinnames[i])) < 0) {
            return retval;
        }
        
	}
    // create misc pins
    if (((retval = hal_pin_bit_newf(HAL_IN, &(ip->record), comp_id,
				    "%s.record", name))) ||
        ((retval = hal_pin_u32_newf(HAL_IN, &(ip->dropped), comp_id,
				    "%s.dropped", name))) ||
    	((retval = hal_pin_u32_newf(HAL_OUT, &(ip->send_fail), comp_id,
				    "%s.send-fail", name)))) {
    	return retval;
    }
    ip->dropped = 0;
    return 0;
}


static int create_ring(struct inst_data *ip, const char *name)
{
    int retval=0;
    // determine the required size of the ringbuffer
    size_t sample_size = sizeof(machinetalk_Sample);
    size_t rbsize = (sample_size * ip->cycles) * RB_HEADROOM;

    // create the ring queue
    if ((retval = hal_ring_newf(rbsize, sample_size, RINGTYPE_RECORD,
			"%s.ring", name)) < 0) {
	    hal_print_msg(RTAPI_MSG_ERR,
		    "%s: failed to create new ring '%s.ring': %d\n", compname,
            name, retval);
	    return retval;
    }

    // attach to the command ringbuffer '<instancename>.ring' if it exists
    // must be record mode
    unsigned flags;
    if (!hal_ring_attachf(&(ip->sample_ring), &flags,  "%s.ring", name))
    {
		if ((flags & RINGTYPE_MASK) != RINGTYPE_RECORD) {
		    HALERR("ring %s.ring not a record mode ring: mode=%d", name,
            flags & RINGTYPE_MASK);
		    return -EINVAL;
		}
    } 
    else
    {
		HALERR("ring %s.ring does not exist", name);
		return -EINVAL;
    }
    return 0;
}


static int instantiate_sample_channel_pb(const int argc, const char **argv)
{
    struct inst_data *ip;
    const char *name = argv[1];
    int inst_id=0, i=0, j=0, cycles=0, pin_i=0, commas=0;
    char samples[MAX_PINS];
    char pname[HAL_MAX_NAME_LEN];
    char full_pname[HAL_MAX_NAME_LEN];
    char pinnames[(MAX_PINS * HAL_NAME_LEN) + (MAX_PINS - 1)];
    bool has_samples_string=false, has_cycles_string=false, has_pinnames_string=false;
    // pointers to character in string
    char *idx, *idx_prev;

    // parse arguments
    for(i = 2; i < argc; i++)
	{
        const char *argument = argv[i];
        hal_print_msg(RTAPI_MSG_ERR,
            "%s: %s: argument %d = %s\n", compname, name, i, argument);
        if((strstr(argument, "samples=")) != NULL)
        {
            strcpy(samples, &argument[8]);
            has_samples_string = true;
        }
        else if((strstr(argument, "cycles=")) != NULL)
        {
            cycles = atoi(&argument[7]);
            hal_print_msg(RTAPI_MSG_DBG, "cycles = %d", cycles);
            has_cycles_string = true;
        }
        else if((strstr(argument, "pinnames=")) != NULL)
        {
            strcpy(pinnames, &argument[9]);
            hal_print_msg(RTAPI_MSG_DBG, "pinnames = %s", pinnames);
            has_pinnames_string = true;
        }
	}
    // exit if no samples
    if (!has_samples_string)
    {
        hal_print_msg(RTAPI_MSG_ERR,
            "%s: %s: ERROR: sample pins must be specified\n",
            compname,
            name);
        return -1;
    }
    // exit if no cycles
    if (!has_cycles_string)
    {
        hal_print_msg(RTAPI_MSG_ERR,
            "%s: %s: ERROR: cycles (nr of samples on the ring) is hot given\n",
            compname, name);
        return -1;
    }
    // check if samples string is not void
    if (samples[0] == '\0')
    {
	    hal_print_msg(RTAPI_MSG_ERR,
            "%s: %s: ERROR: sample pins string is void\n", compname, name);
	    return -1;
	}
    // check pinnames basic sanity, needs to have samples-1 commas
    if (has_pinnames_string)
    {

        // get first part of string into the pinnames array
        if (pinnames[0] == '\0')
        {
            hal_print_msg(RTAPI_MSG_ERR,
                "%s: %s: ERROR: empty pin name found, check pinnames argument\n",
                compname, name);
            return -1;
        }
        // return the first occurence of a comma
        idx = strchr(pinnames, ',');
        while (idx!=NULL)
        {
            commas += 1;
            idx = strchr(idx + 1, ',');
        }

        if ((commas + 1) != strlen(samples))
        {
            hal_print_msg(RTAPI_MSG_ERR,
                "%s: %s: ERROR: pinnames string found, but amount does not match with samples\n",
                compname, name);
            return -1;
        }
	}

    // instantiate component
    if ((inst_id = hal_inst_create(name, comp_id,
				  sizeof(struct inst_data),
                  (void **)&ip)) < 0)
	    return -1;

    strcpy(ip->samples_cfg, samples);
    ip->cycles = cycles;
    hal_print_msg(RTAPI_MSG_DBG,"%s: %s: ip->samples_cfg set to %s",
        compname, name, ip->samples_cfg);
    // calculate amount of pins, types, values in instance data
    if (calc_sampling_config(ip) < 0){
	    hal_print_msg(RTAPI_MSG_ERR,
            "%s: %s: ERROR: setup sampling configuration failed\n",
            compname, name);
	    return -1;
    }
    hal_print_msg(RTAPI_MSG_DBG,
        "%s: %s: ip->nr_samples set to %d", compname, name, ip->nr_samples);
    for (j = 0; (j < ip->nr_samples); j++) {
        hal_print_msg(RTAPI_MSG_DBG,
            "%s: %s: ip->samples_cfg[%d] = %c", compname, name, j,
            ip->samples_cfg[j]);
    }
    hal_print_msg(RTAPI_MSG_DBG,
        "%s: %s: ip->samples_cfg is %s", compname, name, ip->samples_cfg);

    // copy pinnames in instance pinnames array, rough sanity check has been done
    if (has_pinnames_string)
    {
        // return the first occurence of a comma
        idx_prev = pinnames;
        idx = strchr(pinnames, ',');
        pin_i = 0;
        while (idx!=NULL)
        {
            // get first part of string into the pinnames array
            strncpy(pname, idx_prev, idx-idx_prev);
            // make sure we have terminating char, because content can be garbage
            pname[idx-idx_prev] = '\0';
            hal_print_msg(RTAPI_MSG_DBG,
                "%s: %s: pinname = %s", compname, name, pname);
            if (pname[0] == '\0')
            {
                hal_print_msg(RTAPI_MSG_ERR,
                    "%s: %s: ERROR: pin name is empty", compname, name);
                return -1;
            }
            sprintf(full_pname, "%s.%s", name, pname);
            strcpy(ip->pinnames[pin_i], full_pname);
            hal_print_msg(RTAPI_MSG_DBG,
                "%s: %s: pinname = %s", compname, name, ip->pinnames[pin_i]);
            idx_prev = idx + 1;
            pin_i += 1;
            idx = strchr(idx_prev, ',');
        }
        // the remaining part, after removing commas and pin names should not be empty
        strcpy(pname, idx_prev);
        sprintf(full_pname, "%s.%s", name, pname);
        strcpy(ip->pinnames[pin_i], full_pname);
	}

    // create the ring
	if ( create_ring(ip, name) < 0 ) {
	    hal_print_msg(RTAPI_MSG_ERR,
			"%s: %s: ERROR: creation of ring failed for instance \n",
            compname, name);
	    return -1;
	}
    hal_print_msg(RTAPI_MSG_DBG,
        "%s: %s: ip->samples_cfg is %s", compname, name, ip->samples_cfg);
    // export pins
	if ( export_pins(ip, name) < 0 ) {
	    hal_print_msg(RTAPI_MSG_ERR,
			"%s: %s: ERROR: export of pins failed for instance \n",
            compname, name);
	    return -1;
	}
    // export the function for using it on the thread
    hal_export_xfunct_args_t xfunct_args = {
        .type = FS_XTHREADFUNC,
        .funct.x = record_sample,
        .arg = ip,
        .uses_fp = 0,
        .reentrant = 0,
        .owner_id = inst_id
    };
    return hal_export_xfunctf(&xfunct_args, "%s.record_sample", name);
}


// entry point creating this instance
int rtapi_app_main(void)
{
    comp_id = hal_xinit(TYPE_RT, 0, 0,
        (hal_constructor_t)instantiate_sample_channel_pb, NULL, compname);
    if (comp_id < 0) return comp_id;
    hal_ready(comp_id);
    return 0;
}


void rtapi_app_exit(void)
{
    hal_exit(comp_id);
}
