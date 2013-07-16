#ifndef HTTP_H_
#define HTTP_H_

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <fstream>
#include <boost/regex.hpp>  // BIBLIOTECA PARA PARSEAR O HTML
#include <boost/algorithm/string/regex.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/time.h>

#include "Thread.h"

#define PORT 80		// PORTA PADRAO HTTP

using namespace std;

class http : public Thread {
private:
	static std::vector<Thread> threads;

	string url;
	int depth;
	string domain;
	string path;
	string file;
	string filename;

	int socket_desc;

	char * http_query;
	struct sockaddr_in server;
	struct hostent * host;

	Lock * _lck;

public:
	http(string url,int depth);
	void run();

	virtual ~http();

	static std::vector<string> urlVisited;
	static std::vector<string> urlUnVisited;

public:
	int Server();
	std::vector<string> GetUrlList();
private:
	int ParseUrl();
	int CreateDir();
	int BuildQuery();
	int SendRequest();
	int ReceiveData();

};

#endif /* HTTP_H_ */
