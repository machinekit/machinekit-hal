//----------------------------------------------------------------------//
// Description: pru_pwmread.p                                           //
// PRU code implementing reading PWM frequency and duty cycle           //
//                                                                      //
// Author(s): John Allwine                                              //
// License: GNU GPL Version 2.0 or (at your option) any later version.  //
//                                                                      //
// Major Changes:                                                       //
// 2020-May    John Allwine                                             //
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
// This code was written as part of the MachineKit project.  For more   //
// information, go to www.machinekit.io.                                //
//----------------------------------------------------------------------//

MODE_PWM_READ:
.enter PWM_READ_SCOPE

.assign pwm_read_state, GState.State_Reg0, *, State

    LBBO State, GTask.addr, SIZE(task_header), SIZE(State)

    // Increment the current time
    // The current time represents the time the current hi or low values has been in its current state
    ADD State.CurTime, State.CurTime, State.TimeIncr

    // Read the pin
    AND r0, r31, State.HiValue

    // If there's no change than continue
    QBEQ CHECK_MAX_TIME, r0, State.CurPinState

    // Otherwise, check if we're moving from hi to low or low to hi
    QBEQ LO_TO_HI, r0, State.HiValue

// Hi to lo
    MOV State.HiTime, State.CurTime
    LDI State.CurTime, 0
    LDI State.CurPinState, 0
    JMP CHECK_MAX_TIME

LO_TO_HI:
    MOV State.LoTime, State.CurTime
    LDI State.CurTime, 0
    MOV State.CurPinState, State.HiValue

CHECK_MAX_TIME:
    QBLT PWM_READ_DONE, State.MaxTime, State.CurTime

    // The pin has been in its current state for the max amount of time,
    // so we can zero the other state
    QBEQ ZERO_HI, State.CurPinState, 0
// zero lo
    LDI State.LoTime, 0
    MOV State.HiTime, State.MaxTime
    MOV State.CurTime, State.MaxTime
    JMP PWM_READ_DONE

ZERO_HI:
    MOV State.LoTime, State.MaxTime
    LDI State.HiTime, 0
    MOV State.CurTime, State.MaxTime

PWM_READ_DONE:
    
    SBBO State, GTask.addr, SIZE(task_header), SIZE(State)
    JMP     NEXT_TASK
.leave PWM_READ_SCOPE
