/*
 * http.h
 *
 *  Created on: 04/07/2013
 *      Author: andref
 */

#ifndef HTTP_H_
#define HTTP_H_

#include <iostream>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>

class http {
private:
	int socket_desc;
	string message;
	string reply;
	struct sockaddr_in server;

public:
	http();
	virtual ~http();

public:
	bool Server(string url, int port );

};

#endif /* HTTP_H_ */
