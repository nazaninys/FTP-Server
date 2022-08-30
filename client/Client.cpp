#include "Client.hpp"

void Client::start() {
    getPorts();
    connect_to_server();
    connect_to_data_port();
    communicate();
}

void Client::getPorts() {
    ifstream data("../server/config.json", ifstream::binary);
    reader.parse(data, root, false);
    Json::Value p1 = root["commandChannelPort"];
    commandPort = p1.asInt();
    Json::Value p2 = root["dataChannelPort"];
    dataPort = p2.asInt();
}

void Client::connect_to_server(){
	if ((sockid = socket(AF_INET, SOCK_STREAM, 0)) < 0){ 
        write(1,"Socket creation error\n", 23); 
        return;
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(commandPort); 
       

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0){ 
        write(1,"Invalid address/ Address not supported\n", 40); 
        return; 
    } 
   
    if (connect(sockid, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) { 
        write(1,"Connection Failed\n", 19); 
        return; 
    } 
    
}

void Client::connect_to_data_port() {
    if ((dataId = socket(AF_INET, SOCK_STREAM, 0)) < 0){ 
        write(1,"Socket creation error\n", 23); 
        return;
    } 
   
    data_addr.sin_family = AF_INET; 
    data_addr.sin_port = htons(dataPort); 
       

    if(inet_pton(AF_INET, "127.0.0.1", &data_addr.sin_addr)<=0){ 
        write(1,"Invalid address/ Address not supported\n", 40); 
        return; 
    } 
   
    if (connect(dataId, (struct sockaddr *)&data_addr, sizeof(data_addr)) < 0){ 
        write(1,"Connection Failed\n", 19); 
        return; 
    } 

}

void Client::communicate() {
    string request;
    char response[SIZE];
    while(true) {
        
        getline(cin, request);
        send(sockid, request.c_str(), SIZE, 0);
        
        if (request.substr(0, request.find(' ')).compare("ls") == 0) {
            ls();
            continue;
        }
        if (request.substr(0, request.find(' ')).compare("retr") == 0) {
            retr(request);
            continue;
        }
        memset(response, 0, 255);
        read(sockid, response, SIZE);
        string res(response);
        cout<<res<<endl;
    }
    
}

void Client::ls() {
    char list[3000];
    char success[SIZE];
    read(sockid, success, SIZE);
    string res(success);

    if(res.compare("success") == 0) {
        char response[SIZE];
        read(dataId, list, 3000);
        string files(list);
        read(sockid, response, SIZE);
        string res(response);
        cout<<res<<endl;
        cout<<files;
    }
    else 
        cout<<res<<endl;
}

void Client::retr(string req) {
    char list[30000];
    char msg[SIZE];
    read(sockid, msg, SIZE);
    string res(msg);
    if(res.compare("success") == 0) {
        char list[10000];
        read(dataId, list, 10000);
        string filename = req.substr(req.find(' ') + 1, req.size());
        FILE* newfile;
        newfile = fopen(filename.c_str(), "w");
        fwrite(list, sizeof(list), 1, newfile);
        fclose(newfile);
        char response[SIZE];
        read(sockid, response, SIZE);
        string res(response);
        cout<<res<<endl;
    }
    else
        cout<<res<<endl;
}

