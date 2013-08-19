/*
 * ssl.cpp
 *
 */

#include "ssl.h"

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
bool SslConnection::Connect(int socket) {
	bool retorno = true;
	string erro;
	try {
		this->socket = socket;

		if (this->socket) {
			SSL_load_error_strings();

			SSL_library_init();

			this->sslCtx = SSL_CTX_new (SSLv23_client_method());
			if (this->sslCtx == NULL)
				throw 1;
			if (this->fileCertificate.empty() && this->pathCertificate.empty()) {
				//sem certificado local
			} else {
				if(!SSL_CTX_load_verify_locations(this->sslCtx, (this->fileCertificate.empty() ? NULL : this->fileCertificate.c_str()), (this->pathCertificate.empty() ? NULL : this->pathCertificate.c_str())))
					std::cerr << "Erro no caminho dos certificados locais" << std::endl;
			}
			this->sslHnd = SSL_new (this->sslCtx);
			if (this->sslHnd == NULL)
				throw 3;

			if (!SSL_set_fd(this->sslHnd, this->socket))
				throw 4;

			if (SSL_connect(this->sslHnd) != 1)
				throw 5;

			if (this->fileCertificate.empty() && this->pathCertificate.empty()) {
				//sem certificado
			} else {
				if(SSL_get_verify_result(this->sslHnd) != X509_V_OK) {
					std::cerr << "Certificado inválido" << std::endl;
				} else {
					X509 * certs = SSL_get_peer_certificate(this->sslHnd);
					this->issuerCertificate = X509_NAME_oneline(X509_get_issuer_name(certs), 0, 0);
					free(certs);
				}
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

	if (this->sslHnd != NULL) {

		int tam = 0;
		char buffer[BUFSIZ];
		int header = 0;
		char * content = NULL;

		struct timeval tv; /* timeval and timeout stuff added by davekw7x */
		tv.tv_sec = 3;
		tv.tv_usec = 0;
		if (setsockopt(this->socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,  sizeof tv)){
			//perror("setsockopt");
			throw 1;
		}
		while ((tam = SSL_read(this->sslHnd, buffer, BUFSIZ)) > 0) {
			retorno += buffer;
			if (header == 0) {
				content = strstr(buffer,"\r\n\r\n");
				if (content != NULL) {
					header = 1;
					content +=4;
					//ponteiro = content - server_reply;
				}
			} else {
				//ponteiro = 0;
				content = buffer;
				//puts(content);
			}
			//if (tam < 1024)
			//	break;

			//saida.write(content,tam-ponteiro);

			memset(buffer,0,tam);
		}
	}
	return retorno;
}

string SslConnection::GetCertificateSubString(string substring) {
	string retorno;
	if (!this->issuerCertificate.empty()) {
		vector<string> str_split;

		boost::split_regex(str_split, this->issuerCertificate,boost::regex("/"));
		std::size_t pos;

		for (unsigned int i = 0; i < str_split.size(); i++) {
			if ((pos = str_split[i].find(substring)) != std::string::npos) {
				retorno = str_split[i].substr(substring.length()+1);
				break;
			}
		}
	}
	return retorno;
}
