#ifndef CSV_LOGGER_H
#define CSV_LOGGER_H

#include <iostream> 
#include <fstream>
#include <ctime>


class csv_logger{

    private:
        std::string _getTimeStamp();

    public:
        csv_logger();
        ~csv_logger();
        int appendToFile(std::string fileName, std::string txt);

};

#endif