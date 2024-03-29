
//
//    Copyright (C) 2007-2008 Sebastian Kuzminsky
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

#include "user_pci/config_module.h"
#include RTAPI_INC_SLAB_H

#include "runtime/rtapi.h"
#include "runtime/rtapi_string.h"
#include "runtime/rtapi_math.h"

#include "hal/hal.h"

#include "hostmot2/hostmot2.h"

int hm2_capsense_parse_md(hostmot2_t *hm2, int md_index) {
    hm2_module_descriptor_t *md = &hm2->md[md_index];
    int r;


    if (hm2->capsense.num_instances != 0) {
        HM2_ERR(
            "found duplicate Module Descriptor for %s (inconsistent firmware), not loading driver\n",
            hm2_get_general_function_name(md->gtag)
        );
        return -EINVAL;
    }

    // allocate the module-global HAL shared memory
    hm2->capsense.hal = (hm2_capsense_module_global_t *)hal_malloc(sizeof(hm2_capsense_module_global_t));
    if (hm2->capsense.hal == NULL) {
        HM2_ERR("out of memory!\n");
        r = -ENOMEM;
        goto fail0;
    }

    if (hm2->config.num_capsensors > md->instances) {
        HM2_ERR(
            "config.num_capsensors=%d, but only %d are available, not loading driver\n",
            hm2->config.num_capsensors,
            md->instances
        );
        return -EINVAL;
    }

    if (hm2->config.num_capsensors == 0) {
        return 0;
    }

    //
    // looks good, start initializing
    //


    if (hm2->config.num_capsensors == -1) {
        hm2->capsense.num_instances = md->instances;
    } else {
        hm2->capsense.num_instances = hm2->config.num_capsensors;
    }


    hm2->capsense.instance = (hm2_capsense_instance_t *)hal_malloc(hm2->capsense.num_instances * sizeof(hm2_capsense_instance_t));
    if (hm2->capsense.instance == NULL) {
        HM2_ERR("out of memory!\n");
        r = -ENOMEM;
        goto fail0;
    }

    hm2->capsense.capsense_data_addr = md->base_address + (0 * md->register_stride);
    hm2->capsense.capsense_hysteresis_addr = md->base_address + (0 * md->register_stride) + (1 * sizeof(u32));

    r = hm2_register_tram_read_region(hm2, hm2->capsense.capsense_data_addr, (1 * sizeof(u32)), &hm2->capsense.capsense_data_reg);
    if (r < 0) {
        HM2_ERR("error registering tram read region for CAPSENSE Data register (%d)\n", r);
        goto fail0;
    }

    hm2->capsense.capsense_data_reg = (u32 *)kmalloc(1 * sizeof(u32), GFP_KERNEL);
    if (hm2->capsense.capsense_data_reg == NULL) {
        HM2_ERR("out of memory!\n");
        r = -ENOMEM;
        goto fail0;
    }

    r = hm2_register_tram_read_region(hm2, hm2->capsense.capsense_hysteresis_addr, (1 * sizeof(u32)), &hm2->capsense.capsense_hysteresis_reg);
    if (r < 0) {
        HM2_ERR("error registering tram write region for CAPSENSE Hysteresis register (%d)\n", r);
        goto fail0;
    }

    hm2->capsense.capsense_hysteresis_reg = (u32 *)kmalloc(1 * sizeof(u32), GFP_KERNEL);
    if (hm2->capsense.capsense_hysteresis_reg == NULL) {
        HM2_ERR("out of memory!\n");
        r = -ENOMEM;
        goto fail0;
    }

    // export to HAL
    // FIXME: r hides the r in enclosing function, and it returns the wrong thing
    {
        int i;
        int r;
        char name[HAL_NAME_LEN + 1];

        // parameters

        rtapi_snprintf(name, sizeof(name), "%s.capsense.%02d.hysteresis", hm2->llio->name, 0);
        r = hal_pin_u32_new(name, HAL_IN, &(hm2->capsense.hal->pin.capsense_hysteresis), hm2->llio->comp_id);
        if (r < 0) {
            HM2_ERR("error adding capsense.hysteresis pin, aborting\n");
            goto fail1;
        }

       for (i = 0; i < hm2->capsense.num_instances; i ++) {
            // pins
            rtapi_snprintf(name, sizeof(name), "%s.capsense.%02d.trigged", hm2->llio->name, i);
            r = hal_pin_bit_new(name, HAL_OUT, &(hm2->capsense.instance[i].hal.pin.sensepad), hm2->llio->comp_id);
            if (r < 0) {
                HM2_ERR("error adding pin '%s', aborting\n", name);
                goto fail1;
            }
        }
    }

    return hm2->capsense.num_instances;

fail1:
    kfree(hm2->capsense.capsense_data_reg);
    kfree(hm2->capsense.capsense_hysteresis_reg);

fail0:
    hm2->capsense.num_instances = 0;
    return r;
}

void hm2_capsense_read(hostmot2_t *hm2) {
    int i;
    u32 val;
    hal_bit_t bit;

    if (hm2->capsense.num_instances == 0) return;

    hm2->llio->read(
        hm2->llio,
        hm2->capsense.capsense_data_addr,
        &val,
        sizeof(u32)
    );

    for(i=0;i<hm2->capsense.num_instances; i ++){
        bit = (val >> i) & 0x1;
        *hm2->capsense.instance[i].hal.pin.sensepad = bit;
    }
}

void hm2_capsense_write(hostmot2_t *hm2) {

    if (hm2->capsense.num_instances == 0) return;
    *hm2->capsense.capsense_hysteresis_reg = *hm2->capsense.hal->pin.capsense_hysteresis;

    // update register
    if (*hm2->capsense.capsense_hysteresis_reg && hm2->capsense.written_capsense_hysteresis_reg != *hm2->capsense.capsense_hysteresis_reg) {
        hm2->llio->write(
            hm2->llio,
            hm2->capsense.capsense_hysteresis_addr,
            hm2->capsense.capsense_hysteresis_reg,
            sizeof(u32)
        );
        hm2->capsense.written_capsense_hysteresis_reg = *hm2->capsense.capsense_hysteresis_reg;
    } else {
        hm2->llio->read(
            hm2->llio,
            hm2->capsense.capsense_hysteresis_addr,
            (void *)hm2->capsense.hal->pin.capsense_hysteresis,
            sizeof(u32)
        );
    }
}
