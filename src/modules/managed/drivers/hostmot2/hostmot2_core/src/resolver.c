
//
//    Copyright (C) 2011 Andy Pugh / Kim Kirwan
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//


//
//  Driver for the Mesa 7i49 hex-Resolver card.
//


#include "user_pci/config_module.h"
#include RTAPI_INC_SLAB_H

#include "runtime/rtapi.h"
#include "runtime/rtapi_app.h"
#include "runtime/rtapi_string.h"
#include "runtime/rtapi_math.h"

#include "hal/hal.h"

#include "hostmot2/hostmot2.h"

int hm2_resolver_get_param(int param){
    // This function will eventually return parameters from the 7i49
    // but for now, it just returns the number 6
    return 6;
}

int hm2_resolver_parse_md(hostmot2_t *hm2, int md_index) {
    hm2_module_descriptor_t *md = &hm2->md[md_index];
    int i, r = 0;
    int resolvers_per_instance;

    //
    // some standard sanity checks
    //

    if ( ! hm2_md_is_consistent_or_complain(hm2, md_index, 0, 5, 4, 0x001F)) {
        HM2_ERR("inconsistent resolver Module Descriptor!\n");
        return -EINVAL;
    }

    if (hm2->resolver.num_instances != 0) {
        HM2_ERR(
                "found duplicate Module Descriptor for %s (inconsistent firmware), not loading driver\n",
                hm2_get_general_function_name(md->gtag)
                );
        return -EINVAL;
    }

    resolvers_per_instance = hm2_resolver_get_param(2); // just returns 6 at the moment

    if (hm2->config.num_resolvers > (md->instances * resolvers_per_instance)) {
        HM2_ERR(
                "config.num_resolvers=%d, but only %d are available, not loading driver\n",
                hm2->config.num_resolvers,
                md->instances * resolvers_per_instance);
        return -EINVAL;
    }

    if (hm2->config.num_resolvers == 0) {
        return 0;
    }


    //
    // looks good, start initializing
    //

    /*At the moment it is not clear if there will ever be more than one resolver
     instance. If there were to be more than one then they would need to have
     a different base-address, and this code would need to be re-enterable.
     A bridge to cross when we come to it */

    if (hm2->config.num_resolvers == -1) {
        hm2->resolver.num_resolvers = md->instances * resolvers_per_instance;
        hm2->resolver.num_instances = md->instances;
    } else {
        hm2->resolver.num_resolvers = hm2->config.num_resolvers;
        hm2->resolver.num_instances = md->instances;
    }

    hm2->resolver.hal = (hm2_resolver_global_t *)hal_malloc(
                                                sizeof(hm2_resolver_global_t));
    if (hm2->resolver.hal == NULL) {
        HM2_ERR("out of memory!\n");
        r = -ENOMEM;
        goto fail0;
    }
    hm2->resolver.instance = (hm2_resolver_instance_t *)hal_malloc(
                hm2->resolver.num_resolvers * sizeof(hm2_resolver_instance_t));
    if (hm2->resolver.instance == NULL) {
        HM2_ERR("out of memory!\n");
        r = -ENOMEM;
        goto fail0;
    }

    for (i = 0 ; i < hm2->resolver.num_instances ; i++ ){
        hm2->resolver.stride = md->register_stride;
        hm2->resolver.clock_frequency = md->clock_freq;
        hm2->resolver.version = md->version;

        hm2->resolver.command_addr = md->base_address + (0 * md->register_stride);
        hm2->resolver.data_addr = md->base_address + (1 * md->register_stride);
        hm2->resolver.status_addr = md->base_address + (2 * md->register_stride);
        hm2->resolver.velocity_addr = md->base_address + (3 * md->register_stride);
        hm2->resolver.position_addr = md->base_address + (4 * md->register_stride);

        // If there were multiple resolver function instances, this would need
        // to be the number of resolvers for that particular instance
        r = hm2_register_tram_read_region(hm2, hm2->resolver.status_addr,
                                          sizeof(u32),
                                          &hm2->resolver.status_reg);
        r += hm2_register_tram_read_region(hm2, hm2->resolver.position_addr,
                                          (hm2->resolver.num_resolvers * sizeof(u32)),
                                          &hm2->resolver.position_reg);
        r += hm2_register_tram_read_region(hm2, hm2->resolver.velocity_addr,
                                          (hm2->resolver.num_resolvers * sizeof(u32)),
                                          (u32**)&hm2->resolver.velocity_reg);
        if (r < 0) {
            HM2_ERR("error registering tram read region for Resolver "
                    "register (%d)\n", i);
            goto fail1;
        }

    }

    // export the resolvers to HAL

    {
        int i;
        int ret;
        char name[HAL_NAME_LEN + 1];

        rtapi_snprintf(name, sizeof(name), "%s.resolver.excitation-khz",
                       hm2->llio->name);
        ret= hal_pin_float_new(name, HAL_IO,
                                 &(hm2->resolver.hal->pin.excitation_khz),
                                 hm2->llio->comp_id);
        if (ret < 0) {
            HM2_ERR("error adding pin '%s', aborting\n", name);
            goto fail1;
        }

        for (i = 0; i < hm2->resolver.num_resolvers; i ++) {
            // pins

            rtapi_snprintf(name, sizeof(name), "%s.resolver.%02d.position",
                           hm2->llio->name, i);
            ret= hal_pin_float_new(name, HAL_OUT,
                                   &(hm2->resolver.instance[i].hal.pin.position),
                                   hm2->llio->comp_id);
            if (ret < 0) {
                HM2_ERR("error adding pin '%s', aborting\n", name);
                goto fail1;
            }

            rtapi_snprintf(name, sizeof(name), "%s.resolver.%02d.angle",
                           hm2->llio->name, i);
            ret= hal_pin_float_new(name, HAL_OUT,
                                   &(hm2->resolver.instance[i].hal.pin.angle),
                                   hm2->llio->comp_id);
            if (ret < 0) {
                HM2_ERR("error adding pin '%s', aborting\n", name);
                goto fail1;
            }

            rtapi_snprintf(name, sizeof(name), "%s.resolver.%02d.velocity",
                           hm2->llio->name, i);
            ret= hal_pin_float_new(name, HAL_OUT,
                                   &(hm2->resolver.instance[i].hal.pin.velocity),
                                   hm2->llio->comp_id);
            if (ret < 0) {
                HM2_ERR("error adding pin '%s', aborting\n", name);
                goto fail1;
            }

            rtapi_snprintf(name, sizeof(name), "%s.resolver.%02d.count",
                           hm2->llio->name, i);
            ret= hal_pin_s32_new(name, HAL_OUT,
                                 &(hm2->resolver.instance[i].hal.pin.count),
                                 hm2->llio->comp_id);
            if (ret < 0) {
                HM2_ERR("error adding pin '%s', aborting\n", name);
                goto fail1;
            }

            rtapi_snprintf(name, sizeof(name), "%s.resolver.%02d.rawcounts",
                           hm2->llio->name, i);
            ret= hal_pin_s32_new(name, HAL_OUT,
                                 &(hm2->resolver.instance[i].hal.pin.rawcounts),
                                 hm2->llio->comp_id);
            if (ret < 0) {
                HM2_ERR("error adding pin '%s', aborting\n", name);
                goto fail1;
            }

            rtapi_snprintf(name, sizeof(name), "%s.resolver.%02d.reset",
                           hm2->llio->name, i);
            ret= hal_pin_bit_new(name, HAL_IN,
                                 &(hm2->resolver.instance[i].hal.pin.reset),
                                 hm2->llio->comp_id);
            if (ret < 0) {
                HM2_ERR("error adding pin '%s', aborting\n", name);
                goto fail1;
            }

            rtapi_snprintf(name, sizeof(name), "%s.resolver.%02d.index-enable",
                           hm2->llio->name, i);
            ret= hal_pin_bit_new(name, HAL_IO,
                                 &(hm2->resolver.instance[i].hal.pin.index_enable),
                                 hm2->llio->comp_id);
            if (ret < 0) {
                HM2_ERR("error adding pin '%s', aborting\n", name);
                goto fail1;
            }

            rtapi_snprintf(name, sizeof(name), "%s.resolver.%02d.error",
                           hm2->llio->name, i);
            ret= hal_pin_bit_new(name, HAL_OUT,
                                 &(hm2->resolver.instance[i].hal.pin.error),
                                 hm2->llio->comp_id);
            if (ret < 0) {
                HM2_ERR("error adding pin '%s', aborting\n", name);
                goto fail1;
            }


            // parameters
            rtapi_snprintf(name, sizeof(name), "%s.resolver.%02d.scale",
                           hm2->llio->name, i);
            ret= hal_pin_float_new(name, HAL_IO,
                                     &(hm2->resolver.instance[i].hal.pin.scale),
                                     hm2->llio->comp_id);
            if (ret < 0) {
                HM2_ERR("error adding pin '%s', aborting\n", name);
                goto fail1;
            }

            rtapi_snprintf(name, sizeof(name), "%s.resolver.%02d.velocity-scale",
                           hm2->llio->name, i);
            ret= hal_pin_float_new(name, HAL_IO,
                                     &(hm2->resolver.instance[i].hal.pin.vel_scale),
                                     hm2->llio->comp_id);
            if (ret < 0) {
                HM2_ERR("error adding pin '%s', aborting\n", name);
                goto fail1;
            }

            rtapi_snprintf(name, sizeof(name), "%s.resolver.%02d.index-divisor",
                           hm2->llio->name, i);
            ret= hal_pin_u32_new(name, HAL_IO,
                                     &(hm2->resolver.instance[i].hal.pin.index_div),
                                     hm2->llio->comp_id);
            if (ret < 0) {
                HM2_ERR("error adding pin '%s', aborting\n", name);
                goto fail1;
            }

            //
            // init the hal objects that need it
            // the things not initialized here will be set by hm2_resolver_tram_init()
            //

            *hm2->resolver.instance[i].hal.pin.reset = 0;
            *hm2->resolver.instance[i].hal.pin.scale = 1.0;
            *hm2->resolver.instance[i].hal.pin.vel_scale = 1.0;
            *hm2->resolver.instance[i].hal.pin.index_div = 1;
            *hm2->resolver.hal->pin.excitation_khz = -1; // don't-write
            hm2->resolver.kHz = (hm2->resolver.clock_frequency / 5000);
        }
    }


    return hm2->resolver.num_instances;

fail1:
    // This is where we would kfree anything kmalloced.


fail0:
    hm2->resolver.num_instances = 0;
    return r;
}

void hm2_resolver_process_tram_read(hostmot2_t *hm2, long period) {
    int i;
    hm2_resolver_instance_t *res;

    if (hm2->resolver.num_instances <= 0) return;

    // process each resolver instance independently
    for (i = 0; i < hm2->resolver.num_resolvers; i ++) {

        res = &hm2->resolver.instance[i];

        // sanity check
        if (*res->hal.pin.scale == 0.0) {
            HM2_ERR("resolver.%02d.scale == 0.0, bogus, setting to 1.0\n", i);
            *(res->hal.pin.scale) = 1.0;
        }
        if (*res->hal.pin.vel_scale == 0.0) {
            HM2_ERR("resolver.%02d.velocity-scale == 0.0, bogus, setting to 1.0\n", i);
            *(res->hal.pin.vel_scale) = 1.0;
        }

        // PROCESS THE REGISTERS, SET THE PINS

        res->accum += (__s32)(hm2->resolver.position_reg[i] - res->old_reg );

        if ((res->old_reg > hm2->resolver.position_reg[i]) && (res->old_reg - hm2->resolver.position_reg[i] > 0x80000000)){
            res->index_cnts++;
            if (*res->hal.pin.index_enable){
                int r = (res->index_cnts % *res->hal.pin.index_div);
                if ((*res->hal.pin.index_div  > 1 && r == 1)
                 || (*res->hal.pin.index_div == 1 && r == 0)){
                    res->offset = (res->accum - hm2->resolver.position_reg[i]);
                    *res->hal.pin.index_enable = 0;
                }
            }
        }
        else if ((res->old_reg < hm2->resolver.position_reg[i]) && (hm2->resolver.position_reg[i] - res->old_reg > 0x80000000)){
            res->index_cnts--;
            if (*res->hal.pin.index_enable && (res->index_cnts % *res->hal.pin.index_div == 0)){
                res->offset = (res->accum - hm2->resolver.position_reg[i] + 0x100000000LL);
                *res->hal.pin.index_enable = 0;
            }
        }

        if (*res->hal.pin.reset){
            res->offset = res->accum;
        }

        res->old_reg = hm2->resolver.position_reg[i];

        *res->hal.pin.angle = hm2->resolver.position_reg[i] / 4294967296.0;
        *res->hal.pin.rawcounts = (res->accum >> 8);
        *res->hal.pin.count = (res->accum - res->offset) >> 8;
        *res->hal.pin.position = (res->accum - res->offset) / 4294967296.0
                                 * *res->hal.pin.scale;
        *res->hal.pin.velocity = ((hm2->resolver.velocity_reg[i] / 4294967296.0)
                                  * hm2->resolver.kHz * *res->hal.pin.vel_scale);
        *res->hal.pin.error = *hm2->resolver.status_reg & (1 << i);
    }
}

void hm2_resolver_write(hostmot2_t *hm2, long period){
    //This function needs to handle comms handshaking, so is written as a state machine
    static int state = 0;
    static u32 cmd_val, data_val;
    static u32 timer;
    u32 buff;

    if (hm2->resolver.num_instances <= 0) return;

    switch (state){
        case 0: // Idle/waiting
            if (*hm2->resolver.hal->pin.excitation_khz < 0){
                return;
            }
            if (*hm2->resolver.hal->pin.excitation_khz != hm2->resolver.written_khz){
                if (*hm2->resolver.hal->pin.excitation_khz > 8){
                    *(hm2->resolver.hal->pin.excitation_khz) = 10;
                    hm2->resolver.written_khz = 10;
                    hm2->resolver.kHz = (hm2->resolver.clock_frequency / 5000);
                    cmd_val = 0x803;
                } else if (*hm2->resolver.hal->pin.excitation_khz > 4){
                    *(hm2->resolver.hal->pin.excitation_khz) = 5;
                    hm2->resolver.written_khz = 5;
                    hm2->resolver.kHz = (hm2->resolver.clock_frequency / 10000);
                    cmd_val = 0x802;
                }else{
                    *(hm2->resolver.hal->pin.excitation_khz) = 2.5;
                    hm2->resolver.written_khz = 2.5;
                    hm2->resolver.kHz= (hm2->resolver.clock_frequency / 20000);
                    cmd_val = 0x801;
                }
                state = 10;
                timer = 0;
                return;
            }
            break;
        case 10: // wait for command register clear before setting pins
            hm2->llio->read(hm2->llio,hm2->resolver.command_addr,
                            &buff, sizeof(u32));
            if (buff){
                timer += period;
                if (timer > 1e9){
                    HM2_ERR("Command not cleared in hm2_resolver, setting aborted");
                    state = 0;
                }
                return;
            }
            hm2->llio->write(hm2->llio, hm2->resolver.data_addr,
                             &data_val,sizeof(u32));
            hm2->llio->write(hm2->llio, hm2->resolver.command_addr,
                             &cmd_val,sizeof(u32));
            state = 20;
            timer = 0;
            return;
        case 20: // wait for command to clear before processing any more pins
            hm2->llio->read(hm2->llio,hm2->resolver.command_addr,
                            &buff, sizeof(u32));
            if (buff){
                timer += period;
                if (timer > 1e9){
                    HM2_ERR("Command not cleared after setting in hm2_resolver");
                    state = 0;
                }
                return;
            }
            state = 0;
            return;
            break;
        default: // That's odd
            HM2_ERR("hm2_resolver, unexpected / illegal state in comms state"
                    "machine");
    }
}

void hm2_resolver_cleanup(hostmot2_t *hm2) {
    if (hm2->resolver.num_instances <= 0) return;
    // nothing kmallocced, so nothing to kfree
}


void hm2_resolver_print_module(hostmot2_t *hm2) {
    int i;
    HM2_PRINT("resolvers: %d\n", hm2->resolver.num_instances);
    if (hm2->resolver.num_instances <= 0) return;
    HM2_PRINT("    clock_frequency: %d Hz (%s MHz)\n",
              hm2->resolver.clock_frequency,
              hm2_hz_to_mhz(hm2->resolver.clock_frequency));
    HM2_PRINT("    version: %d\n", hm2->resolver.version);
    HM2_PRINT("    position_addr: 0x%04X\n", hm2->resolver.position_addr);
    HM2_PRINT("    velocity_addr: 0x%04X\n", hm2->resolver.velocity_addr);
    for (i = 0; i < hm2->resolver.num_instances; i ++) {
        HM2_PRINT("    instance %d:\n", i);
        HM2_PRINT("        hw:\n");
        HM2_PRINT("            position = %08x\n",
                  (hm2->resolver.position_reg[i]));
        HM2_PRINT("            velocity = %08x\n",
                  (hm2->resolver.velocity_reg[i]));
    }
}
