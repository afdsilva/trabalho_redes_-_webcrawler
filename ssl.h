/*
 * ssl.h
 *
 */

#ifndef SSL_H_
#define SSL_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#include <boost/regex.hpp>  // BIBLIOTECA PARA PARSEAR O HTML
#include <boost/algorithm/string/regex.hpp>

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

	string issuerCertificate;
	string subjectCertificate;

public:
	SslConnection();
	virtual ~SslConnection();

	bool LocalCertificate(string file, string path);
	bool Connect(int socket);

	bool SslSend(const char * text);
	string SslReceive();

	string GetCertificateIssuer(string substring);
	string GetCertificateSubject(string substring);
};

#endif /* SSL_H_ */
