#ifndef CLIENT
#define CLIENT
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <bits/stdc++.h>
#include <fstream>
#include <jsoncpp/json/json.h>
#define TRUE 1
#define ADDRESS "255.255.255.255"
using namespace std;

#define SIZE 2000

class Client {
public:
	void start();
private:
	Json::Value root;
    Json::Reader reader;
	int commandPort, dataPort;
	int sockid, dataId;
	struct sockaddr_in serv_addr, data_addr; 
	void getPorts();
	void connect_to_server();
	void connect_to_data_port();
	void communicate();
	void ls();
	void retr(string);

};


#endif