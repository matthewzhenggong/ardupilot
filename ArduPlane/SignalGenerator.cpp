/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include "SignalGenerator.h"

#include <AP_HAL/system.h>

const AP_Param::GroupInfo SignalGenerator::var_info[] = {
    // @Param: AMP
    // @DisplayName: Amplitude
    // @Description: Max amplitude of pwm signal
    // @Range: 0 500
    // @User: Advanced
    AP_GROUPINFO("AMP",    0, SignalGenerator, amplitude, 200),

    // @Param: TC
    // @DisplayName: Time Constant
    // @Description: TC for signal
    // @Range: 300 5000
    // @User: Advanced
    AP_GROUPINFO("TC",     1, SignalGenerator, tc, 500),

    // @Param: TYPE
    // @DisplayName: Type of Signal
    // @Description: 0-doublet
    // @Range: 0 3
    // @User: Advanced
    AP_GROUPINFO("TYPE",   2, SignalGenerator, singal_type, 0),

    // @Param: DIR
    // @DisplayName: Direction of Signal
    // @Description: 1-up;other-down
    // @Range: 1 0
    // @User: Advanced
    AP_GROUPINFO("DIR",    3, SignalGenerator, direct, 1),

    AP_GROUPEND
};

static float mayer[]={0.0006, -0.0005, -0.0017, -0.0029, -0.0039, -0.0046, -0.0048, -0.0044, -0.0031, -0.0008,
        0.0023, 0.0056, 0.0086, 0.0107, 0.0116, 0.0111, 0.0089, 0.0047, -0.0013, -0.0085,
        -0.0158, -0.0218, -0.0252, -0.0254, -0.0217, -0.0139, -0.0023, 0.0123, 0.0271, 0.0388,
        0.0444, 0.0433, 0.0377, 0.0304, 0.0217, 0.0078, -0.0174, -0.0554, -0.0978, -0.1264,
        -0.1213, -0.0754, -0.0033, 0.0629, 0.0926, 0.0790, 0.0500, 0.0527, 0.1189, 0.2301,
        0.3113, 0.2655, 0.0371, -0.3334, -0.7021, -0.8792, -0.7272, -0.2479, 0.3953, 0.9418,
        1.1554};

int16_t SignalGenerator::update(void) {
    int16_t sig=0;
    uint32_t dt;
    float t;
    int ts;

    switch (stage) {
    case 1:
        t0 = AP_HAL::millis();
        stage = 2;
        break;
    case 2:
        dt = AP_HAL::millis()-t0;
        switch (singal_type) {
        case 1:
            if (dt < 1000u) {
                sig = 0;
            } else if (dt < (uint32_t)tc+1000u) {
                sig = amplitude*((dt-1000u)/(float)tc);
                if (!direct)
                    sig = -sig;
            } else if (dt < (uint32_t)tc*2u+1000u) {
                sig = amplitude*((dt-1000u)/(float)tc*-1.0f +2.0f);
                if (!direct)
                    sig = -sig;
            } else {
                sig = 0;
                stage = 0;
            }
            break;
        case 2:
            t = (dt/(float)tc)/0.125f;
            ts = int(t);
            if (ts < 120) {
                if (ts >=60) {
                    t = 120-t;
                    ts = int(t);
                }
                sig = amplitude*(mayer[ts]+(mayer[ts+1]-mayer[ts])*(t-ts));
            } else {
                sig = 0;
                stage = 0;
            }
            if (!direct)
                sig = -sig;
            break;
        default:
            if (dt < (uint32_t)tc) {
                sig = 0;
            } else if (dt < 2u*(uint32_t)tc) {
                if (direct)
                    sig = amplitude;
                else
                    sig = -amplitude;
            } else if (dt < 3*(uint32_t)tc) {
                if (direct)
                    sig = -amplitude;
                else
                    sig = amplitude;
            } else if (dt < 13*(uint32_t)tc) {
                sig = 0;
            } else {
                stage = 0;
            }
            break;
        }
        break;
    default :
        break;
    }
    return sig;
}
