#ifndef SHAREDSTRUCT_H
#define SHAREDSTRUCT_H

#include <thread>
#include <signal.h>
#include <fstream>
#include <vector>

class sharedStruct{

    private:
        std::string _readFromFile(std::string key);
        std::string _myJson(std::string json, std::string key);

    public:
        sharedStruct();
        ~sharedStruct();

        const int FLOWPIN = 2;
        const int RELAYPIN = 14;
        const int PORT = 12345;

        int startDiff = 0, relayTimer = 0;
        double liters_per_min = 0;
        std::vector<int> timeStampsDayVec, timeStampsWeekVec;
        std::vector<double> solarTempHourVec, houseTempHourVec, convexInTempHourVec, convexOutTempHourVec, kWhHourVec;
        std::vector<double> solarTempDayVec, houseTempDayVec, convexInTempDayVec, convexOutTempDayVec, kWhDayVec;
        std::string relayState = "OFF";
        pthread_mutex_t app_mutex = PTHREAD_MUTEX_INITIALIZER, flowMeter_mutex = PTHREAD_MUTEX_INITIALIZER;

};

#endif
