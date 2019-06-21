#include <iostream>
#include <thread>
#include <signal.h>
#include <string>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctime>

#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 

#include <wiringPi.h>

#include "tcp_ip.h"
#include "activities.h"


std::string myJson(std::string json, std::string key){

    int cnt = 0;
    int jsonLen = json.length();
    std::string k = "";
    std::string data = "";

    for (int i = 0; i < jsonLen; i++){

        if (json[i] == key[cnt]){
            k += json[i];
            if (k == key){
                int idx = i +3;
                while (true){
                    data += json[idx];
                    idx++;
                    if (json[idx] == ',' || json[idx] == '}'){
                        return data;
                    }else if (idx > jsonLen){
                        break;
                    }
                }
            }
            cnt++;
        }
    }
    return data;
}

int getTemp(std::string device){

    int temp = 0;
    char buf[256];          // Data from device
    char tmpData[6];        // Temp Celcius reported by device 
    std::string devPath = "/sys/bus/w1/devices/" + device + "/w1_slave";

    int fd = open(devPath.c_str(), O_RDONLY);
    if(fd == -1){
        perror ("Couldn't open the w1 device...");
        return 1;   
    }
        
    if (read(fd, buf, 256) > 0){
        strncpy(tmpData, strstr(buf, "t=") + 2, 5); 
        temp = strtof(tmpData, NULL)/1000;
    }

    close(fd); 
    return temp;
}

double waterFlow(int Pin){
    bool impluse = false;
    const double constant = 6.6;
    int rate_cnt = 0;
    long rate_timer = std::time(nullptr) + 60;

    while (std::time(nullptr) <= rate_timer){

        if(digitalRead(Pin) == 1 && impluse == false){
            std::cout << "rate_cnt: " << rate_cnt << std::endl;
            impluse = true;
            rate_cnt++;
        }else if (digitalRead(Pin) == 0){
            impluse = false;
        }
    }
    return (rate_cnt / constant);
}

void pumpActivation(int solarTemp, int houseTemp, int sAppDiff, int rAppTimer, std::time_t relayTimer){

    const int relayPin = 17;

    if (solarTemp > 95){
        std::cout << "solarTemp > 95... relay ON" << std::endl;
        digitalWrite(relayPin, 1); // 1 = high
        relayTimer = std::time(nullptr);
    }else if((solarTemp - houseTemp) > sAppDiff){
        std::cout << "relay ON startDiff exceeded" << std::endl;
        digitalWrite(relayPin, 1); // 1 = high
        relayTimer = std::time(nullptr);
    }else if(std::time(nullptr) > (relayTimer + rAppTimer)){
        digitalWrite(relayPin, 0); // 0 = low
    }

}

struct thread_args {

    int startDiff = 0, relayTimer = 0;
    double liters_per_min;
    bool newMinFlowData; 
    pthread_mutex_t app_mutex, flowMeter_mutex;
};

void* appConnection(void* a){

    std::cout << "appConnection()" << std::endl;

    thread_args *b;
    b = (thread_args*)a;

    tcp_ip tcp;
    activities act;

    const int PORT = 12345;

    while (true){

        tcp.connect(PORT);

        std::string json = tcp.readMsg();
        std::cout << "json: " << json << std::endl;

        std::string state = myJson(json, "state");

        if (state == "login"){
            tcp.sendMsg(act.loginActivity(json));
        }else if (state == "overView"){
            tcp.sendMsg(act.overViewActivity(json));
        }else if (state == "setParameter"){
            tcp.sendMsg(act.setParameterActivity(json));
        }else{
            std::cout << "state not found\n"; 
        }

        tcp.closeConn();

        pthread_mutex_lock(&b->app_mutex);
        b->startDiff = act.getStartDiff();
        b->relayTimer = act.getRelayTimer();
        pthread_mutex_unlock(&b->app_mutex);

    }
}

void* control(void* a){

    std::cout << "control()" << std::endl;

    thread_args *b;
    b = (thread_args*)a;

    std::string dev = "28-0517c494cfff"; // Device/sensor ID
    int sAppDiff = 0, rAppTimer = 0;
    int solarTemp = getTemp(dev), houseTemp = 0;
    int hour_cnt = 0, day_cnt = 0;
    double liters_per_min, liters_per_hour, liters_per_day;
    bool newMinFlowData;
    std::time_t relayTimer = std::time(nullptr);
    

    while (true){

        pthread_mutex_lock(&b->app_mutex);
        pthread_mutex_lock(&b->flowMeter_mutex);
        sAppDiff = b->startDiff;
        rAppTimer = b->relayTimer;
        newMinFlowData = b->newMinFlowData;
        pthread_mutex_unlock(&b->flowMeter_mutex);
        pthread_mutex_unlock(&b->app_mutex);

        sleep(2);
        solarTemp = getTemp(dev);
        houseTemp = 25;
        std::cout << "\nsDiff: " << sAppDiff << std::endl;
        std::cout << "rTimer: " << rAppTimer << std::endl;
        std::cout << "temp: " << solarTemp << "\n" << std::endl;
        
        pumpActivation(solarTemp, houseTemp, sAppDiff, rAppTimer, relayTimer);        

        if(newMinFlowData){
            pthread_mutex_lock(&b->flowMeter_mutex);
            liters_per_min = b->liters_per_min;
            b->newMinFlowData = false;
            pthread_mutex_unlock(&b->flowMeter_mutex);

            if (hour_cnt < 61){ // 1hour = 60min
                liters_per_hour += liters_per_min; 
                hour_cnt++;
            }else{
                hour_cnt = 0;
                liters_per_hour = 0;
                std::cout << "liters/hour: " << liters_per_hour << std::endl;
            }

            if (day_cnt < 1441){ //  1day = 1440min 
                liters_per_day += liters_per_min;
                day_cnt++;
            }else{
                day_cnt = 0;
                liters_per_day = 0;
                std::cout << "liters/day: " << liters_per_day << std::endl;
            }
        }


    }
}

void* flowMeter(void* a){

    std::cout << "flowMeter()" << std::endl;

    thread_args *b;
    b = (thread_args*)a;

    const int flowPin = 2;

    double liters_per_min = 0;

    while (true){

        liters_per_min = waterFlow(flowPin);
        std::cout << "\nliters/min: " << liters_per_min << std::endl;

        pthread_mutex_lock(&b->flowMeter_mutex);
        b->liters_per_min = liters_per_min;
        b->newMinFlowData = true;
        pthread_mutex_unlock(&b->flowMeter_mutex);

    }
}

int main(int argc, const char **argv){

    const int relayPin = 17, flowPin = 2;

    wiringPiSetupGpio();
    pinMode(relayPin, OUTPUT); 
    pinMode(flowPin, INPUT);
    digitalWrite(relayPin, 0); // 0 = low


    thread_args *args = new thread_args;

    //start the threads
    std::thread thread1(appConnection, args);
    sleep(2);
    std::thread thread2(control, args);
    sleep(2);
    std::thread thread3(flowMeter, args);
    sleep(2);

    thread1.join();
    thread2.join();
    thread3.join();

    return 0;
}

