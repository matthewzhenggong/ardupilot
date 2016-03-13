/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

/*
   Authors: Matthew GONG

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Plane.h"

#if FIWT == ENABLED

static int16_t pwm_last[3] = {0,0,0};

void Plane::trigger_input_signal(void)
{
    int i;
    int16_t pwm[4];

    pwm[0] = g.rc_5.radio_in;
    pwm[1] = g.rc_6.radio_in;
    pwm[2] = g.rc_14.radio_in;

    for (i=0;i<4;++i) {
        if (abs(pwm[i]-pwm_last[i]) > 300) {
            if (pwm[i]>700 && pwm_last[i]>700) {
                switch (i) {
                case 0:
                    if (pwm[i]>1700) {
                        gcs_send_text_fmt(MAV_SEVERITY_INFO, "trigger pitch doublet inputs");
                        pitch_signal.start();
                        if (!enable_logging) enable_logging = true;
                    } else if (pwm[i]<1200) {
                        gcs_send_text_fmt(MAV_SEVERITY_INFO, "trigger roll doublet inputs");
                        roll_signal.start();
                        if (!enable_logging) enable_logging = true;
                    } else {
                        pitch_signal.stop();
                        roll_signal.stop();
                    }
                    break;
                case 1:
                    if (pwm[i]>1700) {

                    } else if (pwm[i]<1200) {

                    } else {

                    }
                    break;
                default :
                    enable_logging = !enable_logging;
                    if (enable_logging)
                        gcs_send_text_fmt(MAV_SEVERITY_INFO, "enable logging");
                    else
                        gcs_send_text_fmt(MAV_SEVERITY_INFO, "disable logging");
                    break;
                }
            }
            pwm_last[i] = pwm[i];
        }
    }
}

void Plane::read_control_surfaces(void)
{
    if (should_log(MASK_LOG_RC)) {
        Log_Write_RC();
        Log_Write_ElevonPos();
    }
}
#endif

