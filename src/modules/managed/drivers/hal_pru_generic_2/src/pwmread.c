//----------------------------------------------------------------------//
// Description: pwmread.c                                               //
// Code to interface to a PRU software pwm signal reader                //
//                                                                      //
// Author(s): John Allwine                                              //
// License: GNU GPL Version 2.0 or (at your option) any later version.  //
//                                                                      //
// Major Changes:                                                       //
// 2020-May    John Allwine                                             //
//             Initial version                                          //
//----------------------------------------------------------------------//
// This file is part of MachineKit HAL                                  //
//                                                                      //
// Copyright (C) 2020  Pocket NC Company                                //
//                                                                      //
// This program is free software; you can redistribute it and/or        //
// modify it under the terms of the GNU General Public License          //
// as published by the Free Software Foundation; either version 2       //
// of the License, or (at your option) any later version.               //
//                                                                      //
// This program is distributed in the hope that it will be useful,      //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with this program; if not, write to the Free Software          //
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA        //
// 02110-1301, USA.                                                     //
//                                                                      //
// THE AUTHORS OF THIS PROGRAM ACCEPT ABSOLUTELY NO LIABILITY FOR       //
// ANY HARM OR LOSS RESULTING FROM ITS USE.  IT IS _EXTREMELY_ UNWISE   //
// TO RELY ON SOFTWARE ALONE FOR SAFETY.  Any machinery capable of      //
// harming persons must have provisions for completely removing power   //
// from all motors, etc, before persons enter any danger area.  All     //
// machinery must be designed to comply with local and national safety  //
// codes, and the authors of this software can not, and do not, take    //
// any responsibility for such compliance.                              //
//                                                                      //
// This code was written as part of the LinuxCNC project.  For more     //
// information, go to www.machinekit.io.                                //
//----------------------------------------------------------------------//

// Use config_module.h instead of config.h so we can use RTAPI_INC_LIST_H
#include "config_module.h"
#include "config.h"             // TARGET_PLATFORM_BEAGLEBONE

#if !defined(TARGET_PLATFORM_BEAGLEBONE)
#error "This driver is for the beaglebone platform only"
#endif

#include "rtapi.h"
#include "rtapi_app.h"
#include "rtapi_string.h"
#include "rtapi_math.h"

#include "hal.h"

#include "hal/drivers/hal_pru_generic/hal_pru_generic.h"

void hpg_pwmread_read(hal_pru_generic_t *hpg) {
    int i;

    for (i = 0; i < hpg->pwmread.num_instances; i ++) {
      PRU_task_pwmread_t *pru = (PRU_task_pwmread_t*)((u32)hpg->pru_data+(u32)hpg->pwmread.instance[i].task.addr);
      const u32 hiTime = pru->hiTime;
      const u32 loTime = pru->loTime;
      const u32 maxTime = pru->maxTime;

      u32 period = hiTime+loTime;

      if(period > maxTime) {
        period = maxTime;
      }

      const float frequency = 1.0f/((float)period/1e9);
      const float duty_cycle = (float)(hiTime)/period;

      *(hpg->pwmread.instance[i].hal.duty_cycle) = duty_cycle;
      *(hpg->pwmread.instance[i].hal.frequency) = frequency;
      *(hpg->pwmread.instance[i].hal.period) = period;
    }
}

int export_pwmread(hal_pru_generic_t *hpg, int i)
{
    int r;

    r = hal_pin_float_newf(HAL_OUT, &(hpg->pwmread.instance[i].hal.frequency), hpg->config.inst_id, "%s.pwmread.%02d.frequency", hpg->config.halname, i);
    if(r < 0) {
      HPG_ERR("pwmread %02d: Error adding pin 'frequency', aborting\n", i);
      return r;
    }

    r = hal_pin_float_newf(HAL_OUT, &(hpg->pwmread.instance[i].hal.duty_cycle), hpg->config.inst_id, "%s.pwmread.%02d.duty_cycle", hpg->config.halname, i);
    if(r < 0) {
      HPG_ERR("pwmread %02d: Error adding pin 'duty_cycle', aborting\n", i);
      return r;
    }

    r = hal_pin_u32_newf(HAL_OUT, &(hpg->pwmread.instance[i].hal.period), hpg->config.inst_id, "%s.pwmread.%02d.period", hpg->config.halname, i);
    if(r < 0) {
      HPG_ERR("pwmread %02d: Error adding pin 'period', aborting\n", i);
      return r;
    }

    r = hal_pin_u32_newf(HAL_IN, &(hpg->pwmread.instance[i].hal.max_time), hpg->config.inst_id, "%s.pwmread.%02d.max_time", hpg->config.halname, i);
    if(r < 0) {
      HPG_ERR("pwmread %02d: Error adding pin 'max_time', aborting\n", i);
      return r;
    }

    r = hal_pin_u32_newf(HAL_IN, &(hpg->pwmread.instance[i].hal.pin), hpg->config.inst_id, "%s.pwmread.%02d.pin", hpg->config.halname, i);
    if(r < 0) {
      HPG_ERR("pwmread %02d: Error adding pin 'pin', aborting\n", i);
      return r;
    }

    *(hpg->pwmread.instance[i].hal.frequency) = 0;
    *(hpg->pwmread.instance[i].hal.period) = 0;
    *(hpg->pwmread.instance[i].hal.duty_cycle) = 0;
    *(hpg->pwmread.instance[i].hal.max_time) = 1000000000;
    *(hpg->pwmread.instance[i].hal.pin) = 0;

    hpg->pwmread.instance[i].written_max_time = 1000000000;
    hpg->pwmread.instance[i].written_pin = 0;

    return 0;
}

int hpg_pwmread_init(hal_pru_generic_t *hpg){
    int r,i;

    if (hpg->config.num_pwmreads <= 0)
        return 0;

    hpg->pwmread.num_instances = hpg->config.num_pwmreads;
    hpg->pwmread.instance = (hpg_pwmread_instance_t *)hal_malloc(sizeof(hpg_pwmread_instance_t)*hpg->pwmread.num_instances);
    if(hpg->pwmread.instance == 0) {
      rtapi_print_msg(RTAPI_MSG_ERR, "%s: ERROR: hal_malloc() failed\n", hpg->config.halname);
      return -1;
    }
    memset(hpg->pwmread.instance, 0, (sizeof(hpg_pwmread_instance_t)*hpg->pwmread.num_instances));
    for(i = 0; i < hpg->pwmread.num_instances; i++) {
      hpg->pwmread.instance[i].task.addr = pru_malloc(hpg, sizeof(hpg->pwmread.instance[i].pru));
      hpg->pwmread.instance[i].pru.task.hdr.mode = eMODE_PWM_READ;
      pru_loop_task_add(hpg, &(hpg->pwmread.instance[i].task));
      if((r = export_pwmread(hpg,i)) != 0) {
        rtapi_print_msg(RTAPI_MSG_ERR, "%s: ERROR: failed to export pwmread %i: %i\n", hpg->config.halname, i, r);
        return -1;
      }
    }

    return 0;
}

void hpg_pwmread_update(hal_pru_generic_t *hpg) {
    int i;

    if (hpg->pwmread.num_instances <= 0) return;

    for (i = 0; i < hpg->pwmread.num_instances; i ++) {
      if((*(hpg->pwmread.instance[i].hal.pin) != hpg->pwmread.instance[i].written_pin) ||
         (*(hpg->pwmread.instance[i].hal.max_time) != hpg->pwmread.instance[i].written_max_time)) {
        hpg->pwmread.instance[i].pru.hiValue = 1 << (fixup_pin(*(hpg->pwmread.instance[i].hal.pin)) & 0x1f);
        hpg->pwmread.instance[i].pru.maxTime = *(hpg->pwmread.instance[i].hal.max_time);
        hpg->pwmread.instance[i].written_pin = *(hpg->pwmread.instance[i].hal.pin);
        hpg->pwmread.instance[i].written_max_time = *(hpg->pwmread.instance[i].hal.max_time);

        PRU_task_pwmread_t *pru = (PRU_task_pwmread_t*)((u32)hpg->pru_data+(u32)hpg->pwmread.instance[i].task.addr);
        pru->hiValue = hpg->pwmread.instance[i].pru.hiValue;
        pru->maxTime = hpg->pwmread.instance[i].pru.maxTime;
      }
    }
}

//
// *_force_write sets up any persistent state data required that does not get
// written by the standard *_update() procedure, above
//
void hpg_pwmread_force_write(hal_pru_generic_t *hpg) {
    int i;

    if (hpg->pwmread.num_instances <= 0) return;

    for (i = 0; i < hpg->pwmread.num_instances; i ++) {
        PRU_task_pwmread_t *pru = (PRU_task_pwmread_t *) ((u32) hpg->pru_data + (u32) hpg->pwmread.instance[i].task.addr);

        // Global data common to all channels
        hpg->pwmread.instance[i].pru.task.hdr.mode  = eMODE_PWM_READ;
        hpg->pwmread.instance[i].pru.task.hdr.len   = 0;
        hpg->pwmread.instance[i].pru.task.hdr.dataX = 0x00;
        hpg->pwmread.instance[i].pru.task.hdr.dataY = 0x00;
        hpg->pwmread.instance[i].pru.task.hdr.addr  = hpg->pwmread.instance[i].task.next;

        hpg->pwmread.instance[i].pru.hiTime = 0;
        hpg->pwmread.instance[i].pru.loTime = 0;
        hpg->pwmread.instance[i].pru.curTime = 0;
        hpg->pwmread.instance[i].pru.curPinState = 0;
        hpg->pwmread.instance[i].pru.maxTime = *(hpg->pwmread.instance[i].hal.max_time);
        hpg->pwmread.instance[i].pru.timeIncr = hpg->config.pru_period;
        hpg->pwmread.instance[i].pru.hiValue = 1 << (fixup_pin(*(hpg->pwmread.instance[i].hal.pin)) & 0x1f);

        *pru = hpg->pwmread.instance[i].pru;

        hpg->pwmread.instance[i].written_max_time = hpg->pwmread.instance[i].pru.maxTime;
        hpg->pwmread.instance[i].written_pin = *(hpg->pwmread.instance[i].hal.pin);
    }

    // Call the regular update routine to finish up
    hpg_pwmread_update(hpg);
}
