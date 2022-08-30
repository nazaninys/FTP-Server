#include <iostream>
#include <fstream>
#include <jsoncpp/json/json.h>
#include <bits/stdc++.h>
#include <set>
using namespace std;



class Database {
public:
	void start();
	int getCommandPort();
	int getDataPort();
	bool findUser(string);
	string getPassword(string);
	bool restrictedFile(string);
	bool isAdmin(string);
	int getUserTraffic(string);
	void setUserTraffic(string, int);

	
private:
	Json::Value root;
    Json::Reader reader;
    map<string, map<string, string>> users;
    set <string> files;
    
    void readFromFile();
    void addUserInf();
    void addFiles();

    


};