//----------------------------------------------------------------------//
// Description: pru_init_iep.p                                          //
// PRU code that initializes the IEP timer for use by pru_wait.p        //
//                                                                      //
// Author(s): John Allwine                                              //
// License: GNU GPL Version 2.0 or (at your option) any later version.  //
//                                                                      //
// Major Changes:                                                       //
// 2020-Sep    John Allwine                                             //
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

MODE_INIT_IEP:
.enter INIT_IEP_SCOPE
#ifdef BBAI
    // Setup IEP Timer
    // Used for even PRU numbers (see pru_wait.p)
    // http://www.ti.com/lit/ug/spruhz6l/spruhz6l.pdf
    // Section 30.1.11.2.2.3 PRU-ICSS Industrial Ethernet Timer Basic Programming Sequence

    // 1. Initialize timer to known state (default values)

    // Disable counter
    LBCO r6, CONST_IEP, 0x0, 4
    CLR r6, 0
    SBCO r6, CONST_IEP, 0x0, 4

    // Reset Count Register
    MOV r0, 0xffffffff
    SBCO r0, CONST_IEP, 0x10, 4
    SBCO r0, CONST_IEP, 0x14, 4

    // Clear overflow status register
    LBCO r1, CONST_IEP, 0x04, 4
    SET r1, 0
    SBCO r1, CONST_IEP, 0x04, 4

    // Clear compare status
    SBCO r0, CONST_IEP, 0x74, 4

    // 2. Set compare values
    // Use Task_Addr to point to static variables during init
    // Load loop period from static variables into CMP0
    SBCO r2, CONST_IEP, 0x78, 4
    LDI r1, 0
    SBCO r1, CONST_IEP, 0x7C, 4

    // 3. Enable compare events
    LBCO r1, CONST_IEP, 0x70, 4
    OR r1, r1, 0x03
    SBCO r1, CONST_IEP, 0x70, 4

    // 4. Set increment value
    CLR r6, r6, 7
    CLR r6, r6, 6
    CLR r6, r6, 5
    SET r6, r6, 4 // set increment to 1
    SBCO r6, CONST_IEP, 0x0, 4

    // 5. Set compensation value
    LDI r1, 0
    SBCO r1, CONST_IEP, 0x08, 4
    SBCO r1, CONST_IEP, 0x0C, 4

    // 6. Enable counter
    SET r6, 0
    SBCO r6, CONST_IEP, 0x0, 4
#else
    // Setup IEP timer
    LBCO    r6, CONST_IEP, 0x40, 40                 // Read all 10 32-bit CMP registers into r6-r15
    OR      r6, r6, 0x03                            // Set count reset and enable compare 0 event
    // Load loop period from static variables into CMP0
    LBBO    r8, GState.Task_Addr, OFFSET(pru_statics.period), SIZE(pru_statics.period)

    SBCO    r6, CONST_IEP, 0x40, 40                 // Save 10 32-bit CMP registers
    MOV     r2, 0x00000111                          // Enable counter, configured to count instructions (increments by 1 each clock, to represent 5 ns)
    SBCO    r2, CONST_IEP, 0x00, 4                  // Save IEP GLOBAL_CFG register

#endif
    JMP     NEXT_TASK

.leave INIT_IEP_SCOPE
