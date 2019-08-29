#ifndef ACTIVITIES_H
#define ACTIVITIES_H

#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>

class activities{

    private:
        std::string _myJson(std::string json, std::string key);
        int _startDiff = 0;
        int _relayTimer = 0;

    public:
        activities();
        ~activities();
        std::string loginActivity(std::string json);
        std::string overViewActivity(double *temp, double *kWh, std::string relayState);
        std::string setParameterActivity(std::string json);
        std::string dayStatsActivity(std::vector<double> solarTemp, std::vector<double> houseTemp, std::vector<double> convexIn, std::vector<double> convexOut, std::vector<double> kWh);
        std::string weekStatsActivity(std::vector<double> solarTemp, std::vector<double> houseTemp, std::vector<double> convexIn, std::vector<double> convexOut, std::vector<double> kWh);
        int getStartDiff();
        int getRelayTimer();

};

#endif
