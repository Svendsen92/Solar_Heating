#ifndef SOLARCONTROL_H
#define SOLARCONTROL_H

#include <iostream>
#include <vector>
#include <thread>
#include <signal.h>
#include <numeric>
#include <fcntl.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <cmath>
#include <algorithm>

#include <wiringPi.h>
#include "timing.h"
#include "sharedStruct.h"
#include "csv_logger.h"

class solarControl{

    private:
        timing _t;
        std::time_t _relayTimer = std::time(nullptr);

        std::string _solar_ID = "28-0516903509ff", _house_ID = "28-0416913a34ff";
        std::string _convexIn_ID = "28-0517c494cfff", _convexOut_ID = "28-051692162aff";

        int _sAppDiff, _rAppTimer;
        double _solarTemp = _getTemp(_solar_ID), _houseTemp = _getTemp(_house_ID);
        double _convexInTemp = _getTemp(_convexIn_ID), _convexOutTemp = _getTemp(_convexOut_ID);
        const double _KjToKWhConstant = 0.0002777778, _specificHeatCapasity = 4.179; // specific heat capasity of water = 4.179kj/(kg*C);

        void _updateDoubleVector(std::vector<double> *vec, double vecUpdate, size_t vecLen);
        void _updateIntVector(std::vector<int> *vec, int vecUpdate, size_t vecLen);
        double _getTemp(std::string deviceId);
        bool _pumpActivation(int relayPin, std::time_t relayTimer);

    public:
        solarControl();
        ~solarControl();
        void* sControl(void* a);
};

#endif
