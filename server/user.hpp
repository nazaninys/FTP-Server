#include <iostream>
#include <bits/stdc++.h>
using namespace std;


class User {
public:
	User(string, string);
	bool getLogged();
	string getName();
	void setLogged(bool);
	void setDir(string);
	string getDir();
	
private:
	string name;
	string password;
	bool logged;
	string directory;


};