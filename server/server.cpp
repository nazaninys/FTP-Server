#include "server.hpp"
#include "requestHandler.hpp"


Server::Server() {
	max_clients = SIZE;
	reqHandler = new RequestHandler(this);
	char dir[FILENAME_MAX]; 
	getcwd(dir, FILENAME_MAX);
	string temp(dir);
	curDir = temp;
	now = time(0);
}

Server::~Server() {
	fclose(logFile);
} 

void Server::start() {
	runDatabase();
	commandPort = db.getCommandPort();
	dataPort = db.getDataPort();
	createCommandSocket();
	createDataSocket();
	serviceClients();
}



void Server::createCommandSocket() {
	int opt = 1; 
	serverSocket = socket(AF_INET , SOCK_STREAM , 0);
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
	
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons(commandPort); 

	bind(serverSocket, (struct sockaddr *)&address, sizeof(address));
	listen(serverSocket, 5);
	cout<<"waiting for conections to command socket"<<endl;
}

void Server::serviceClients() {
	for (int i = 0; i < max_clients; i++) 
		client_socket[i] = 0; 
	for (int i = 0; i < max_clients; i++) 
		data_socket[i] = 0; 
	int max_sd, max_data;
	while(1) { 
		
		FD_ZERO(&readfds); 
		FD_SET(serverSocket, &readfds); 
		max_sd = serverSocket; 

		max_sd = addTofdSet(max_sd);
		
		select( max_sd + 1 , &readfds , NULL , NULL , NULL); 

		if (FD_ISSET(serverSocket, &readfds)) {
			newConnection();
			max_data = dataTransfer(max_data);
		}

		else {
			if(socketIO()) 
				max_data = dataTransfer(max_data);
		}
	} 
}

int Server::addTofdSet(int max_sd) {
	int sd;
	for (int i = 0 ; i < max_clients ; i++) { 
			
		sd = client_socket[i]; 
		if(sd > 0) 
			FD_SET( sd , &readfds); 
		if(sd > max_sd) 
			max_sd = sd; 
	} 

	return max_sd;
}

void Server::newConnection() {
	int addrlen = sizeof(address);
	int new_socket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t*)&addrlen);
	
	for (int i = 0; i < max_clients; i++) { 
		
		if( client_socket[i] == 0 ) { 
			client_socket[i] = new_socket; 
			channel = client_socket[i];
			cout<<"new client connected to command socket"<<endl;
			break; 
		} 
	} 
}

bool Server::socketIO() {
	char buffer[MSGSIZE + 1];
	int valread;
	for (int i = 0; i < max_clients; i++) { 
		curId = client_socket[i]; 
			
		if (FD_ISSET( curId , &readfds)) { 
			
			if ((valread = read( curId , buffer, MSGSIZE)) == 0) { 
				close( curId ); 
				client_socket[i] = 0; 
				users.erase(curId);
				return true;
			} 
				
			else{ 
				
				buffer[valread] = '\0'; 
				string mes(buffer);
				reqHandler->tokenizer(mes);
				return false;
				
			} 
		} 
	} 
}

void Server::createDataSocket() {
	int opt = 1; 
	serverDataSocket = socket(AF_INET , SOCK_STREAM , 0);
	setsockopt(serverDataSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
	
	dataAddress.sin_family = AF_INET; 
	dataAddress.sin_addr.s_addr = INADDR_ANY; 
	dataAddress.sin_port = htons(dataPort); 

	bind(serverDataSocket, (struct sockaddr *)&dataAddress, sizeof(dataAddress));
	listen(serverDataSocket, 5);
	cout<<"waiting for conections to data socket"<<endl;
}

int Server::dataTransfer(int max_sd) {
	FD_ZERO(&readDatafds); 
	FD_SET(serverDataSocket, &readDatafds); 
	max_sd = serverDataSocket; 

	max_sd = addToDatafdSet(max_sd);
	
	select( max_sd + 1 , &readDatafds , NULL , NULL , NULL); 

	if (FD_ISSET(serverDataSocket, &readDatafds)) 
		newDataConnection();

	else
		dataSocketIO();
	return max_sd;
}

int Server::addToDatafdSet(int max_sd) {
	int sd;
	for (int i = 0 ; i < max_clients ; i++) { 
			
		sd = data_socket[i]; 
		if(sd > 0) 
			FD_SET( sd , &readDatafds); 
		if(sd > max_sd) 
			max_sd = sd; 
	} 

	return max_sd;
}

void Server::newDataConnection() {
	int addrlen = sizeof(dataAddress);
	int new_socket = accept(serverDataSocket, (struct sockaddr *)&dataAddress, (socklen_t*)&addrlen);
	
	for (int i = 0; i < max_clients; i++) { 
		
		if( data_socket[i] == 0 ) { 
			data_socket[i] = new_socket; 
			ports.insert({channel, data_socket[i]});
			cout<<"new client connected to data socket"<<endl;
			break; 
		} 
	} 
}

void Server::dataSocketIO() {
	char buffer[MSGSIZE + 1];
	int valread;
	for (int i = 0; i < max_clients; i++) { 
		curData = data_socket[i]; 
			
		if (FD_ISSET( curData , &readDatafds)) { 
			
			if ((valread = read( curData , buffer, MSGSIZE)) == 0) { 
				close( curData ); 
				ports.erase(curId);
				
			} 
		} 
	} 
}


void Server::runDatabase() {
	db.start();
}

void Server::user(string name) {
	if (alreadyLogged()) {
		error();
		return;
	}
	bool exist = db.findUser(name);
	
	if (exist) {
		users.insert({curId, new User(name, curDir)});
		send(curId, "331: User name okay, need password.", MSGSIZE, 0);
	}
	else
		send(curId, "430: Invalid username or password", MSGSIZE, 0);
}

void Server::pass(string password) {
	if (alreadyLogged()) {
		error();
		return;
	}
	if(users.find(curId) == users.end())
		send(curId, "503: Bad sequence of commands.", MSGSIZE, 0);
	else{
		string actualPass = db.getPassword(users[curId]->getName());
		if(actualPass.compare(password) == 0){
			users[curId]->setLogged(true);
			printLog("logged in");
			send(curId, "230: User logged in, proceed. Logged out if appropriate.", MSGSIZE, 0);
		}
		else
			send(curId, "430: Invalid username or password", MSGSIZE, 0);

	}
}

bool Server::alreadyLogged() {
	if (users.find(curId) != users.end()) 
		if (users[curId]->getLogged()) 
			return true;
	return false;
}

bool Server::checkLog() {
	if (users.find(curId) != users.end()) 
		if (users[curId]->getLogged()) 
			return true;
	needAccount();
	return false;
}

void Server::needAccount() {
	send(curId, "332: Need account for login.", MSGSIZE, 0);
}

void Server::badReq() {
	send(curId, "501: Syntax error in parameters or arguments.", MSGSIZE, 0);
}

void Server::pwd() {
   string dir = users[curId]->getDir();
   string msg = "257: " + dir;
   send(curId, msg.c_str(), MSGSIZE, 0);

}

void Server::mkd(string path) {
    int i = mkdir(path.c_str(), 0777);
    string msg;
    if (i == 0) {
    	msg = "257: " + path + " created.";
    	printLog("created new directory in " + path);
    }
    else 
    	msg = "500: Error";
    send(curId, msg.c_str(), MSGSIZE, 0);
}

void Server::ls() {
	string path = users[curId]->getDir();
	DIR* dirp = opendir(path.c_str());
    struct dirent * child;
    string msg;
    while ((child = readdir(dirp)) != NULL) {
        string s(child->d_name);
        if(s != "." && s!= ".."){
            msg += s + "\n";
        }
    }
    closedir(dirp);
    send(curId, "success", MSGSIZE, 0);
    send(ports[curId], msg.c_str(), 3000, 0);
    send(curId, "226: List transfer done.", MSGSIZE, 0);
}

void Server::cwd(string path) {
	bool success = false;
	if (path.compare("") == 0) {
		users[curId]-> setDir(curDir);
		success = true;
	}

	else if (path.compare("..") == 0) {
		string dir = users[curId] -> getDir();
		if (dir.compare(curDir) != 0) {
			size_t found = dir.find_last_of("/\\");
       	    users[curId]-> setDir(dir.substr(0,found));
       	    success = true;
		}
	}

	else {
		struct stat buffer;
		path = curDir + "/" + path;
	    if (stat (path.c_str(), &buffer) == 0) {
	    	users[curId]-> setDir(path);
	    	success = true;
	    }
	}

	if (success) 
		send(curId, "250: Successful change.", MSGSIZE, 0);
	else
		error();
}

void Server::deled(string path) {
	string dir = curDir + "/" + path;
	if (rmdir(dir.c_str()) == 0) {
		string msg = "250: " + path + " deleted.";
		printLog("deleted directory " + path);
		send(curId, msg.c_str(), MSGSIZE, 0);
	}
	else
		error();
}

bool Server::hasAccess(string filename) {
	string dir = users[curId]-> getDir();
	if (dir.compare(curDir) == 0) {
		if(db.restrictedFile(filename) && ! db.isAdmin(users[curId]-> getName())) {
			fileAccessError();
			return false;
		}
	}
	return true;
}

void Server::delef(string filename) {
	if (hasAccess(filename))
		removeFile(filename);
}


void Server::removeFile(string filename) {
	string dir = users[curId]->getDir() + "/" + filename;
	string msg = "250: "  + filename + " deleted.";
	if (remove(dir.c_str()) == 0) {
		printLog("deleted file " + dir);
		send(curId, msg.c_str(), MSGSIZE, 0);
	}
	else
		error();
}

void Server::changeName(string filename, string newname) {
	if(!hasAccess(filename))
		return;
	string dir = users[curId]-> getDir();
	if(rename((dir + "/" + filename).c_str(), (dir + "/" + newname).c_str()) == 0) {
		printLog("renamed file " + dir + "/" + filename + "to " + dir + "/" + newname);
		send(curId, "250: Successful change.", MSGSIZE, 0);
	}
	else
		error();

} 

void Server::retr(string filename) {
	if (hasAccess(filename))
		downloadFile(filename);
} 

void Server::downloadFile(string filename) {
	string dir = users[curId]->getDir() + "/" + filename;
	FILE* file_;
	char buffer[10000];
	file_ = fopen(dir.c_str(), "r");
	if (file_ == NULL) {
		error();
		return;
	}
	if (checkTraffic(file_)) {
		printLog("downloaded file " + dir);
		char buffer[10000];
		fread(buffer, sizeof(buffer), 1, file_);
		send(curId, "success", MSGSIZE, 0);
		send(ports[curId], buffer, 10000, 0);
		send(curId, "226: Successful Download.", MSGSIZE, 0);
	}
	else 
		send(curId, "425: Can't open data connection.", MSGSIZE, 0);
	fclose(file_);
}

bool Server::checkTraffic(FILE* file_) {
	int remainedSize = 	db.getUserTraffic(users[curId]->getName());
	fseek(file_, 0L, SEEK_END);
	int sz = ftell(file_);
	fseek(file_, 0L, SEEK_SET);
	if(remainedSize >= sz) {
		db.setUserTraffic(users[curId]->getName(), sz);
		return true;
	}
	else
		return false;
}

void Server::help() {
	string msg;
	msg += "214\n";
	msg += "USER [name], Its argument is used to specify the user’s string. It is used for user authentication.\n";
	msg += "PASS [password], Its argument is used to specify the user's password. It is used for user login.\n";
	msg += "PWD, It is used to show current directory.\n";
	msg += "MKD [directory path], Its argument is used to specify the directory path. It is used for creating a new directory in the path specified.\n";
	msg += "DELE -f [filename], It is used to delete a file named filename.\n";
	msg += "DELE -d [directory path], It is used to delete a directory in directory path.\n";
	msg += "LS, It is used to show list of files.\n";
	msg += "CWD [path], It is used to change directory to the path. If no argument is entered the directory changes to the first directory and if the argument equals to .. it goes to previous directory.\n";
	msg += "RENAME [from] [to], Its first arguments indicates file name. Its second arguments shows the new name for the file. It is used to rename a file.\n";
	msg += "RETR [name], It is used to download a file that its name equals to first argument.\n";
	msg += "HELP, It is uesd to show list of commands and their explanation.";
	msg += "QUIT, It is used for user logout.\n";
	send(curId, msg.c_str(), MSGSIZE, 0);


}


void Server::quit() {
	printLog("logged out");
	users.erase(curId);
	send(curId, "221: Successful Quit.", MSGSIZE, 0);
}


void Server::error() {
	send(curId, "500: Error", MSGSIZE, 0);
}


void Server::fileAccessError() {
	send(curId, "550: File unavailable.", MSGSIZE, 0);
}


void Server::printLog(string msg) {
	logFile = fopen("log.txt", "a+");
	fprintf(logFile, "%s : %s %s\n", ctime(&now), users[curId]->getName().c_str(), msg.c_str());
	fclose(logFile);
	
}
