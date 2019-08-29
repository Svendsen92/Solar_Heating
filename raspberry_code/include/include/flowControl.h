#ifndef FLOWCONTROL_H
#define FLOWCONTROL_H

#include <iostream>
#include <vector>
#include <numeric>
#include <wiringPi.h>

#include "timing.h"
#include "sharedStruct.h"

class flowControl{

    private:
        timing _t;
        const double _constant = 6.6;

    public:
        flowControl();
        ~flowControl();
        void* fControl(void* a);
};

#endif
