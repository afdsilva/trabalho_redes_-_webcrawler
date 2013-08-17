/*
 * ssl.cpp
 *
 *  Created on: 16/08/2013
 *      Author: andref
 */

#include "ssl.h"

int tcpConnect (const char * url, int port);

int main() {
	//criando conexao
	string resposta;
	SslConnection * sslTest = new SslConnection();
	sslTest->LocalCertificate("","/usr/lib/ssl/certs");
	sslTest->Connect("www.google.com",443);
	sslTest->SslSend("GET /?gws_rd=cr \r\n\r\n");
	resposta = sslTest->SslReceive();

	std::cout << resposta << std::endl;
	delete(sslTest);

	return 0;
}

SslConnection::SslConnection() {
	this->sslHnd = NULL;
	this->sslCtx = NULL;
	this->socket = 0;
}
SslConnection::~SslConnection() {
	if (this->socket)
		close(this->socket);
	if (this->sslHnd) {
		SSL_shutdown(this->sslHnd);
		SSL_free(this->sslHnd);
	}
	if (this->sslCtx)
		SSL_CTX_free(this->sslCtx);

}
bool SslConnection::LocalCertificate(string file, string path) {
	bool retorno = true;
	try {
		this->fileCertificate = file;
		this->pathCertificate = path;
	} catch (int e) {

	}
	return retorno;
}
bool SslConnection::Connect(const char * url, int port) {
	bool retorno = true;
	string erro;
	try {
		this->socket = tcpConnect(url, port);

		if (this->socket) {
			SSL_load_error_strings();

			SSL_library_init();

			this->sslCtx = SSL_CTX_new (SSLv23_client_method());
			if (this->sslCtx == NULL)
				throw 1;
			if(!SSL_CTX_load_verify_locations(this->sslCtx, (this->fileCertificate.empty() ? NULL : this->fileCertificate.c_str()), (this->pathCertificate.empty() ? NULL : this->pathCertificate.c_str())))
				std::cerr << "Erro no caminho dos certificados locais" << std::endl;

			this->sslHnd = SSL_new (this->sslCtx);
			if (this->sslHnd == NULL)
				throw 3;

			if (!SSL_set_fd(this->sslHnd, this->socket))
				throw 4;

			if (SSL_connect(this->sslHnd) != 1)
				throw 5;

			if(SSL_get_verify_result(this->sslHnd) != X509_V_OK) {
				std::cerr << "Certificado inválido" << std::endl;
			}
		} else
			throw 0;
	} catch (int e) {
		switch (e) {
		case 1:
			erro = "Não foi possivel criar contexto";
			break;
		case 3:
			erro = "Não foi possivel criar o controlador da conexao";
			break;
		case 4:
			erro = "Não foi possivel conectar a estrutura SSL a conexão";
			break;
		case 5:
			erro = "Não foi possivel iniciar handshake";
			break;
		default:
			erro = "Conexão falhou";
			break;
		}
		std::cerr << erro << std::endl;
		retorno = false;
	}
	return retorno;
}
bool SslConnection::SslSend(const char * text) {
	bool retorno = true;
	try {
		if (this->sslHnd != NULL)
			SSL_write(this->sslHnd, text, strlen(text));
		else
			throw 0;
	} catch (int e) {
		retorno = false;
	}
	return retorno;

}

string SslConnection::SslReceive() {
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

int tcpConnect (const char * url, int port) {
  int error, handle;
  struct hostent *host;
  struct sockaddr_in server;

  host = gethostbyname (url);
  handle = socket (AF_INET, SOCK_STREAM, 0);
  if (handle == -1)
    {
      perror ("Socket");
      handle = 0;
    }
  else
    {
      server.sin_family = AF_INET;
      server.sin_port = htons (port);
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
