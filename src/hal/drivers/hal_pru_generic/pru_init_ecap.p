//----------------------------------------------------------------------//
// Description: pru_init_ecap.p                                         //
// PRU code that initializes the ECAP timer for use by pru_wait_ecap.p  //
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

MODE_INIT_ECAP:
.enter INIT_ECAP_SCOPE
    // Setup ECAP Timer
    // Used for odd PRU numbers (see pru_wait_ecap.p)
    LDI r0, 0
    SBCO r0, CONST_ECAP, 0x28, 2 // clear ECCTL1 register
    SBCO r0, CONST_ECAP, 0x2A, 2 // clear ECCTL2 register
    SBCO r0, CONST_ECAP, 0x00, 4 // clear counter
    SBCO r0, CONST_ECAP, 0x04, 4 // clear counter phase

    SBCO r2, CONST_ECAP, 0x08, 4 // set ecap period
    SBCO r2, CONST_ECAP, 0x10, 4 // set ecap period (shadow)
    LDI  r0, 0x40
    SBCO r0, CONST_ECAP, 0x2C, 2 // set trigger when count = period mode
    LDI  r0, ((1 << 9) | (1 << 4)) 
    SBCO r0, CONST_ECAP, 0x2A, 2 // turn on APWM mode (reset count after period), free counting mode
    JMP     NEXT_TASK

.leave INIT_ECAP_SCOPE
