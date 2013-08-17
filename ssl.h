/*
 * ssl.h
 *
 *  Created on: 17/08/2013
 *      Author: andref
 */

#ifndef SSL_H_
#define SSL_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

class SslConnection {
private:
	int socket;
	SSL * sslHnd;
	SSL_CTX * sslCtx;

	string pathCertificate;
	string fileCertificate;

public:
	SslConnection();
	virtual ~SslConnection();

	bool LocalCertificate(string file, string path);
	bool Connect(const char * server, int port);

	bool SslSend(const char * text);
	string SslReceive();

};



#endif /* SSL_H_ */
