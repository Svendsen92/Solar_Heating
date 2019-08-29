#include "timing.h"

timing::timing(){}
timing::~timing(){}

int timing::getDay(){
    time_t now = time(0);
    tm *ltm = localtime(&now);
    return ltm->tm_mday;
}

int timing::getHour(){
    time_t now = time(0);
    tm *ltm = localtime(&now);
    return ltm->tm_hour;
}

int timing::getMinute(){
    time_t now = time(0);
    tm *ltm = localtime(&now);
    return ltm->tm_min;
}