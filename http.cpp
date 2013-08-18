#include "http.h"

using namespace std;

std::vector<UrlNodo *> http::urlVisited;
string http::certificateFile;
string http::certificatePath;

http::http(string url) {
	this->url = url;
	this->port = PORT;
	this->secure = false;
	this->valid = true;

	this->conexaoSegura = NULL;
	_lck = new Lock();

}
http::~http() {
	delete _lck;

}
void http::run() {
	try {
		if (this->ParseUrl() == -1) throw 1;
		if (this->BuildQuery() == -1 ) throw 2;
		if (!this->Connect()) throw 3;
		string resposta;
		if (secure) {
			this->conexaoSegura = new SslConnection();
			conexaoSegura->LocalCertificate(http::certificateFile,http::certificatePath);
			if (!conexaoSegura->Connect(this->socket_desc))
				throw 4;
			conexaoSegura->SslSend(this->http_query);
			resposta = conexaoSegura->SslReceive();

		} else {
			//std::cout << "Conexao nao segura" << std::endl;
			if (this->SendRequest() == -1) throw 5;
			resposta = this->HtmlReceive();
		}
		this->ParseData(resposta);

	} catch (int e) {
		string erro;
		switch (e) {
		case 1:
			erro = "ParseURL()";
			break;
		case 2:
			erro = "BuildQuery()";
			break;
		case 3:
			erro = "Connect()";
			break;
		case 4:
			erro = "conexaoSegura->Connect(";
			erro += this->socket_desc;
			erro += ")";
			break;
		case 5:
			erro = "SendRequest()";
			break;
		}
		std::cerr << "ERRO: " << erro << std::endl;
		this->socket_desc = 0;
		this->valid = false;
	}
}
/**
 * Faz a conexão TCP
 * Retorna o socket
 */
int http::Connect() {
	try {
		this->socket_desc = socket(AF_INET, SOCK_STREAM, 0);

		if (this->socket_desc == -1) {
			throw 1;
		} else {

			struct sockaddr_in * server = new sockaddr_in;
			struct hostent * host;
			if (this->domain.empty())
				throw 2;
			host = gethostbyname(this->domain.c_str());
			if (host == NULL)
				throw 2;
			server->sin_family = AF_INET;
		    server->sin_port = htons(this->port);
			char * host_ip = inet_ntoa(*(struct in_addr *)host->h_addr_list[0]);
		    server->sin_addr.s_addr = inet_addr(host_ip);
		    for (int i = 0; i < 8; i++)
		    	server->sin_zero[i] = '\0';

			if (connect (this->socket_desc, (struct sockaddr *) server, sizeof (struct sockaddr)))
				throw 3;
		}
	} catch (int e) {
		switch (e) {
		case 1:
			std::cerr << "Nao foi possivel criar socket" << std::endl;
			break;
		case 2:
			std::cerr << "Hostname invalido: " << this->url.c_str() << std::endl;
			break;
		case 3:
			std::cerr << "Nao foi possivel conectar" << std::endl;
			break;
		}
		this->socket_desc = 0;
	}

	return this->socket_desc;
}

std::vector<string> http::GetUrlList() {
	return this->urlList;
}

int http::ParseUrl() {
	int retorno = 0;
	try {
		vector<string> str_split;

		boost::split_regex(str_split, url,boost::regex("/"));
		this->path = "/";
		this->file = str_split[str_split.size()-1] == "" ? "index.html" : str_split[str_split.size()-1];

		std::size_t found = str_split[0].find("http");
		if (found != std::string::npos) {
			if (str_split[0].find("https") != std::string::npos) {
				this->secure = true;
				this->port = 443;
			}
			this->domain = str_split[2];
			for (int i = 3; i < (int)str_split.size();i++) {
				this->path += str_split[i];
				if (i < (int)str_split.size() - 1)
					this->path += "/";
			}
		} else {
			this->domain = str_split[0];
		}
	} catch (int e) {
		retorno = -1;
	}
	return retorno;
}
int http::BuildQuery() {
	int retorno = 0;
	try {
	    char request_temp[] = "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n";

	    this->http_query = (char *) malloc(strlen(this->domain.c_str()) + strlen(this->path.c_str()) + strlen(request_temp));
	    sprintf(this->http_query, request_temp, this->path.c_str(), this->domain.c_str());
	} catch (...) {
		retorno = -1;
	}
	return retorno;
}
int http::SendRequest() {
	int retorno = 0;
	try {
		if(send(this->socket_desc, this->http_query, strlen(this->http_query), 0) < 0)
			throw 1;

	} catch (int e) {
		std::cout << "Falha no envio da requisição:";
		std::cout << " socket_desc: " << this->socket_desc;
		std::cout << " http_query: " << this->http_query << std::endl;
		retorno = -1;
	}
	return retorno;
}
int http::CreateDir() {
	int retorno = 0;
	try {

	} catch (int e) {
		retorno = -1;
	}
	return retorno;
}
string http::HtmlReceive() {
	string resposta;
	resposta.clear();
	try {
		char server_reply[BUFSIZ];

		//std::ofstream saida(filename.c_str(), ios::out);
		//unsigned int ponteiro = 0;

		int tam = 0;
		memset(server_reply,0,sizeof(server_reply));
		int header = 0;
		char * content = NULL;


		struct timeval tv; /* timeval and timeout stuff added by davekw7x */
		int timeouts = 0;
		tv.tv_sec = 3;
		tv.tv_usec = 0;
		if (setsockopt(this->socket_desc, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,  sizeof tv)){
			//perror("setsockopt");
			throw 1;
		}
		while((tam = recv(this->socket_desc, server_reply ,BUFSIZ, 0)) > 0  && (++timeouts < 10000)) {
			resposta += server_reply;
			//std::cout << "Tamanho: " << tam << std::endl;
			if (header == 0) {
				content = strstr(server_reply,"\r\n\r\n");
				if (content != NULL) {
					header = 1;
					content +=4;
					//ponteiro = content - server_reply;
				}
			} else {
				//ponteiro = 0;
				content = server_reply;
				//puts(content);
			}

			//saida.write(content,tam-ponteiro);

			memset(server_reply,0,tam);

		}

		//saida.close();
	} catch (int e) {
		resposta.clear();
	}
	return resposta;
}
int http::ParseData(string reply) {
	int retorno = 0;
	try {
		size_t pos = reply.find("\r\n\r\n");
		string headerStr( reply.begin(), reply.begin()+pos );
		std::size_t found = headerStr.find("text/html");
		if (found != std::string::npos) {

			//boost::regex e("<\\s*A\\s+[^>]*href\\s*=\\s*\"([^\"]*)\"|<img.+?src=[\"'](.+?)[\"'].+?>",
			boost::regex e("<\\s*A\\s+[^>]*href\\s*=\\s*\"([^\"]*)\"",
					   boost::regbase::normal | boost::regbase::icase);

			// lista de string acumulando os links parcialmente
			// depois colocar nos atributos da classe de urlVisited...
			list<string> l;

			//std::cerr << "URL's encontradas " << ":" << endl;
			regex_split(back_inserter(l), reply, e);
			string temp;
			std::vector<string> aux;
			boost::regex invalid("mailto:|javascript:");
			boost::regex absoluto("http|https");
			boost::regex arquivo(".*");
			_lck->lock();
			string nPath = this->path;
			for (std::list<string>::iterator it = l.begin(); it != l.end(); it++ ) {
				temp.clear();
				string novaUrl = *it;
				if(!(boost::regex_search(novaUrl, invalid))){
					if (!novaUrl.empty()) {
						if(!(boost::regex_search(novaUrl, absoluto))){
							int nr = 0;
							if (novaUrl.at(0) == '.') {
								nr = 2;
								if (novaUrl.at(1) == '.') {
									nr = 3;
								}
							} else if (novaUrl.at(0) == '/')
								nr = 1;
							if (nr == 3) {
								aux.clear();
								boost::split_regex(aux, this->path, boost::regex("/"));
								nPath.clear();
								for(int i = 0; i < (int)aux.size()-2; i++){
									nPath += aux[i];
									if(i < (int)aux.size()-2){
										nPath += "/";
									}
								}
							}
							novaUrl.replace(novaUrl.begin(), novaUrl.begin()+nr,"");
							temp += (this->secure ? "https://" : "http://");
							temp += this->domain;
							if (boost::regex_search(this->path, arquivo)) {
								aux.clear();
								boost::split_regex(aux, this->path, boost::regex("/"));
								for(int i = 0; i < (int)aux.size()-1; i++){
									temp += aux[i];
									if(i < (int)aux.size()-1){
										temp += "/";
									}
								}
							}
							else{
								temp += nPath;
							}
							novaUrl = temp + novaUrl;
						}
						urlList.push_back(novaUrl);
					}
				}
			}
			_lck->unlock();

		}

	} catch (int e) {
		retorno = -1;
	}
	return retorno;
}

string http::GetCertificateOwner() {
	string retorno;
	if (secure && this->conexaoSegura != NULL)
		retorno = this->conexaoSegura->GetCertificateSubString("CN");
	return retorno;
}
bool http::IsSelfSign() {
	bool retorno = false;
	if (secure && this->conexaoSegura != NULL)
		retorno = this->conexaoSegura->GetCertificateSubString("CN") == this->conexaoSegura->GetCertificateSubString("O") ? true : false;
	return retorno;
}
bool http::Secure() {
	return (this->secure && this->conexaoSegura != NULL ? true : false);
}
bool http::IsValid() {
	return this->valid;
}
void http::LocalCertificates(string file, string path) {
	http::certificateFile = file;
	http::certificatePath = path;
}

void http::Recursive(string url, int depth) {

	std::cerr << "Acessando <" << url << "> Profundidade:  " << depth << std::endl;

	http m_http(url);

	m_http.start();
	m_http.join();

	if (m_http.IsValid()) {
		UrlNodo * nodo = new UrlNodo();
		nodo->url = url;
		nodo->useSsl = m_http.Secure();
		nodo->CN = m_http.GetCertificateOwner();
		nodo->autoassinado = m_http.IsSelfSign();
		http::urlVisited.push_back(nodo);

		bool found = false;
		if (depth > 0) {
			std::vector<string> urlList = m_http.GetUrlList();
			for (unsigned int i = 0; i < urlList.size(); i++ ) {
				string atual = urlList[i];
				for (unsigned int k = 0; k < http::urlVisited.size(); k++) {
					if (http::urlVisited[k]->url == atual) {
						found = true;
						break;
					}
				}
				if (!found) {
					Recursive(atual,depth-1);
				}
			}
		}
	}
}


UrlNodo::UrlNodo() {
	this->autoassinado = false;
	this->useSsl = false;
}

bool UrlNodo::operator==(const UrlNodo & b) {
	if (this->url == b.url && this->CN == b.CN && this->autoassinado == b.autoassinado)
		return true;
	return false;
}
