/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef SIGNAL_GENERATOR_H_
#define SIGNAL_GENERATOR_H_

#include <AP_Param/AP_Param.h>

class Plane;

class SignalGenerator
{
public:
    friend class Plane;
    SignalGenerator(void) : stage(0), t0(0) {
        AP_Param::setup_object_defaults(this, var_info);
    }

    // var_info for holding Parameter information
    static const struct AP_Param::GroupInfo var_info[];

    void start(void) {
        stage = 1;
    }

    void stop(void) {
        stage = 0;
    }

    int16_t update(void);

private:
    int stage;
    uint32_t t0;
    AP_Int16 amplitude;
    AP_Int32 tc;
    AP_Int8 singal_type;
    AP_Int8 direct;
};

#endif /* SIGNAL_GENERATOR_H_ */
