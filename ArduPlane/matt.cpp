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

static int16_t pwm_last[4] = {0,0,0,0};
static uint32_t t0 = 0u;
static uint16_t mode = 0u;
static uint32_t tc = 500u;
static int16_t input_dir = 1;

void Plane::trigger_input_signal(void)
{
    int i;
    int16_t pwm[4];

    pwm[0] = g.rc_5.read();
    pwm[1] = g.rc_6.read();
    pwm[2] = g.rc_8.read();
    pwm[3] = g.rc_14.read();

    uint32_t t = millis();

    for (i=0;i<4;++i) {
        if (abs(pwm[i]-pwm_last[i]) > 300) {
            if (pwm[i]>700 && pwm_last[i]>700) {
                switch (i) {
                case 0:
                    if (pwm[i]>1700) {
                        gcs_send_text_fmt(MAV_SEVERITY_INFO, "trigger pitch doublet inputs");
                        t0 = t;
                        mode |= 0x1;
                    } else if (pwm[i]<1200) {
                        gcs_send_text_fmt(MAV_SEVERITY_INFO, "trigger roll doublet inputs");
                        t0 = t;
                        mode |= 0x2;
                    } else {
                        mode &= 0xFFFC;
                    }
                    break;
                case 1:
                    if (pwm[i]>1700) {
                        tc = 700u;
                        gcs_send_text_fmt(MAV_SEVERITY_INFO, "tc=0.7s");
                    } else if (pwm[i]<1200) {
                        tc = 300u;
                        gcs_send_text_fmt(MAV_SEVERITY_INFO, "tc=0.3s");
                    } else {
                        tc = 500u;
                        gcs_send_text_fmt(MAV_SEVERITY_INFO, "tc=0.5s");
                    }
                    break;
                case 2:
                    if (pwm[i]>1700) {

                    } else if (pwm[i]<1200) {

                    } else {

                    }
                    break;
                default :
                    if (pwm[i]>1700) {
                        input_dir = -1;
                        gcs_send_text_fmt(MAV_SEVERITY_INFO, "up dir");
                    } else {
                        input_dir = 1;
                        gcs_send_text_fmt(MAV_SEVERITY_INFO, "down dir");
                    }
                    break;
                }
            }
            pwm_last[i] = pwm[i];
        }
    }

    if (mode & 0x1) {
        int16_t amp = channel_throttle->norm_input()*500;
        int16_t delta = 0;
        uint32_t dt = t - t0;
        if (dt < tc*2) {
            if (dt < tc) {
                delta = input_dir*amp;
            } else {
                delta = -input_dir*amp;
            }
            channel_pitch->set_pwm(channel_pitch->radio_in + delta);
        } else {
            mode &= 0xFFFE;
            gcs_send_text_fmt(MAV_SEVERITY_INFO, "pitch doublet inputs end(delta=%d)", amp);
        }
    } else if (mode & 0x2) {
        int16_t amp = channel_throttle->norm_input()*500;
        int16_t delta = 0;
        uint32_t dt = t - t0;
        if (dt < tc*2) {
            if (dt < tc) {
                delta = input_dir*amp;
            } else {
                delta = -input_dir*amp;
            }
            channel_roll->set_pwm(channel_roll->radio_in + delta);
        } else {
            mode &= 0xFFFD;
            gcs_send_text_fmt(MAV_SEVERITY_INFO, "roll doublet inputs end(delta=%d)", amp);
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

