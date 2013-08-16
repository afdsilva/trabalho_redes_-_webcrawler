/*
 * ssl.cpp
 *
 *  Created on: 16/08/2013
 *      Author: andref
 */


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

#define SERVER  "www.www.openssl.org"
#define PORT 443

class SslConnection {
private:
	int socket;
	SSL * sslHnd;
	SSL_CTX * sslCtx;

	bool isValid;
public:
	SslConnection();
	virtual ~SslConnection();

	void SslSend(const char * text);
	string SslReceive();

};

int tcpConnect ();

int main() {

	//criando conexao
	string resposta;
	SslConnection * sslTest = new SslConnection();
	sslTest->SslSend("GET / \r\n\r\n");
	resposta = sslTest->SslReceive();

	std::cout << resposta << std::endl;
	delete(sslTest);

	return 0;
}

SslConnection::SslConnection() {
	std::cout << "Criando Conexao Segura" << std::endl;
	this->sslHnd = NULL;
	this->sslCtx = NULL;

	this->socket = tcpConnect();

	if (this->socket) {
		SSL_load_error_strings();

		SSL_library_init();

		this->sslCtx = SSL_CTX_new (SSLv23_client_method());
		if (this->sslCtx == NULL)
			ERR_print_errors_fp(stderr);

		this->sslHnd = SSL_new (this->sslCtx);
		if (this->sslHnd == NULL)
			ERR_print_errors_fp(stderr);

		if (!SSL_set_fd(this->sslHnd, this->socket))
			ERR_print_errors_fp(stderr);

		if (SSL_connect(this->sslHnd) != 1)
			ERR_print_errors_fp(stderr);

	} else {
		std::cout << "Conexao falhou" << std::endl;
	}
}
SslConnection::~SslConnection() {
	std::cout << "Destruindo Conexao Segura" << std::endl;
	if (this->socket)
		close(this->socket);
	if (this->sslHnd) {
		SSL_shutdown(this->sslHnd);
		SSL_free(this->sslHnd);
	}
	if (this->sslCtx)
		SSL_CTX_free(this->sslCtx);

}

void SslConnection::SslSend(const char * text) {
	std::cout << "Enviando Requisição " << std::endl;
	if (this->sslHnd != NULL)
		SSL_write(this->sslHnd, text, strlen(text));
}

string SslConnection::SslReceive() {
	std::cout << "Recebendo resposta" << std::endl;
	string retorno;
	const int size = 1024;
	int received;
	char buffer[size];

	if (this->sslHnd != NULL) {
		while(true) {
			received = SSL_read(this->sslHnd, buffer, size);
			buffer[received] = '\0';
			if (received > 0)
				retorno += buffer;

			if (received < size)
				break;
		}
	}

	return retorno;
}

int tcpConnect () {
  int error, handle;
  struct hostent *host;
  struct sockaddr_in server;

  host = gethostbyname (SERVER);
  handle = socket (AF_INET, SOCK_STREAM, 0);
  if (handle == -1)
    {
      perror ("Socket");
      handle = 0;
    }
  else
    {
      server.sin_family = AF_INET;
      server.sin_port = htons (PORT);
      server.sin_addr = *((struct in_addr *) host->h_addr);
      bzero (&(server.sin_zero), 8);

      error = connect (handle, (struct sockaddr *) &server,
                       sizeof (struct sockaddr));
      if (error == -1)
        {
          perror ("Connect");
          handle = 0;
        }
    }

  return handle;
}
