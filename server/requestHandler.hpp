#include <iostream>
#include <bits/stdc++.h>
class Server;
using namespace std;



class RequestHandler {
public:
	RequestHandler(Server*);
	void tokenizer(string);
private:
	Server *server;
	vector <string> tokens; 

	void getCommand();
	bool reqExists();
	bool checkLog();


};