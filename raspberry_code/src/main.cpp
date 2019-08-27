#include <thread>
#include <signal.h>
#include <wiringPi.h>

#include "solarControl.h"
#include "appControl.h"
#include "flowControl.h"
#include "sharedStruct.h"


int main(int argc, const char **argv){

    appControl *appTaskPtr = new appControl();
    solarControl *solTaskPtr = new solarControl();
    flowControl *flowTaskPtr = new flowControl();
    
    sharedStruct *args = new sharedStruct();

    //start the threads
    std::thread thread1(&appControl::aControl, appTaskPtr, args);
    sleep(2);
    std::thread thread2(&solarControl::sControl, solTaskPtr, args);
    sleep(2);
    std::thread thread3(&flowControl::fControl, flowTaskPtr, args);
    sleep(2);

    thread1.join();
    thread2.join();
    thread3.join();

    return 0;
}

