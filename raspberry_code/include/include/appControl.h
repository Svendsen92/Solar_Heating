#ifndef APPCONTROL_H
#define APPCONTROL_H

#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>

#include "activities.h"
#include "tcp_ip.h"
#include "sharedStruct.h"

class appControl{

    private:
        void _writeToFile(std::string str);
        std::string _myJson(std::string json, std::string key);

    public:
        appControl();
        ~appControl();
        void* aControl(void *a);
};

#endif
