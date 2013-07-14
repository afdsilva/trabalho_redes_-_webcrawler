#ifndef HTTP_H_
#define HTTP_H_

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <fstream>

using namespace std;

class http {
private:
	static std::vector<string> urlVisited;
	static std::vector<string> urlUnVisited;
	int socket_desc;
	char * http_query;
	char reply[BUFSIZ];
	struct sockaddr_in server;
	struct hostent * host;

public:
	http();
	http(string url, int port);
	virtual ~http();

public:
	bool Server(string url, int port);
private:
	struct hostent * ParseUrl(string url);
	char * BuildQuery(char * host, char * page);
	string ReceiveData();
	bool SendRequest();

};

#endif /* HTTP_H_ */
