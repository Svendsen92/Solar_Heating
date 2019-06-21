#include "tcp_ip.h"


tcp_ip::tcp_ip(){}
tcp_ip::~tcp_ip(){}


void tcp_ip::connect(int port){

    int optval = 1;
    struct sockaddr_in my_addr;
    if ((ds_sock=socket(AF_INET,SOCK_STREAM,0)) == 0){
        std::cout << "socket failed\n" << std::endl;
        exit(0);
    }

    if (setsockopt(ds_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))){
        std::cout << "setsockopt failed\n" << std::endl;
        exit(0);
    }

    memset(&my_addr,0,sizeof(my_addr));
    my_addr.sin_family=AF_INET;
    my_addr.sin_port=htons(port);
    my_addr.sin_addr.s_addr=INADDR_ANY;

    if(bind(ds_sock,(struct sockaddr *)&my_addr,sizeof(my_addr))<0){
        std::cout << "bind failed\n" << std::endl;
        exit(0);
    }
        
    if (listen(ds_sock,2) < 0){
        std::cout << "listen timed-out\n" << std::endl;
        exit(0);
    }

    struct sockaddr_in addr;
    size_t sin_size = sizeof(struct sockaddr_in);

    if((ds_sock_acc=accept(ds_sock,(struct sockaddr *)&addr,&sin_size))<1){
        std::cout << "accept failed\n" << std::endl;
        exit(0);
    }
}

std::string tcp_ip::readMsg(){

    char buffer[1024] = {0};
    recv(ds_sock_acc, buffer, 1024, 0);

    return buffer;
}

void tcp_ip::sendMsg(std::string msg){

    if (send(ds_sock_acc, msg.c_str(), strlen(msg.c_str()), 0) < 0){
        std::cout << "falied to send message\n" << std::endl;
        exit(0);
    }
}

void tcp_ip::closeConn(){

    close(ds_sock_acc);
    close(ds_sock);

}

