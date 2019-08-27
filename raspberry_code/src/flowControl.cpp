#include "flowControl.h"

flowControl::flowControl(){}
flowControl::~flowControl(){}


void* flowControl::fControl(void* a){

    std::cout << "flowControl_lib >> fControl()" << std::endl;

    sharedStruct *b;
    b = (sharedStruct*)a;

    const int flowPin = b->FLOWPIN;
    wiringPiSetupGpio();
    pinMode(flowPin, INPUT);

    double literPerMin = 0;

    while (true){

        int rate_cnt = 0, prev_cnt = 0, current_cnt = 0;
        long rate_timer = std::time(nullptr) + 60;

        while(std::time(nullptr) <= rate_timer){

            current_cnt = digitalRead(flowPin);
            if(current_cnt != prev_cnt){
                prev_cnt = current_cnt;
                rate_cnt++;
                std::cout << "flowControl_lib >> rate_cnt: " << rate_cnt -1 << std::endl;
            }
        }

        literPerMin = 15; // (rate_cnt / _constant) * 60; // times 60 to estimate liter/min
        //std::cout << "\nliters/min: " << literPerMin << std::endl;

        pthread_mutex_lock(&b->flowMeter_mutex);
        b->liters_per_min = literPerMin;
        pthread_mutex_unlock(&b->flowMeter_mutex);

    }
}

