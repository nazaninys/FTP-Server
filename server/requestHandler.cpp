#include "requestHandler.hpp"
#include "server.hpp"


RequestHandler::RequestHandler(Server* ser) {
	this->server = ser;
}

void RequestHandler::tokenizer(string request) {

	tokens.clear();
	istringstream req(request);
  
    string word;
    while (req >> word) 
       tokens.push_back(word);

    getCommand();
}


void RequestHandler::getCommand() {
	if(! reqExists()) {
			server->badReq();
			return;
	}
	if(tokens[0].compare("user") == 0)
		server->user(tokens[1]);
	else if(tokens[0].compare("pass") == 0)
		server->pass(tokens[1]);
	else {
		if (! checkLog()) 
			return;

		if (tokens[0].compare("pwd") == 0)
			server->pwd();

		else if (tokens[0].compare("mkd") == 0)
			server->mkd(tokens[1]);

		else if (tokens[0].compare("ls") == 0)
			server->ls();

		else if (tokens[0].compare("cwd") == 0) {
			if (tokens.size() == 1)
				server->cwd("");
			else 
				server->cwd(tokens[1]);
		}

		else if (tokens[0].compare("dele") == 0) {
			if (tokens[1].compare("-d") == 0)
				server->deled(tokens[2]);
			else if (tokens[1].compare("-f") == 0) {
				if (tokens[2].find('/') == string::npos)
					server->delef(tokens[2]);
				else
					server->badReq();
			}
			else 
				server->badReq();
		}

		else if(tokens[0].compare("rename") == 0)
			server->changeName(tokens[1], tokens[2]);

		else if(tokens[0].compare("retr") == 0)
			server->retr(tokens[1]);
		
		else if (tokens[0].compare("help") == 0)
			server->help();

		else if(tokens[0].compare("quit") == 0)
			server->quit();
	}
}


bool RequestHandler::reqExists() {
	if (tokens.size() == 0)
		return false;
	int size = tokens.size();
	if ((tokens[0].compare("user") == 0 && size == 2) ||
		(tokens[0].compare("pass") == 0 && size == 2) ||
		(tokens[0].compare("pwd") == 0 && size == 1) || 
		(tokens[0].compare("mkd") == 0 && size == 2) || 
		(tokens[0].compare("dele") == 0 && size == 3) || 
		(tokens[0].compare("ls") == 0 && size == 1) || 
		(tokens[0].compare("cwd") == 0 && size <= 2) ||
		(tokens[0].compare("rename") == 0 && size == 3) || 
		(tokens[0].compare("retr") == 0 && size == 2) || 
		(tokens[0].compare("help") == 0 && size == 1) ||
		(tokens[0].compare("quit") == 0 && size == 1))
		return true;
	else
		return false;
}


bool RequestHandler::checkLog() {
	if (server-> checkLog())
		return true;
	else 
		return false;
}