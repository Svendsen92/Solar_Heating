#include "csv_logger.h"

csv_logger::csv_logger(){}

csv_logger::~csv_logger(){}



std::string csv_logger::_getTimeStamp() {
   
   	time_t curr_time;
	tm * curr_tm;
	char date_string[100];
	
	time(&curr_time);
	curr_tm = localtime(&curr_time);
	strftime(date_string, 50, "%d/%m/%Y-%T", curr_tm);
	
   	return date_string;
}


int csv_logger::appendToFile (std::string fileName, std::string txt) {

	std::string timeStamp = _getTimeStamp();

	std::ofstream myfile (fileName + ".csv", std::ios::out | std::ios::app);

  	if (myfile.is_open()) {

    	myfile << timeStamp + "," + txt + "\n";
	    myfile.close();
	    return 0;
	}
	else { 
	  	std::cout << "Unable to open file\n";
	  	return 1;
	}
}