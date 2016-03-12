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

int16_t SignalGenerator::update(void) {
    int16_t sig=0;
    uint32_t dt;

    switch (stage) {
    case 1:
        t0 = AP_HAL::millis();
        stage = 2;
        break;
    case 2:
        dt = AP_HAL::millis()-t0;
        switch (singal_type) {
        default:
            if (dt < (uint32_t)tc) {
                if (direct)
                    sig = amplitude;
                else
                    sig = -amplitude;
            } else if (dt < 2*(uint32_t)tc) {
                if (direct)
                    sig = -amplitude;
                else
                    sig = amplitude;
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
