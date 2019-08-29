#include "activities.h"

activities::activities(){}

activities::~activities(){}


std::string activities::_myJson(std::string json, std::string key){

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

std::string activities::loginActivity(std::string json){
    std::cout << "activities_lib >> login activity" << std::endl;

    if (_myJson(json, "userName") == "admin" && _myJson(json, "passWord") == "1234"){
        std::cout << "activities_lib >> access granted\n" << std::endl;
        return "{login: 1}";
    }else{
        std::cout << "activities_lib >> access denied\n" << std::endl;
        return "{login: 0}";
    }
}


std::string activities::overViewActivity(double *temp, double *kWh, std::string relayState){
    std::cout << "activities_lib >> overView activity\n" << std::endl;

    std::stringstream tmpCurkWh, tmpHourkWh, tmpDaykWh;
    tmpCurkWh << std::setprecision(2) << kWh[0];
    tmpHourkWh << std::setprecision(2) << kWh[1];
    tmpDaykWh << std::setprecision(2) << kWh[2];

    std::string str = "";
    str += "{state: overView, newData: 0, ";
    str += "relayState: " + relayState + ", ";
    str += "roofTemp: " + std::to_string(int(temp[0] +0.5)) + ", ";
    str += "houseTemp: " + std::to_string(int(temp[1] +0.5)) + ", ";
    str += "convexInTemp: " + std::to_string(int(temp[2] +0.5)) + ", ";
    str += "convexOutTemp: " + std::to_string(int(temp[3] +0.5)) + ", ";
    str += "currentkWh: " + tmpCurkWh.str() + ", ";
    str += "hourlykWh: " + tmpHourkWh.str() + ", ";
    str += "dailykWh: " + tmpDaykWh.str();
    str += "}";

    return str;
}


std::string activities::setParameterActivity(std::string json){
    std::cout << "activities_lib >> setParameter activity\n";

    std::string data = "";
    if (_myJson(json, "newData") == "true"){

        _startDiff = std::stoi(_myJson(json, "startDiff"));
        _relayTimer = std::stoi(_myJson(json, "relayTimer"));
        std::cout << "startDiff: " << _startDiff << std::endl;
        std::cout << "relayTimer: " << _relayTimer << std::endl;
    }
    data = "{startDiff: " + std::to_string(_startDiff);
    data += ", relayTimer: " + std::to_string(_relayTimer);
    data += "}";

    return data;
}

std::string activities::dayStatsActivity(std::vector<double> solarTemp, std::vector<double> houseTemp, std::vector<double> convexIn, std::vector<double> convexOut, std::vector<double> kWh){
    std::cout << "activities_lib >> dayStats activity\n" << std::endl;

    size_t strLen = kWh.size();
    if (strLen > 24)
    {
        strLen = 24;
    }

    std::string str = "{state: dayStats, strLen: " + std::to_string(strLen) + ", ";

    size_t i;
    for (i = 0; i < (strLen-1); ++i)
    {
        str += "kWh"  + std::to_string(i) + ": " + std::to_string(kWh[i + (kWh.size() -strLen)]) + ", ";
        str += "sol"  + std::to_string(i) + ": " + std::to_string(solarTemp[i + (solarTemp.size() -strLen)]) + ", ";
        str += "hou"  + std::to_string(i) + ": " + std::to_string(houseTemp[i + (houseTemp.size() -strLen)]) + ", "; 
        str += "cIn"  + std::to_string(i) + ": " + std::to_string(convexIn[i + (convexIn.size() -strLen)]) + ", ";
        str += "cOut" + std::to_string(i) + ": " + std::to_string(convexOut[i + (convexOut.size() -strLen)]) + ", "; 
    }

    str += "kWh"  + std::to_string(i) + ": " + std::to_string(kWh[i + (kWh.size() -strLen)]) + ", ";
    str += "sol"  + std::to_string(i) + ": " + std::to_string(solarTemp[i + (solarTemp.size() -strLen)]) + ", ";
    str += "hou"  + std::to_string(i) + ": " + std::to_string(houseTemp[i + (houseTemp.size() -strLen)]) + ", ";
    str += "cIn"  + std::to_string(i) + ": " + std::to_string(convexIn[i + (convexIn.size() -strLen)]) + ", ";
    str += "cOut" + std::to_string(i) + ": " + std::to_string(convexOut[i + (convexOut.size() -strLen)]) + "}";

    return str;
}

std::string activities::weekStatsActivity(std::vector<double> solarTemp, std::vector<double> houseTemp, std::vector<double> convexIn, std::vector<double> convexOut, std::vector<double> kWh){
    std::cout << "activities_lib >> weekStats activity\n" << std::endl;

    size_t strLen = kWh.size();
    std::string str = "{state: dayStats, strLen: " + std::to_string(strLen) + ", ";

    for (size_t i = 0; i < (strLen-1); ++i){

        str += "kWh"  + std::to_string(i) + ": " + std::to_string(kWh[i]) + ", ";
        str += "sol"  + std::to_string(i) + ": " + std::to_string(solarTemp[i]) + ", ";
        str += "hou"  + std::to_string(i) + ": " + std::to_string(houseTemp[i]) + ", "; 
        str += "cIn"  + std::to_string(i) + ": " + std::to_string(convexIn[i]) + ", ";
        str += "cOut" + std::to_string(i) + ": " + std::to_string(convexOut[i]) + ", "; 
    }

    str += "kWh"  + std::to_string(strLen -1) + ": " + std::to_string(kWh[strLen -1]) + ", ";
    str += "sol"  + std::to_string(strLen -1) + ": " + std::to_string(solarTemp[strLen -1]) + ", ";
    str += "hou"  + std::to_string(strLen -1) + ": " + std::to_string(houseTemp[strLen -1]) + ", ";
    str += "cIn"  + std::to_string(strLen -1) + ": " + std::to_string(convexIn[strLen -1]) + ", ";
    str += "cOut" + std::to_string(strLen -1) + ": " + std::to_string(convexOut[strLen -1]) + "}";

    return str;
}


int activities::getStartDiff(){
    return _startDiff;
}
int activities::getRelayTimer(){
    return _relayTimer;
}
