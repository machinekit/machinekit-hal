# ~~~
# ####################################################################
# Description:  CMakeLists.txt
#
#               This file, 'CMakeLists.txt', implements build system
#               rules for Machinekit-HAL project
#
# Copyright (C) 2021    Jakub Fišer  <jakub DOT fiser AT eryaf DOT com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#
# #####################################################################
# ~~~

option(BUILD_RUNTESTS_TESTSUITE "Build the Machinekit-HAL's Runtest testsuite."
       TRUE)

if(BUILD_RUNTESTS_TESTSUITE)

  set(RUNTESTS_SOURCE_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/abs.0/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/abs.0/runstreamer
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/abs.0/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/and-or-not-mux.0/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/and-or-not-mux.0/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/and-or-not-mux.0/runstreamer
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/and-or-not-mux.0/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/basic/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/basic/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/basic/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/counter-encoder.0/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/counter-encoder.0/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/counter-encoder.0/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/counter-encoder-v2.0/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/counter-encoder-v2.0/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/counter-encoder-v2.0/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/cpusets/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/cpusets/skip
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/cpusets/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/edge/both/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/edge/both/runstreamer
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/edge/both-starting-high/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/edge/both-starting-high/runstreamer
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/edge/both-starting-high/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/edge/both/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/edge/falling/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/edge/falling/runstreamer
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/edge/falling-starting-high/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/edge/falling-starting-high/runstreamer
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/edge/falling-starting-high/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/edge/falling/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/edge/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/edge/rising/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/edge/rising/runstreamer
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/edge/rising-starting-high/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/edge/rising-starting-high/runstreamer
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/edge/rising-starting-high/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/edge/rising/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/encoder-ratio.0/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/encoder-ratio.0/skip
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/encoder-ratio.0/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/encoder-ratio-stepgen-v2.0/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/encoder-ratio-stepgen-v2.0/skip
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/encoder-ratio-stepgen-v2.0/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/flipflop.0/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/flipflop.0/runstreamer
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/flipflop.0/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/halgroups/group.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/hal-link-unlink/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/hal-link-unlink/hallink.py
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/hal-link-unlink/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/halmodule.0/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/halmodule.0/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/halmodule.0/test.py
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/halmodule.0/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/hm2-idrom/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/hm2-idrom/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/hm2-idrom/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/ilowpass.0/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/ilowpass.0/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/ilowpass.0/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/inst_comps/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/inst_comps/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/inst_comps/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/constraints/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/constraints/README.md
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/constraints/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/limit-accel-and-max/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/limit-accel-and-max/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/limit-max-velocity/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/limit-max-velocity/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/lock-on-input-and-limits/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/lock-on-input-and-limits/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/lock-on-input-and-limits/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/min-max-overshoot/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/min-max-overshoot/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/min-max-overshoot/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/out_acc_vel-pins/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/out_acc_vel-pins/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/out_acc_vel-pins/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/runaway/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/runaway/input
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/runaway/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/runaway/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/stop-fwd/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/stop-fwd/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/stop-fwd/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/stopped-pin/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/stopped-pin/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/stop-rev/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/stop-rev/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/stop-rev/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/sunny-day/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/sunny-day/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3/sunny-day/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/constraints/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/constraints/README.md
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/constraints/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/limit-accel-and-max/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/limit-accel-and-max/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/limit-max-velocity/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/limit-max-velocity/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/lock-on-input-and-limits/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/lock-on-input-and-limits/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/lock-on-input-and-limits/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/min-max-overshoot/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/min-max-overshoot/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/min-max-overshoot/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/out_acc_vel-pins/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/out_acc_vel-pins/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/out_acc_vel-pins/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/runaway/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/runaway/input
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/runaway/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/runaway/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/stop-fwd/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/stop-fwd/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/stop-fwd/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/stopped-pin/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/stopped-pin/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/stop-rev/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/stop-rev/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/stop-rev/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/sunny-day/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/sunny-day/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/limit3v2/sunny-day/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/loadrt.1/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/loadrt.1/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/loadrt.1/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/modparam.0/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/modparam.0/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/1-names/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/1-names/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/1-names/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/1-names/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/1-names/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/1-names/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/8-names/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/8-names/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/8-names/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/8-names/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/8-names/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/8-names/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/9-names/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/9-names/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/9-names/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/9-names/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/9-names/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/9-names/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/num_chan=0/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/num_chan=0/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/num_chan=0/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/num_chan=0/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/num_chan=0/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/num_chan=0/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/num_chan=1/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/num_chan=1/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/num_chan=1/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/num_chan=1/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/num_chan=1/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/num_chan=1/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/num_chan=8/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/num_chan=8/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/num_chan=8/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/num_chan=8/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/num_chan=8/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/num_chan=8/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/num_chan=9/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/num_chan=9/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/num_chan=9/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/num_chan=9/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/num_chan=9/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/num_chan=9/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/PIN_NAME_REGEX.names
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder/PIN_NAME_REGEX.num_chan
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/1-names/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/1-names/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/1-names/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/1-names/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/1-names/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/1-names/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/8-names/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/8-names/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/8-names/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/8-names/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/8-names/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/8-names/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/9-names/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/9-names/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/9-names/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/9-names/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/9-names/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/9-names/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/num_chan=0/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/num_chan=0/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/num_chan=0/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/num_chan=0/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/num_chan=0/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/num_chan=0/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/num_chan=1/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/num_chan=1/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/num_chan=1/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/num_chan=1/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/num_chan=1/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/num_chan=1/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/num_chan=8/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/num_chan=8/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/num_chan=8/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/num_chan=8/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/num_chan=8/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/num_chan=8/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/num_chan=9/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/num_chan=9/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/num_chan=9/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/num_chan=9/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/num_chan=9/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/num_chan=9/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/PIN_NAME_REGEX.names
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/encoder_ratio/PIN_NAME_REGEX.num_chan
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/16-names/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/16-names/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/16-names/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/16-names/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/16-names/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/16-names/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/17-names/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/17-names/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/17-names/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/17-names/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/17-names/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/17-names/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/1-names/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/1-names/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/1-names/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/1-names/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/1-names/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/1-names/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/8-names/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/8-names/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/8-names/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/8-names/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/8-names/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/8-names/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/num_chan=0/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/num_chan=16/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/num_chan=16/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/num_chan=16/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/num_chan=16/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/num_chan=16/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/num_chan=16/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/num_chan=17/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/num_chan=17/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/num_chan=17/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/num_chan=17/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/num_chan=17/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/num_chan=17/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/num_chan=1/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/num_chan=1/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/num_chan=1/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/num_chan=1/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/num_chan=1/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/num_chan=1/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/PIN_NAME_REGEX.names
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/pid/PIN_NAME_REGEX.num_chan
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/shared-test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/16-names/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/16-names/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/16-names/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/16-names/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/16-names/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/16-names/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/17-names/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/17-names/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/17-names/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/17-names/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/17-names/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/17-names/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/1-names/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/1-names/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/1-names/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/1-names/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/1-names/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/1-names/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/num_chan=0/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/num_chan=0/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/num_chan=0/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/num_chan=0/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/num_chan=0/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/num_chan=0/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/num_chan=16/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/num_chan=16/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/num_chan=16/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/num_chan=16/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/num_chan=16/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/num_chan=16/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/num_chan=17/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/num_chan=17/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/num_chan=17/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/num_chan=17/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/num_chan=17/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/num_chan=17/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/num_chan=1/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/num_chan=1/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/num_chan=1/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/num_chan=1/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/num_chan=1/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/num_chan=1/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/PIN_NAME_REGEX.names
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/siggen/PIN_NAME_REGEX.num_chan
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/1-names/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/1-names/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/1-names/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/1-names/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/1-names/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/1-names/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/8-names/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/8-names/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/8-names/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/8-names/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/8-names/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/8-names/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/9-names/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/9-names/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/9-names/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/9-names/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/9-names/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/9-names/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/num_chan=0/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/num_chan=0/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/num_chan=0/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/num_chan=0/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/num_chan=0/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/num_chan=0/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/num_chan=1/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/num_chan=1/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/num_chan=1/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/num_chan=1/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/num_chan=1/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/num_chan=1/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/num_chan=8/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/num_chan=8/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/num_chan=8/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/num_chan=8/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/num_chan=8/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/num_chan=8/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/num_chan=9/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/num_chan=9/NUM_PINS
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/num_chan=9/PIN_NAME_REGEX
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/num_chan=9/RESULT
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/num_chan=9/setup.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/num_chan=9/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/PIN_NAME_REGEX.names
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/module-loading/sim_encoder/PIN_NAME_REGEX.num_chan
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/multiclick/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/multiclick/input-signals
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/multiclick/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/multiclick/skip
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/multiclick/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/mux/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/mux/input-signals
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/mux/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/mux/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/near.0/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/near.0/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/near.0/runstreamer
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/near.0/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/overrun/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/overrun/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/overrun/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/pll_correction/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/pll_correction/params.py.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/pll_correction/pll_correction.comp
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/pll_correction/pll_correction.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/pll_correction/pll_time.h
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/pll_correction/README.md
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/pll_correction/skip
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/pll_correction/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/rtapi.0/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/rtapi.0/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/rtapi_printf.0/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/rtapi_printf.0/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/save.0/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/save.0/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/save.0/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/save.1/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/save.1/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/save.1/skip
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/save.1/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/source.0/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/source.0/runstreamer
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/source.0/sourced.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/source.0/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/stepgen.0/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/stepgen.0/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/stepgen.1/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/stepgen.1/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/stepgen.2/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/stepgen.2/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/stepgen.2/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/stepgen-v2.0/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/stepgen-v2.0/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/stepgen-v2.1/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/stepgen-v2.1/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/stepgen-v2.2/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/stepgen-v2.2/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/stepgen-v2.2/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/symbols.0/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/symbols.0/dotest.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/symbols.0/test_define.comp
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/symbols.0/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/symbols.0/test_use.comp
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/symbols.1/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/symbols.1/dotest.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/symbols.1/test_define1.comp
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/symbols.1/test.sh
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/symbols.1/test_use1.comp
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/threads.0/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/threads.0/README
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/threads.0/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/threads.1/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/threads.1/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/timedelay.0/expected
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/timedelay.0/test.hal
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/usercomp.0/checkresult
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/usercomp.0/rand.comp
      ${CMAKE_CURRENT_SOURCE_DIR}/runtests/usercomp.0/test.sh)

  add_custom_target(copy_runtests ALL COMMENT "Copying Runtests source files")

  foreach(source_file IN ITEMS ${RUNTESTS_SOURCE_FILES})
    cmake_path(GET source_file PARENT_PATH source_file_parent_path)
    cmake_path(
      RELATIVE_PATH source_file_parent_path BASE_DIRECTORY
      "${CMAKE_CURRENT_SOURCE_DIR}/runtests" OUTPUT_VARIABLE
      source_file_parent_path_relative_path)
    cmake_path(GET source_file FILENAME source_file_filename)

    set(source_file_runtests_relative_part
        "${source_file_parent_path_relative_path}/${source_file_filename}")
    set(output_file
        "${MACHINEKIT_HAL_TEST_OUTPUT_DIRECTORY}/runtests/${source_file_runtests_relative_part}"
    )

    add_custom_command(
      TARGET copy_runtests
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} "-E" "copy_if_different" "${source_file}"
              "${output_file}"
      BYPRODUCTS "${output_file}"
      COMMENT
        "Copying runtests source file ${source_file_runtests_relative_part}.")

    install(
      FILES "${output_file}"
      DESTINATION
        "${MACHINEKIT_HAL_TEST_DIRECTORY}/runtests/${source_file_parent_path_relative_path}"
      PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ
                  WORLD_EXECUTE
      COMPONENT MachinekitHAL_TestSuite_Runtests_Test_Files)

  endforeach()

  set(runtests_install_directory
      "${MACHINEKIT_HAL_TEST_FULL_INSTALL_DIRECTORY}/runtests")

  # The relative path between executable's and testsuite's location is the same
  # both in BINARY and INSTALL trees, so just pick one on random
  cmake_path(
    RELATIVE_PATH runtests_install_directory BASE_DIRECTORY
    "${MACHINEKIT_HAL_EXECUTABLE_FULL_INSTALL_DIRECTORY}" OUTPUT_VARIABLE
    run_runtests_runtests_relative_path)

  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/run_runtests.in
    ${CMAKE_CURRENT_BINARY_DIR}/build/run_runtests
    FILE_PERMISSIONS
    OWNER_READ
    OWNER_EXECUTE
    GROUP_READ
    GROUP_EXECUTE
    WORLD_READ
    WORLD_EXECUTE
    @ONLY
    NEWLINE_STYLE UNIX)

  add_custom_target(
    run_runtests ALL
    COMMENT "Creating the Run_runtests executable"
    COMMAND
      ${CMAKE_COMMAND} "-E" "copy_if_different"
      "${CMAKE_CURRENT_BINARY_DIR}/build/run_runtests"
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/build/run_runtests")

  install(
    PROGRAMS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/run_runtests"
    DESTINATION "${MACHINEKIT_HAL_EXECUTABLE_DIRECTORY}"
    COMPONENT MachinekitHAL_TestSuite_Runtests_Binaries)

  cpack_add_component(MachinekitHAL_TestSuite_Runtests_Binaries
                      GROUP MachinekitHAL_TestSuite_Runtests)

  cpack_add_component(
    MachinekitHAL_TestSuite_Runtests_Test_Files
    GROUP MachinekitHAL_TestSuite_Runtests
    DEPENDS MachinekitHAL_TestSuite_Runtests_Binaries
            MachinekitHAL_Managed_Module_Sampler_Components
            MachinekitHAL_Unmanaged_Module_Halstreamer_Components
            MachinekitHAL_Managed_Module_Hostmot2_Test_Drivers)

  # Specification of artifacts placement in package tree
  cpack_add_component_group(
    MachinekitHAL_TestSuite_Runtests
    PARENT_GROUP MachinekitHAL_Package_Base_Tests_Runtests)

endif()
