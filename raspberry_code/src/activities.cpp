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
    std::cout << "login activity" << std::endl;

    if (_myJson(json, "userName") == "admin" && _myJson(json, "passWord") == "1234"){
        std::cout << "access granted\n" << std::endl;
        return "{login: 1}";
    }else{
        std::cout << "access denied\n" << std::endl;
        return "{login: 0}";
    }
}


std::string activities::overViewActivity(std::string json){
    std::cout << "overView activity\n" << std::endl;
    
    return "{state: overView, newData: 0, roofTemp: 64, houseTemp: 32}";
}


std::string activities::setParameterActivity(std::string json){
    std::cout << "setParameter activity\n";

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


int activities::getStartDiff(){
    return _startDiff;
}
int activities::getRelayTimer(){
    return _relayTimer;
}