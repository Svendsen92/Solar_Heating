#include "sharedStruct.h"

sharedStruct::sharedStruct(){
    startDiff = std::stoi(_readFromFile("startDiff"));
    relayTimer = std::stoi(_readFromFile("relayTime"));
}

sharedStruct::~sharedStruct(){}


std::string sharedStruct::_myJson(std::string json, std::string key){

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

std::string sharedStruct::_readFromFile(std::string key){

    std::ifstream file("config_data.txt");
    std::string txt;
    getline(file, txt);

    return _myJson(txt, key);
}
