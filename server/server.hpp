#include <stdio.h> 
#include <string.h> 
#include <string> 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> 
#include <arpa/inet.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> 
#include <iostream>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cstdio>
#include <ctime>
#include "database.hpp"
#include "user.hpp"
using namespace std;

class RequestHandler;
	
#define SIZE 30
#define MSGSIZE 2000

class Server {
public: 
	void start();
	Server();
	~Server();
	void user(string);
	void pass(string);
	bool checkLog();
	void badReq();
	void pwd();
	void mkd(string);
	void ls();
	void cwd(string);
	void deled(string);
	void delef(string);
	void changeName(string, string);
	void retr(string);
	void help();
	void quit();


	
	
private:
	time_t now;
	FILE* logFile;
	int commandPort, dataPort;
	int serverSocket;
	struct sockaddr_in address;
	fd_set readfds;
	int client_socket[SIZE];
	int max_clients;
	int serverDataSocket;
	struct sockaddr_in dataAddress;
	fd_set readDatafds;
	int data_socket[SIZE];
	Database db;
	RequestHandler* reqHandler;
	map <int, User*> users;
	int curId, curData;
	string curDir;
	map<int, int> ports;
	int channel;

	void createCommandSocket(); 
	void serviceClients();
	int addTofdSet(int);
	void newConnection();
	bool socketIO();
	void createDataSocket(); 
	int dataTransfer(int);
	int addToDatafdSet(int);
	void newDataConnection();
	void dataSocketIO();
	void runDatabase();
	bool alreadyLogged();
	void needAccount();
	void removeFile(string);
	void downloadFile(string);
	bool checkTraffic(FILE*);
	void error();
	void fileAccessError();
	void printLog(string);
	bool hasAccess(string);




};