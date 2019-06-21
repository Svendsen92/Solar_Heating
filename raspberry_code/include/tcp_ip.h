#ifndef TCP_IP_H
#define TCP_IP_H

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 


class tcp_ip{

    private:
        int ds_sock;
        int ds_sock_acc;
        

    public:
        tcp_ip();
        ~tcp_ip();
        void connect(int port);
        std::string readMsg();
        void sendMsg(std::string msg);
        void closeConn();

};

#endif
