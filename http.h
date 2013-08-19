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
#include "ssl.h"

#define PORT 80		// PORTA PADRAO HTTP

using namespace std;
class UrlNodo {
public:
	string url;
	string CN;
	string O;
	bool autoassinado;
	bool useSsl;

	UrlNodo();
	bool operator==(const UrlNodo & a);
};
class http : public Thread {
private:
	static std::vector<Thread> threads;

	string url;
	int port;

	int depth;
	string domain;
	string path;
	string file;
	string filename;

	int socket_desc;

	SslConnection * conexaoSegura;

	bool secure;
	bool valid;

	char * http_query;
	std::vector<string> urlList;

	Lock * _lck;

public:
	http(string url);
	void run();

	virtual ~http();

	static std::vector<UrlNodo *> urlVisited;

	static void Recursive(string url, int depth);

public:
	int Connect();
	std::vector<string> GetUrlList();

	string GetCertificateOwner();
	string GetCertificateOwnerOrganization();
	bool IsSelfSign();
	bool Secure();
	bool IsValid();
	void static LocalCertificates(string file, string path);
	static string certificateFile;
	static string certificatePath;


private:
	int ParseUrl();
	int CreateDir();
	int BuildQuery();
	string HtmlReceive();
	int SendRequest();
	int ParseData(string reply);

};

#endif /* HTTP_H_ */
