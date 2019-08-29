#include "appControl.h"

appControl::appControl(){}
appControl::~appControl(){}

void appControl::_writeToFile(std::string str){
    std::ofstream myfile;

    myfile.open("config_data.txt");
    myfile << str;
    myfile.close();
}

std::string appControl::_myJson(std::string json, std::string key){

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


void* appControl::aControl(void *a){
    std::cout << "appControl_lib >> aControl()" << std::endl;

    sharedStruct *b;
    b = (sharedStruct*)a;

    tcp_ip tcp;
    activities act;

    const int port = b->PORT;

    while (true){

        tcp.connect(port);

        std::string json = tcp.readMsg();
        std::cout << "appControl_lib >> json: " << json << std::endl;

        std::string state = _myJson(json, "state");

        if (state == "login"){
            tcp.sendMsg(act.loginActivity(json));
            tcp.closeConn();

        }else if (state == "overView"){
            pthread_mutex_lock(&b->app_mutex);
            size_t len = b->solarTempHourVec.size();
            double sensorTemps[4] = {b->solarTempHourVec[len -1], b->houseTempHourVec[len -1], b->convexInTempHourVec[len -1], b->convexOutTempHourVec[len -1]};

        
            int lenDay = 0;
            if (b->kWhDayVec.size() > 24){
                lenDay = b->kWhDayVec.size() - 24;
            }

            double curkWh = b->kWhHourVec[len -1]; 
            double hourkWh = std::accumulate(b->kWhHourVec.begin(), b->kWhHourVec.end(), 0.0) / b->kWhHourVec.size(); // accumulated average 
            double daykWh = std::accumulate(b->kWhDayVec.begin() + lenDay, b->kWhDayVec.end(), 0.0); // accumulated sum

            double kWh[3] = {curkWh, hourkWh, daykWh};
            std::string relayState = b->relayState;
            pthread_mutex_unlock(&b->app_mutex);

            tcp.sendMsg(act.overViewActivity(sensorTemps, kWh, relayState));
            tcp.closeConn();

        }else if (state == "setParameter"){
            pthread_mutex_lock(&b->app_mutex);
            act.setParameterActivity(json);
            b->startDiff = act.getStartDiff();
            b->relayTimer = act.getRelayTimer();
            pthread_mutex_unlock(&b->app_mutex);

            tcp.sendMsg(act.setParameterActivity(json));
            tcp.closeConn();

            std::string str;
            str = "startDiff: " + std::to_string(act.getStartDiff()) + ", ";
            str += "relayTime: " + std::to_string(act.getRelayTimer());
            _writeToFile(str);

        }else if (state == "dayStats"){
            std::vector<double> kWh, solarTemp, houseTemp, convexIn, convexOut;

            pthread_mutex_lock(&b->app_mutex);
            kWh = b->kWhDayVec;
            solarTemp = b->solarTempDayVec;
            houseTemp = b->houseTempDayVec;
            convexIn = b->convexInTempDayVec;
            convexOut = b->convexOutTempDayVec;
            pthread_mutex_unlock(&b->app_mutex);

            tcp.sendMsg(act.dayStatsActivity(solarTemp, houseTemp, convexIn, convexOut, kWh));    
            tcp.closeConn();

        }else if (state == "weekStats"){
            std::vector<double> kWh, solarTemp, houseTemp, convexIn, convexOut;

            pthread_mutex_lock(&b->app_mutex);
            kWh = b->kWhDayVec;
            solarTemp = b->solarTempDayVec;
            houseTemp = b->houseTempDayVec;
            convexIn = b->convexInTempDayVec;
            convexOut = b->convexOutTempDayVec;
            pthread_mutex_unlock(&b->app_mutex);

            tcp.sendMsg(act.weekStatsActivity(solarTemp, houseTemp, convexIn, convexOut, kWh));    
            tcp.closeConn();

        }else{
            std::cout << "appControl_lib >> state not found\n";
            tcp.closeConn();
        }
    }
}
