//----------------------------------------------------------------------//
// Description: hal_pru_generic.c                                       //
// HAL module to communicate with PRU code implementing step/dir        //
// generation and other functions of hopeful use to off-load timing     //
// critical code from Machinekit HAL                                    //
//                                                                      //
// Author(s): Charles Steinkuehler, John Allwine                        //
// License: GNU GPL Version 2.0 or (at your option) any later version.  //
//                                                                      //
// Major Changes:                                                       //
// 2020-May    John Allwine                                             //
//             Added support for Beaglebone AI                          //
//             Made hal_pru_generic an instantiable HAL component       //
//             Added PWM read task                                      //
// 2013-May    Charles Steinkuehler                                     //
//             Split into several files                                 //
//             Added support for PRU task list                          //
//             Refactored code to more closely match mesa-hostmot2      //
// 2012-Dec-30 Charles Steinkuehler                                     //
//             Initial version, based in part on:                       //
//               hal_pru.c      Micheal Haberler                        //
//               supply.c       Matt Shaver                             //
//               stepgen.c      John Kasunich                           //
//               hostmot2 code  Sebastian Kuzminsky                     //
//----------------------------------------------------------------------//
// This file is part of MachineKit HAL                                  //
//                                                                      //
// Copyright (C) 2012  Charles Steinkuehler                             //
//                     <charles AT steinkuehler DOT net>                //
// Copyright (C) 2020 Pocket NC Company                                 //
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
// This code is part of the Machinekit HAL project.  For more           //
// information, go to https://github.com/machinekit.                    //
//----------------------------------------------------------------------//

// Use config_module.h instead of config.h so we can use RTAPI_INC_LIST_H
#define MAX_PATH_LEN 100
#include "hal/config_module.h"

#include RTAPI_INC_LIST_H
#include "rtapi.h"          /* RTAPI realtime OS API */
#include "rtapi_app.h"      /* RTAPI realtime module decls */
#include "rtapi_compat.h"   /* RTAPI support functions */
#include "rtapi_math.h"
#include "hal/hal.h"            /* HAL public API decls */
#include "config.h"         // TARGET_PLATFORM_BEAGLEBONE
#include "hal/hal_priv.h"
#include <pthread.h>

#include "prussdrv.h"           // UIO interface to uio_pruss
//#include "pru.h"                // PRU-related defines
#include "pruss_intc_mapping.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "hal/drivers/hal_pru_generic/hal_pru_generic.h"
#include "hal/drivers/hal_pru_generic/beaglebone_pinmap.h"

// this probably should be an ARM335x #define
#if !defined(TARGET_PLATFORM_BEAGLEBONE)
#error "This driver is for the beaglebone platform only"
#endif

MODULE_AUTHOR("Charles Steinkuehler");
MODULE_DESCRIPTION("AM335x PRU demo component");
MODULE_LICENSE("GPL");

/***********************************************************************
*                    MODULE PARAMETERS AND DEFINES                     *
************************************************************************/

// Maximum number of PRU "channels"
#define MAX_CHAN 8

// Default PRU code to load (prefixed by EMC_RTLIB_DIR)
// Fixme: This should probably be compiled in, via a header file generated
//        by pasm -PRUv2 -c myprucode.p
#define  DEFAULT_CODE  "stepgen.bin"

// The kernel module required to talk to the PRU
#define UIO_PRUSS  "uio_pruss"

// Default pin to use for PRU modules...use a pin that does not leave the PRU
#define PRU_DEFAULT_PIN 17

// Start out with default pulse length/width and setup/hold delays of 1 mS (1000000 nS)
#define DEFAULT_DELAY 1000000

#define f_period_s ((double)(l_period_ns * 1e-9))

static int num_stepgens = 0;
RTAPI_IP_INT(num_stepgens, "Number of step generators (default: 0)");

static int num_pwmgens = 0;
RTAPI_IP_INT(num_pwmgens, "Number of PWM outputs (default: 0)");
//int num_pwmgens[MAX_CHAN] = { -1, -1, -1, -1, -1, -1, -1, -1 };
//RTAPI_MP_ARRAY_INT(num_pwmgens, "Number of PWM outputs for up to 8 banks (default: 0)");

static int num_encoders = 0;
RTAPI_MP_INT(num_encoders, "Number of encoder channels (default: 0)");

static int num_pwmreads = 0;
RTAPI_IP_INT(num_pwmreads, "Number of PWM read channels (default: 0)");

static char *halname = "hal_pru_generic";
RTAPI_MP_STRING(halname, "Prefix for hal names (default: hal_pru_generic)");

static char *prucode = "";
RTAPI_IP_STRING(prucode, "filename of PRU code (.bin, default: stepgen.bin)");

static int pru = 1;
RTAPI_IP_INT(pru, "PRU number to execute this code (0 or 1, default: 1)");

static int pru_period = 10000;
RTAPI_IP_INT(pru_period, "PRU task period (in nS, default: 10,000 nS or 100 KHz)");

static int disabled = 0;
RTAPI_IP_INT(disabled, "start the PRU in disabled state for debugging (0=enabled, 1=disabled, default: enabled");

static int event = -1;
RTAPI_IP_INT(event, "PRU event number to listen for (0..7, default: none)");

static hpg_board_t board_id = BBB;

/***********************************************************************
*                   STRUCTURES AND GLOBAL VARIABLES                    *
************************************************************************/
static int activePRUs[4] = { 0, 0, 0, 0 };

static tprussdrv *pruss;                // driver descriptor

/* other globals */
static int comp_id;     /* component ID */

static const char *modname = "hal_pru_generic";

// if filename doesnt exist, prefix this path:
char *fw_path = "/lib/firmware/pru/";

// shared with PRU
static unsigned long *pru_data_ram;     // points to PRU data RAM
static tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;


/***********************************************************************
*                  LOCAL FUNCTION DECLARATIONS                         *
************************************************************************/
int check_board();
static int hpg_read(void *hpg, const hal_funct_args_t *fa);
static int hpg_write(void *hpg, const hal_funct_args_t *fa);
int export_pru(hal_pru_generic_t *hpg);
int pru_init();
int pru_init_hpg(int pru, char *filename, int disabled, hal_pru_generic_t *hpg);
int setup_pru(int pru, char *filename, int disabled, hal_pru_generic_t *hpg);
void pru_shutdown(int pru);
static void *pruevent_thread(void *arg);

int remoteproc_stop(int pru);
int remoteproc_start(int pru);
int remoteproc_copy_firmware(int pru, char* firmware);
int pru2remoteproc(int pru);

int hpg_wait_init(hal_pru_generic_t *hpg);
void hpg_wait_force_write(hal_pru_generic_t *hpg);
void hpg_wait_update(hal_pru_generic_t *hpg);

/***********************************************************************
*                       INIT AND EXIT CODE                             *
************************************************************************/

static int instantiate_hal_pru_generic(const int argc, char* const *argv) {
    hal_pru_generic_t *hpg;
    int inst_id;
    int retval;

    for(int i = 0; i < argc; i++) {
      HPG_DBG("ARG %d: %s\n", i, argv[i]);
    }
    HPG_DBG("static var num_stepgens %d", num_stepgens);
    HPG_DBG("static var num_pwmgens %d", num_pwmgens);
    HPG_DBG("static var num_encoders %d", num_encoders);
    HPG_DBG("static var prucode %s", prucode);
    HPG_DBG("static var pru %d", pru);
    HPG_DBG("static var pru_period %d", pru_period);
    HPG_DBG("static var disabled %d", disabled);
    HPG_DBG("static var event %d", event);
    HPG_DBG("static var board_id %d", board_id);

    if((inst_id = hal_inst_create(argv[1], comp_id, sizeof(hal_pru_generic_t), (void**)&hpg)) < 0) {
      return -1;
    }

    // Clear memory
    memset(hpg, 0, sizeof(hal_pru_generic_t));

    // Setup global state
    hpg->config.num_pwmgens  = num_pwmgens;
    hpg->config.num_stepgens = num_stepgens;
    hpg->config.num_encoders = num_encoders;
    hpg->config.num_pwmreads = num_pwmreads;
    hpg->config.comp_id      = comp_id;
    hpg->config.inst_id      = inst_id;
    hpg->config.pru_period   = pru_period;
    hpg->config.pruNumber = pru;
    strncpy(hpg->config.halname, argv[1], 10);

    // Initialize PRU and map PRU data memory

    if ((retval = pru_init_hpg(pru, prucode, disabled, hpg))) {
        HPG_ERR("ERROR: failed to initialize PRU hpg\n");
        return -1;
    }

    rtapi_print_msg(RTAPI_MSG_DBG, "num_pwmgens : %d\n",num_pwmgens);
    rtapi_print_msg(RTAPI_MSG_DBG, "num_stepgens: %d\n",num_stepgens);
    rtapi_print_msg(RTAPI_MSG_DBG, "num_encoders: %d\n",num_encoders);
    rtapi_print_msg(RTAPI_MSG_DBG, "num_pwmreads: %d\n",num_pwmreads);

    if ((retval = setup_pru(pru, prucode, disabled, hpg))) {
        HPG_ERR("ERROR: failed to initialize PRU\n");
        return -1;
    }

    rtapi_print_msg(RTAPI_MSG_DBG, "Init pwm\n");
    // Initialize various functions and generate PRU data ram contents
    if ((retval = hpg_pwmgen_init(hpg))) {
        HPG_ERR("ERROR: pwmgen init failed: %d\n", retval);
        return -1;
    }

    rtapi_print_msg(RTAPI_MSG_DBG, "Init stepgen\n");
    if ((retval = hpg_stepgen_init(hpg))) {
        HPG_ERR("ERROR: stepgen init failed: %d\n", retval);
        return -1;
    }

    rtapi_print_msg(RTAPI_MSG_DBG, "Init encoder\n");
    if ((retval = hpg_encoder_init(hpg))) {
        HPG_ERR("ERROR: encoder init failed: %d\n", retval);
        return -1;
    }

    rtapi_print_msg(RTAPI_MSG_DBG, "Init pwmread\n");
    if ((retval = hpg_pwmread_init(hpg))) {
        HPG_ERR("ERROR: pwmread init failed: %d\n", retval);
        hal_exit(comp_id);
        return -1;
    }

    if ((retval = hpg_wait_init(hpg))) {
        HPG_ERR("ERROR: global task init failed: %d\n", retval);
        return -1;
    }

    if ((retval = export_pru(hpg))) {
        HPG_ERR("ERROR: var export failed: %d\n", retval);
        return -1;
    }

    hpg_stepgen_force_write(hpg);
    hpg_pwmgen_force_write(hpg);
    hpg_encoder_force_write(hpg);
    hpg_pwmread_force_write(hpg);
    hpg_wait_force_write(hpg);

//    rtapi_print_msg(RTAPI_MSG_DBG, "about to run setup_pru %d %s %d\n", pru, prucode, disabled);

//    if ((retval = setup_pru(pru, prucode, disabled, hpg))) {
//        HPG_ERR("ERROR: failed to initialize PRU\n");
//        return -1;
//    }

//    HPG_DBG("PRU Data After setup_pru\n");
//    for (int i = 0; i < 25; i++) {
//      HPG_DBG("0x%04x: 0x%08x\n", 4*i, hpg->pru_data[i]);
//    }
//    HPG_INFO("installed\n");
//    hal_ready(comp_id);
    return 0;
}

int rtapi_app_main(void) {
  comp_id = hal_xinit(TYPE_RT, 0, 0, instantiate_hal_pru_generic, NULL, modname);
  if(comp_id < 0)
    return comp_id;

  board_id = check_board();

  // Initialize PRU and map PRU data memory
  if (pru_init()) {
      HPG_ERR("ERROR: failed to initialize PRU\n");
      return -1;
  }


  hal_ready(comp_id);
  return 0;
}

void rtapi_app_exit(void) {
    for(int i = 0; i < sizeof(activePRUs)/sizeof(int); i++) {
      if(activePRUs[i]) {
        pru_shutdown(i);
      }
    }
    hal_exit(comp_id);
}

/***********************************************************************
*                       REALTIME FUNCTIONS                             *
************************************************************************/
static int hpg_read(void *void_hpg, const hal_funct_args_t *fa) {
    hal_pru_generic_t *hpg = void_hpg;

    hpg_stepgen_read(hpg, fa_period(fa));
    hpg_encoder_read(hpg, fa_period(fa));
    hpg_pwmread_read(hpg);

    return 0;
}

u16 ns2periods(hal_pru_generic_t *hpg, hal_u32_t ns) {
    u16 p = rtapi_ceil((double)ns / (double)hpg->config.pru_period);
    return p;
}

static int hpg_write(void *void_hpg, const hal_funct_args_t *fa) {
    hal_pru_generic_t *hpg = void_hpg;

    hpg_stepgen_update(hpg, fa_period(fa));
    hpg_pwmgen_update(hpg);
    hpg_encoder_update(hpg);
    hpg_pwmread_update(hpg);
    hpg_wait_update(hpg);

  return 0;
}

/***********************************************************************
*                   LOCAL FUNCTION DEFINITIONS                         *
************************************************************************/

// Allocate 32-bit words from PRU data memory
// Start at the beginning of memory, and contiguously allocate RAM
// until we run out of requests.  No free, no garbage colletion, etc.
// Feel free to enhance this when you start adding and removing PRU
// tasks at run-time!  :)
pru_addr_t pru_malloc(hal_pru_generic_t *hpg, int len) {
    // Return address is first free memory location
    pru_addr_t a = hpg->pru_data_free;

    // Insure length is a natural 32-bit length
    int len32 = (len & ~0x03);
    if ((len % 4) != 0) len32 += 4;

    // Point to the next free location
    hpg->pru_data_free += len32;

    HPG_DBG("pru_malloc requested %d bytes, allocated %d bytes starting at %04x\n", len, len32, a);

    // ...and we're done
    return a;
}

int export_pru(hal_pru_generic_t *hpg)
{
    int r;

    // Export functions
    hal_export_xfunct_args_t updateArgs = {
      .type = FS_XTHREADFUNC,
      .funct.x = hpg_write,
      .arg = hpg,
      .uses_fp = 1,
      .reentrant = 0,
      .owner_id = hpg->config.inst_id
    };
    r = hal_export_xfunctf(&updateArgs, "%s.update", hpg->config.halname);
    if (r != 0) {
        HPG_ERR("ERROR: function export failed: %s\n", hpg->config.halname);
        return -1;
    }

    hal_export_xfunct_args_t captureArgs = {
      .type = FS_XTHREADFUNC,
      .funct.x = hpg_read,
      .arg = hpg,
      .uses_fp = 1,
      .reentrant = 0,
      .owner_id = hpg->config.inst_id
    };
    r = hal_export_xfunctf(&captureArgs, "%s.capture-position", hpg->config.halname);
    if (r != 0) {
        HPG_ERR("ERROR: function export failed: %s\n", hpg->config.halname);
        return -1;
    }

    return 0;
}


int assure_module_loaded(const char *module)
{
    FILE *fd;
    char line[100];
    int len = strlen(module);
    int retval;

    fd = fopen("/proc/modules", "r");
    if (fd == NULL) {
        HPG_ERR("ERROR: cannot read /proc/modules\n");
        return -1;
    }
    while (fgets(line, sizeof(line), fd)) {
        if (!strncmp(line, module, len)) {
            HPG_DBG("module '%s' already loaded\n", module);
            fclose(fd);
            return 0;
        }
    }
    fclose(fd);
    HPG_DBG("loading module '%s'\n", module);
    rtapi_snprintf(line, sizeof(line), "/sbin/modprobe %s", module);
    if ((retval = system(line))) {
        HPG_ERR("ERROR: executing '%s'  %d - %s\n", line, errno, strerror(errno));
        return -1;
    }
    return 0;
}

int check_board() {
    int b = BBB;
    const char *boardFile = "/proc/device-tree/model";

    FILE *fd;
    char line[100];

    fd = fopen(boardFile, "r");
    if (fd == NULL) {
        HPG_ERR("ERROR: cannot read /proc/device-tree/model\n");
        return -1;
    }
    while (fgets(line, sizeof(line), fd)) {
      if (strcmp(line, "BeagleBoard.org BeagleBone AI") == 0) {
        b = BBAI;
        break;
      }
    }
    fclose(fd);

    return b;
}

int pru_init() {
    int retval;

    if (geteuid()) {
        HPG_ERR("ERROR: not running as root - need to 'sudo make setuid'?\n");
        return -1;
    }
    if ((retval = assure_module_loaded(UIO_PRUSS)))
    return retval;

rtapi_print_msg(RTAPI_MSG_DBG, "prussdrv_init\n");
    // Allocate and initialize memory
    prussdrv_init ();

    // Beaglebone AI doesn't provide uio_pruss module that can
    // load/start/end code on the pru, so we need to use remoteproc
    // for that
    int use_remoteproc = board_id == BBAI;

    // The uio_pruss module on the AI also doesn't have access to the control and version
    // regions of the PRU, so it can't detect the version automatically. Instead,
    // we explicitly tell prussdrv which version to use.
    int force_version = use_remoteproc ? PRUSS_AI : 0;

    // opens an event out and initializes memory mapping
rtapi_print_msg(RTAPI_MSG_DBG, "prussdrv_open %d %d\n", use_remoteproc, force_version);
    if (prussdrv_open((event > -1 ? event : PRU_EVTOUT_0) < 0, use_remoteproc, force_version)) {
      return -1;
    }

    // expose the driver data, filled in by prussdrv_open
    pruss = &prussdrv;

    if(!use_remoteproc) {
      // Map PRU's INTC
  rtapi_print_msg(RTAPI_MSG_DBG, "prussdrv_pruintc_init\n");
      if (prussdrv_pruintc_init(&pruss_intc_initdata) < 0)
      return -1;
    }

  return retval;
}

int pru_init_hpg(int pru, char *filename, int disabled, hal_pru_generic_t *hpg) {
  int i;

    // Maps the PRU DRAM memory to input pointer
rtapi_print_msg(RTAPI_MSG_DBG, "prussdrv_map_prumem, %d\n", pru);
    int prumem;
    switch(pru) {
      case 0:
        prumem = PRUSS0_PRU0_DATARAM;
        break;
      case 1:
        prumem = PRUSS0_PRU1_DATARAM;
        break;
      case 2:
        prumem = PRUSS1_PRU0_DATARAM;
        break;
      case 3:
        prumem = PRUSS1_PRU1_DATARAM;
        break;
      default:
        return -1;
    }
    if (prussdrv_map_prumem(prumem, (void **) &pru_data_ram) < 0)
      return -1;

rtapi_print_msg(RTAPI_MSG_DBG, "PRU data ram mapped\n");
    rtapi_print_msg(RTAPI_MSG_DBG, "%s: PRU data ram mapped at %p\n",
            modname, pru_data_ram);

    hpg->pru_data = (u32 *) pru_data_ram;

    // Zero PRU data memory
    for (i = 0; i < 8192/4; i++) {
        hpg->pru_data[i] = 0;
    }

    // Reserve PRU memory for static configuration variables
    hpg->pru_stat_addr = PRU_DATA_START;
    hpg->pru_data_free = hpg->pru_stat_addr + sizeof(PRU_statics_t);

    // Setup PRU globals
    hpg->pru_stat.task.hdr.dataX = 0xAB;
    hpg->pru_stat.task.hdr.dataY = 0xFE;
    hpg->pru_stat.period = hpg->config.pru_period/5;
    hpg->pru_stat.ready = 0;

    PRU_statics_t *stat = (PRU_statics_t *) ((u32) hpg->pru_data + (u32) hpg->pru_stat_addr);
    *stat = hpg->pru_stat;

    return 0;
}

int remoteproc_copy_firmware(int pru, char* filename) {
  char firmware_path[MAX_PATH_LEN];

  int chip; // chip is 1 or 2
  int core; // core is 0 or 1

  switch(pru) {
    case 0:
      chip = 1;
      core = 0;
      break;
    case 1:
      chip = 1;
      core = 1;
      break;
    case 2:
      chip = 2;
      core = 0;
      break;
    case 3:
      chip = 2;
      core = 1;
      break;
    default:
      return -1;
  }

  rtapi_snprintf(firmware_path, sizeof(firmware_path), "/lib/firmware/am57xx-pru%d_%d-fw", chip, core);

  int firm_fd = open(firmware_path, O_WRONLY);
  if(firm_fd < 0) {
    return -1;
  }

  rtapi_print_msg(RTAPI_MSG_DBG, "%s: copying %s to %s\n", modname, filename, firmware_path);
  int prubin_fd = open(filename, O_RDONLY);

  if(prubin_fd < 0) {
    return -1;
  }

  int r;
#define BUFFER_SIZE 4096
  char buff[BUFFER_SIZE];
  while((r = read(prubin_fd, buff, BUFFER_SIZE)) > 0) {
    if(write(firm_fd, buff, r) != r) {
      rtapi_print_msg(RTAPI_MSG_ERR, "%s: error writing PRU firmware\n", modname);
      return -1;
    }
  }

  close(prubin_fd);
  close(firm_fd);
  return 0;
}

// look up the remoteproc number by checking /sys/class/remoteproc/remoteproc{0-7}/firmware
// for the right value. If it doesn't find the right value, simply return the pru number.
int pru2remoteproc(int pru) {
  int remoteproc = pru;

  char firmwareName[MAX_PATH_LEN];
  char remoteprocPath[MAX_PATH_LEN];
  char remoteprocFirmware[MAX_PATH_LEN];

  int chip; // chip is 1 or 2
  int core; // core is 0 or 1

  switch(pru) {
    case 0:
      chip = 1;
      core = 0;
      break;
    case 1:
      chip = 1;
      core = 1;
      break;
    case 2:
      chip = 2;
      core = 0;
      break;
    case 3:
      chip = 2;
      core = 1;
      break;
    default:
      return -1;
  }

  rtapi_snprintf(firmwareName, sizeof(firmwareName), "am57xx-pru%d_%d-fw\n", chip, core);

  int fd;
  size_t r;
  for(int i = 0; i < 8; i++) {
    rtapi_snprintf(remoteprocPath, sizeof(remoteprocPath), "/sys/class/remoteproc/remoteproc%d/firmware", i);
    fd = open(remoteprocPath, O_RDWR);
    if(fd != -1) {
      r = pread(fd, remoteprocFirmware, sizeof(remoteprocFirmware)-1, 0);
      remoteprocFirmware[r] = 0;
      if (strcmp(remoteprocFirmware, firmwareName) == 0) {
        remoteproc = i;
        break;
      }
      close(fd);
    }
  }

  return remoteproc;
}


int setup_pru(int pru, char *filename, int disabled, hal_pru_generic_t *hpg) {
    activePRUs[pru] = 1;

    int retval;

    if (event > -1) {
    prussdrv_start_irqthread (event, sched_get_priority_max(SCHED_FIFO) - 2,
                  pruevent_thread, (void *) event);
    HPG_ERR("PRU event %d listener started\n",event);
    }

    // Load and execute binary on PRU
    // search for .bin files as passed in and under fw_path
    char pru_binpath[PATH_MAX];

    // default the .bin filename if not given
    if (!strlen(filename))
    filename = DEFAULT_CODE;

    strcpy(pru_binpath, filename);

    struct stat statb;

    if (!((stat(pru_binpath, &statb) == 0) &&
     S_ISREG(statb.st_mode))) {

    // filename not found, prefix fw_path & try that:
    strcpy(pru_binpath, fw_path);
    strcat(pru_binpath, filename);

    if (!((stat(pru_binpath, &statb) == 0) &&
          S_ISREG(statb.st_mode))) {
        // nyet, complain
        getcwd(pru_binpath, sizeof(pru_binpath));
        rtapi_print_msg(RTAPI_MSG_ERR,
                "%s: cant find %s in %s or %s\n",
                modname, filename, pru_binpath, fw_path);
        return -ENOENT;
    }
    }

    if(board_id == BBAI) {
      // Use remoteproc to start PRU code
      remoteproc_stop(pru);

      retval = remoteproc_copy_firmware(pru, pru_binpath);
      if(retval < 0) {
        return -1;
      }

      retval = remoteproc_start(pru);
      if(retval < 0) {
        return -1;
      }
    } else {
      // Use uio_pruss module
      retval =  prussdrv_exec_program (pru, pru_binpath, disabled);
    }
    return retval;
}

void pru_init_task_add(hal_pru_generic_t *hpg, pru_task_t *task) {
  if(hpg->last_init_task == 0) {
    // First init task
    HPG_DBG("Adding first init task: addr=%04x\n", task->addr);
    if(hpg->last_loop_task != 0) {
      // If last_init_task is 0, but last_loop_task isn't, the static task addr will be the first loop task
      // so set that task to be next after the init task
      task->next = hpg->pru_stat.task.hdr.addr;
    }
    hpg->pru_stat.task.hdr.addr = task->addr;
  } else {
    HPG_DBG("Adding init task: addr=%04x, prev=%04x\n", task->addr, hpg->last_init_task->addr);
    task->next = hpg->last_init_task->next;
    hpg->last_init_task->next = task->addr;
  }
}

void pru_loop_task_add(hal_pru_generic_t *hpg, pru_task_t *task) {
    if (hpg->last_loop_task == 0) {
      // This is the first loop task
      HPG_DBG("Adding first loop task: addr=%04x\n", task->addr);
      if(hpg->last_init_task == 0) {
        // No init task, so first task to run needs to be this task
        HPG_DBG("No init task, setting pru_stat.task.hdr.addr to this task: addr=%04x\n", task->addr);
        hpg->pru_stat.task.hdr.addr = task->addr;
      } else {
        // There is an init task, so call this looping task after last init task
        hpg->last_init_task->next = task->addr;
      }

      // first and only looping task right now, so it will be called repeatedly
      task->next = task->addr;
      hpg->last_loop_task  = task;
    } else {
      // Add this task to the end of the task list
      HPG_DBG("Adding task: addr=%04x prev=%04x\n", task->addr, hpg->last_loop_task->addr);
      if(hpg->last_init_task == 0) {
        // No init tasks, so link last looping task back to static start task
        task->next = hpg->pru_stat.task.hdr.addr;
      } else {
        // There is at least one init task, so loop back to the task after the last init task
        task->next = hpg->last_init_task->next;
      }
      hpg->last_loop_task->next = task->addr;
      hpg->last_loop_task = task;
    }
}

static void *pruevent_thread(void *arg)
{
    int event = (int) arg;
    int event_count;
    do {
    if (prussdrv_pru_wait_event(event, &event_count) < 0)
        continue;
    HPG_ERR("PRU event %d received\n",event);
    prussdrv_pru_clear_event(pru ? PRU1_ARM_INTERRUPT : PRU0_ARM_INTERRUPT);
    } while (1);
    HPG_ERR("pruevent_thread exiting\n");
    return NULL; // silence compiler warning
}

int remoteproc_stop(int pru) {
  char remoteproc_path[MAX_PATH_LEN];
  rtapi_snprintf(remoteproc_path, sizeof(remoteproc_path), "/sys/class/remoteproc/remoteproc%d/state", pru2remoteproc(pru));
  int remoteproc_fd = open(remoteproc_path, O_WRONLY);
  char stop[] = "stop";
  int written;
  if((written = write(remoteproc_fd, stop, sizeof(stop))) != sizeof(stop)) {
//    rtapi_print_msg(RTAPI_MSG_ERR, "%s: error stopping PRU with remoteproc %s, %d (maybe it's already stopped?)\n", modname, remoteproc_path, written);
    return -1;
  }
  close(remoteproc_fd);
  return 0;
}

int remoteproc_start(int pru) {
  char remoteproc_path[MAX_PATH_LEN];
  rtapi_snprintf(remoteproc_path, sizeof(remoteproc_path), "/sys/class/remoteproc/remoteproc%d/state", pru2remoteproc(pru));
  int remoteproc_fd = open(remoteproc_path, O_WRONLY);
  char start[] = "start";
  int written;
  if((written = write(remoteproc_fd, start, sizeof(start))) != sizeof(start)) {
    rtapi_print_msg(RTAPI_MSG_ERR, "%s: error starting PRU with remoteproc %s, %d\n", modname, remoteproc_path, written);
    return -1;
  }
  close(remoteproc_fd);
  return 0;
}

void pru_shutdown(int pru)
{
    // Disable PRU and close memory mappings
    if(board_id == BBAI) {
      remoteproc_stop(pru);
    } else{
      prussdrv_pru_disable(pru);
    }
    prussdrv_exit (); // also joins event listen thread
}

int hpg_wait_init(hal_pru_generic_t *hpg) {
    int r;

    hpg->wait.task.addr = pru_malloc(hpg, sizeof(hpg->wait.pru));
    hpg->wait_init.task.addr = pru_malloc(hpg, sizeof(hpg->wait_init.pru));

    pru_loop_task_add(hpg, &(hpg->wait.task));
    pru_init_task_add(hpg, &(hpg->wait_init.task));

    r = hal_pin_u32_newf(HAL_IN, &(hpg->hal.pin.pru_busy_pin), hpg->config.inst_id, "%s.pru_busy_pin", hpg->config.halname);
    if (r != 0) { return r; }

    *(hpg->hal.pin.pru_busy_pin) = 0x80;

    return 0;
}

void hpg_wait_force_write(hal_pru_generic_t *hpg) {
    if(hpg->config.pruNumber % 2) {
      hpg->wait.pru.task.hdr.mode = eMODE_WAIT_ECAP;
      hpg->wait_init.pru.task.hdr.mode = eMODE_INIT_ECAP;
    } else {
      hpg->wait.pru.task.hdr.mode = eMODE_WAIT;
      hpg->wait_init.pru.task.hdr.mode = eMODE_INIT_IEP;
    }
    hpg->wait.pru.task.hdr.dataX = *(hpg->hal.pin.pru_busy_pin);
    hpg->wait.pru.task.hdr.dataY = 0x00;
    hpg->wait.pru.task.hdr.addr = hpg->wait.task.next;
    hpg->wait_init.pru.task.hdr.addr = hpg->wait_init.task.next;

    hpg->pru_stat.ready = 1;

    PRU_task_basic_t *pru = (PRU_task_basic_t *) ((u32) hpg->pru_data + (u32) hpg->wait.task.addr);
    *pru = hpg->wait.pru;

    pru = (PRU_task_basic_t *) ((u32) hpg->pru_data + (u32) hpg->wait_init.task.addr);
    *pru = hpg->wait_init.pru;

    PRU_statics_t *stat = (PRU_statics_t *) ((u32) hpg->pru_data + (u32) hpg->pru_stat_addr);
    *stat = hpg->pru_stat;
}

void hpg_wait_update(hal_pru_generic_t *hpg) {
    if (hpg->wait.pru.task.hdr.dataX != *(hpg->hal.pin.pru_busy_pin))
        hpg->wait.pru.task.hdr.dataX = *(hpg->hal.pin.pru_busy_pin);

    PRU_task_basic_t *pru = (PRU_task_basic_t *) ((u32) hpg->pru_data + (u32) hpg->wait.task.addr);
    *pru = hpg->wait.pru;
}

int fixup_pin(u32 hal_pin) {
    int ret = 0;
    u32 type, p89, index;

    bone_pinmap const *p8_pins;
    bone_pinmap const *p9_pins;

    switch(board_id) {
      case BBAI:
	p8_pins = p8ai_pins;
	p9_pins = p9ai_pins;
	break;
      default:
	p8_pins = p8bbb_pins;
	p9_pins = p9bbb_pins;
	break;
    }

    // Original brain-dead pin numbering
    if (hal_pin < 192) {
        ret = hal_pin;
    } else {
        type  =  hal_pin / 100;
        p89   = (hal_pin % 1000) / 100 ;
        index =  hal_pin % 100;

        // Fixup index value for P9 pins with two CPU pins attached
        if (p89 == 9) {
            if ((index == 91) || (index == 92)) {
                index = index - 50 + (47 - 41);
            } else if (index > 46) {
                index = 0;
            }
        } else if (index > 46) {
            index = 0;
        }

        switch (type) {
        case 8 :
            ret = p8_pins[index].gpio_pin_num;
            break;
        case 9 :
            ret = p9_pins[index].gpio_pin_num;
            break;
        case 18 :
            ret = p8_pins[index].pruO_pin_num;
            break;
        case 19 :
            ret = p9_pins[index].pruO_pin_num;
            break;
        case 28 :
            ret = p8_pins[index].pruI_pin_num;
            break;
        case 29 :
            ret = p9_pins[index].pruI_pin_num;
            break;
        default:
            ret = 0;
        }

        if (ret == 0)
            HPG_ERR("Unknown pin: %d\n",(int)hal_pin);

        if (ret < 0) {
            HPG_ERR("Requested pin unavailable: %d\n",(int)hal_pin);
            ret = 0;
        }
    }

    return ret;
}
