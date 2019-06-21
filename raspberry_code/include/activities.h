#ifndef ACTIVITIES_H
#define ACTIVITIES_H

#include <iostream>

class activities{

    private:
        std::string _myJson(std::string json, std::string key);
        int _startDiff = 0;
        int _relayTimer = 0;

    public:
        activities();
        ~activities();
        std::string loginActivity(std::string json);
        std::string overViewActivity(std::string json);
        std::string setParameterActivity(std::string json);
        int getStartDiff();
        int getRelayTimer();
        
};

#endif
