#include "solarControl.h"

solarControl::solarControl(){}
solarControl::~solarControl(){}


void solarControl::_updateDoubleVector(std::vector<double> *vec, double vecUpdate, size_t vecLen){

    if (vec->size() < vecLen){ // 1day = 24hour
        vec->push_back(vecUpdate);
    }else{
        vec->erase(vec->begin()); // erases the first element
        vec->push_back(vecUpdate);      // puts in new measurement at the end of the vector
    }
}

void solarControl::_updateIntVector(std::vector<int> *vec, int vecUpdate, size_t vecLen){

    if (vec->size() < vecLen){ // 1day = 24hour
        vec->push_back(vecUpdate);
    }else{
        vec->erase(vec->begin()); // erases the first element
        vec->push_back(vecUpdate);      // puts in new measurement at the end of the vector
    }
}

double solarControl::_getTemp(std::string deviceId){

    double temp = 0;
    char buf[256];          // Data from device
    char tmpData[6];        // Temp Celcius reported by device
    std::string devPath = "/sys/bus/w1/devices/" + deviceId + "/w1_slave";

    int fd = open(devPath.c_str(), O_RDONLY);
    if(fd == -1){
        perror ("solarControl >> Couldn't open the w1 device...");
        return 1;
    }

    if (read(fd, buf, 256) > 0){
        strncpy(tmpData, strstr(buf, "t=") + 2, 6);
        temp = strtof(tmpData, NULL)/1000;
    }

    close(fd);
    return temp;
}

bool solarControl::_pumpActivation(int relayPin, std::time_t relayTimer){

    bool On = false;

    if (_solarTemp > 95){
        std::cout << "solarControl_lib >> solarTemp > 95... relay ON" << std::endl;
        digitalWrite(relayPin, 1); // 1 = high
        On = true;
    }else if((_solarTemp - _houseTemp) > _sAppDiff){
        std::cout << "solarControl_lib >> startDiff exceeded... relay ON" << std::endl;
        digitalWrite(relayPin, 1); // 1 = high
        On = true;
    }else if((_convexInTemp - _convexOutTemp) > 2){
        std::cout << "solarControl_lib >> startDiff exceeded... relay ON" << std::endl;
        digitalWrite(relayPin, 1); // 1 = high
        On = true;
    }else if(std::time(nullptr) > (relayTimer + _rAppTimer)){
        std::cout << "solarControl_lib >> Relay OFF" << std::endl;
        digitalWrite(relayPin, 0); // 0 = low
        On = false;
    }

    return On;
}


void* solarControl::sControl(void* a){

    std::cout << "solarControl_lib >> sControl()" << std::endl;

    csv_logger log;
    log.appendToFile("csv_log", "solartemp,houseTemp,convexInTemp,convexOutTemp,hourly_kWh,daily_kWh\n");

    sharedStruct *b;
    b = (sharedStruct*)a;

    const int relayPin = b->RELAYPIN;
    wiringPiSetupGpio();
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, 0); // 0 = low

    std::string relayState = "OFF";
    int prev_day = -1, prev_hour = -1, prev_min = -1;
    double literPerMin = 0;

    std::vector<double> solarTempDayVec, houseTempDayVec, convexInTempDayVec, convexOutTempDayVec, kWhDayVec;
    std::vector<double> solarTempHourVec, houseTempHourVec, convexInTempHourVec, convexOutTempHourVec, kWhHourVec;

    int lenDay = 0;

    while (true){

        pthread_mutex_lock(&b->app_mutex);
        pthread_mutex_lock(&b->flowMeter_mutex);
        _sAppDiff = b->startDiff;
        _rAppTimer = b->relayTimer;
        literPerMin = b->liters_per_min;
        b->kWhHourVec = kWhHourVec;
        b->solarTempHourVec = solarTempHourVec;
        b->houseTempHourVec = houseTempHourVec;
        b->convexInTempHourVec = convexInTempHourVec;
        b->convexOutTempHourVec = convexOutTempHourVec;
        b->kWhDayVec = kWhDayVec;
        b->solarTempDayVec = solarTempDayVec;
        b->houseTempDayVec = houseTempDayVec;
        b->convexInTempDayVec = convexInTempDayVec;
        b->convexOutTempDayVec = convexOutTempDayVec;
        b->relayState = relayState;
        pthread_mutex_unlock(&b->flowMeter_mutex);
        pthread_mutex_unlock(&b->app_mutex);

        // introduces a delay into the thread to reduce processor load
        sleep(7);

        _solarTemp = _getTemp(_solar_ID);
        _houseTemp = _getTemp(_house_ID);
        _convexInTemp = _getTemp(_convexIn_ID);
        _convexOutTemp = _getTemp(_convexOut_ID);
        std::cout << "\nsolarControl_lib >> sAppDiff: " << _sAppDiff << std::endl;
        std::cout << "solarControl_lib >> rAppTimer: " << _rAppTimer << std::endl;
        std::cout << "solarControl_lib >> solartemp: " << _solarTemp << std::endl;
        std::cout << "solarControl_lib >> housetemp: " << _houseTemp << std::endl;
        std::cout << "solarControl_lib >> convexInTemp: " << _convexInTemp << std::endl;
        std::cout << "solarControl_lib >> convexOutTemp: " << _convexOutTemp << std::endl;

        // keeps track of the time that the pump has been running
        if (_pumpActivation(relayPin, _relayTimer)){

            relayState = "ON";
        }else{
            std::cout << "solarControl_lib >> Relay OFF" << std::endl;
            relayState = "OFF";

            literPerMin = 0;
        }

        // keeps track of hourly update
        if (_t.getMinute() != prev_min){
            prev_min = _t.getMinute();

            // calculates the estimated current kWh 
            double currentkWh = fabs(_specificHeatCapasity * literPerMin * (_convexInTemp - _convexOutTemp) * _KjToKWhConstant * 60); // times "60" to estimate hourly based on minutely
            
            _updateDoubleVector(&kWhHourVec, currentkWh, 60);
            _updateDoubleVector(&solarTempHourVec, _solarTemp, 60);
            _updateDoubleVector(&houseTempHourVec, _houseTemp, 60);
            _updateDoubleVector(&convexInTempHourVec, _convexInTemp, 60);
            _updateDoubleVector(&convexOutTempHourVec, _convexOutTemp, 60);
        }

        // keeps track of daily update
        if (_t.getHour() != prev_hour){
            prev_hour = _t.getHour();
            _updateDoubleVector(&kWhDayVec, (std::accumulate(kWhHourVec.begin(), kWhHourVec.end(), 0.0) / kWhHourVec.size()), 24 *7);
            _updateDoubleVector(&solarTempDayVec, (std::accumulate(solarTempHourVec.begin(), solarTempHourVec.end(), 0.0) / solarTempHourVec.size()), 24 *7);
            _updateDoubleVector(&houseTempDayVec, (std::accumulate(houseTempHourVec.begin(), houseTempHourVec.end(), 0.0) / houseTempHourVec.size()), 24 *7);
            _updateDoubleVector(&convexInTempDayVec, (std::accumulate(convexInTempHourVec.begin(), convexInTempHourVec.end(), 0.0) / convexInTempHourVec.size()), 24 *7);
            _updateDoubleVector(&convexOutTempDayVec, (std::accumulate(convexOutTempHourVec.begin(), convexOutTempHourVec.end(), 0.0) / convexOutTempHourVec.size()), 24 *7);

            std::string txt = std::to_string(_solarTemp) + "," + std::to_string(_houseTemp) + "," + std::to_string(_convexInTemp) + ",";
            txt += std::to_string(_convexOutTemp) + "," + std::to_string(std::accumulate(kWhHourVec.begin(), kWhHourVec.end(), 0.0));
            log.appendToFile("csv_log", txt);
        }

        // keeps track of weekly update
        if (_t.getDay() != prev_day){
            prev_day = _t.getDay();

            if (kWhDayVec.size() > 24){
                lenDay = kWhDayVec.size() - 24;
            }else{
                lenDay = 0;
            }

            std::string txt = ", , , , ," + std::to_string(std::accumulate(kWhDayVec.begin() + lenDay, kWhDayVec.end(), 0.0)) + "\n";
            log.appendToFile("csv_log", txt);
        }


        std::cout << "solarControl_lib >> currentkWh: " << kWhHourVec[kWhHourVec.size() -1] << "kWh" << std::endl;
        std::cout << "solarControl_lib >> hourly_kwh: " << std::accumulate(kWhHourVec.begin(), kWhHourVec.end(), 0.0) / kWhHourVec.size() << "kWh" << std::endl;
        std::cout << "solarControl_lib >> daily_kwh: " << std::accumulate(kWhDayVec.begin() + lenDay, kWhDayVec.end(), 0.0) << "kWh" << std::endl;

    }
}

