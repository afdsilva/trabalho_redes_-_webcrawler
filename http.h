/*
 * http.h
 *
 *  Created on: 04/07/2013
 *      Author: andref
 */

#ifndef HTTP_H_
#define HTTP_H_

#include <iostream>
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
	int socket_desc;
	char * http_query;
	string reply;
	struct sockaddr_in server;

public:
	http();
	virtual ~http();

public:
	bool Server(string url, int port );

	char * BuildQuery(char * host, char * page);
	string ReceiveData();




};

#endif /* HTTP_H_ */
