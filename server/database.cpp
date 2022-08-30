#include "database.hpp"


void Database::start() {
	readFromFile();
	addUserInf();
	addFiles();
}


void Database::readFromFile() {

	ifstream data("config.json", std::ifstream::binary);
    reader.parse(data, root, false);
}

int Database::getCommandPort() {
	Json::Value port = root["commandChannelPort"];
	return port.asInt();
}

int Database::getDataPort() {
	Json::Value port = root["dataChannelPort"];
	return port.asInt();
}

void Database::addUserInf() {
	Json::Value usersArray = root["users"];
	for(int i=0; i < usersArray.size(); i++) {
		map <string, string> inf;
		for (auto const& id : usersArray[i].getMemberNames()) {
			string key = id;
			inf.insert({id, usersArray[i][id].asString()});
		}
		inf["size"] = to_string(stoi(inf["size"]) * 1000);
		users.insert({usersArray[i]["user"].asString(), inf});
	}
}

void Database::addFiles() {
	Json::Value filesArray = root["files"];
	for (int i=0; i<filesArray.size(); i++)
		files.insert(filesArray[i].asString());
}

bool Database::findUser(string name) {
	if(users.find(name) != users.end())
		return true;
	else
		return false;
}

string Database::getPassword(string name) {
	map<string, string> user = users[name];
	return user["password"];
}

bool Database::restrictedFile(string filename) {
	if (files.find(filename) != files.end())
		return true;
	else 
		return false;
}

bool Database::isAdmin(string name) {
	if(users[name]["admin"].compare("true") == 0)
		return true;
	else 
		return false;
}

int Database::getUserTraffic(string name) {
	return stoi(users[name]["size"]);
} 

void Database::setUserTraffic(string name, int usedTraffic) {
	int newSize = stoi(users[name]["size"]) - usedTraffic;
	users[name]["size"] = to_string(newSize);
}