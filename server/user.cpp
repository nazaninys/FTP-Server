#include "user.hpp"

User::User(string username, string dir) {
	this->name = username;
	this->directory = dir;
	logged = false;
}


bool User::getLogged() {
	return logged;
}

string User::getName() {
	return name;
}

void User::setLogged(bool log) {
	logged = log;
}

void User::setDir(string dir) {
	directory = dir;
}

string User::getDir() {
	return directory;
}
