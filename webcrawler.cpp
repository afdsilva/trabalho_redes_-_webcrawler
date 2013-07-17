/**
 *
 * FUNÇÕES PARCIAIS
 *
 * **/

#include "http.h"

static std::vector<string> lista_urls;
static std::vector<string> lista_urls_visitadas;
static string fullpath;
static string host_atual, path_atual;

// CRIA O SOCKET
int create_socket(){
	int socket_desc;

	if ((socket_desc = socket(AF_INET , SOCK_STREAM , 0))== -1){
		std::cerr << "Não criou o socket" << std::endl;
        return -1;
    }

    return socket_desc;
}

// FAZ O PAPEL DO DNS
char *get_ip (char *domain){
	struct hostent *host;
	char *host_ip;

	host = gethostbyname(domain);
	if(host == NULL){
		std::cerr << "Erro gethostbyname" << std::endl;
		return NULL;
	}

	host_ip = inet_ntoa(*(struct in_addr *)host->h_addr_list[0]);

	return host_ip;
}

// IP PRO SOCKET PORTA 80
void socket_address(struct sockaddr_in *server, char *host){
	char *host_ip;
	host_ip = get_ip(host);
	if (host_ip == NULL) {
		server->sin_addr.s_addr = -1;
		return;
	}
	server->sin_family = AF_INET;
    server->sin_port = htons(PORT);
    server->sin_addr.s_addr = inet_addr(host_ip);

    for (int i = 0; i < 8; i++){
		server->sin_zero[i] = '\0';
     }
}

// ESTABELECE A CONEXÃO
void socket_connect(int *socket_desc, struct sockaddr_in *server){
	if (connect(*socket_desc, ( struct sockaddr *) server, sizeof(*server)) > 0) {
        std::cerr << "Erro de conexao" << endl;
    	return;
    }
}

// MONTA A REQUISIÇÃO
char *build_request(char *host, char *path) {
    char *request;
    char request_temp[] = "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n";

    request = (char *) malloc(strlen(host) + strlen(path) + strlen(request_temp));
    sprintf(request, request_temp, path, host);

    return request;
}

// ENVIA A REQUISIÇÃO
void send_request(int *socket, char *get) {

	if(send(*socket, get, strlen(get), 0) < 0){
		std::cerr << "Falha no envio da requisição" << endl;
		return ;
	}
}

// RECEBE OS DADOS
std::vector<string> receive_data(int *socket, char *host, char *path){
	std::vector<string> retorno;
	char server_reply[BUFSIZ];
   	char *message;

   	//message = "GET /img_turismo_oqueconhecer/03.jpg HTTP/1.1\r\nHost: ausentesonline.com.br\r\n\r\n";
	message = build_request(host, path);

	//std::ofstream imagem("/home/andref/Downloads/teste_webcrawler.html", ios::out | ios::binary);
	/** fullpath eh setado no create_dir()
	string url = path;


	//std::cerr << fullpath << std::endl;
	**/
	vector<string> str_split;

	boost::split_regex(str_split, fullpath,boost::regex("/"));
	string filename = str_split[str_split.size()-1] == "" ? "index.html" : str_split[str_split.size()-1];
	//std::cerr << "filename: " <<filename << std::endl;
	//fullpath += filename;
	//std::cerr << "fullpath " << fullpath << endl;
	std::ofstream saida(fullpath.c_str(), ios::out);
	std::cerr << "Salvando arquivo: " << fullpath << endl;
	send_request(socket, message);

	int tam = 0;
	memset(server_reply,0,sizeof(server_reply));
	int header = 0;
	char * content = NULL;

	int totalTam = 0;
	int totalTam2 = 0;
	unsigned int ponteiro = 0;

	/** Resposta do receive http **/
	string resposta;
	resposta.clear();
	/**/
	struct timeval tv;
	tv.tv_sec = 10;
	tv.tv_usec = 0;
	if (setsockopt(*socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,  sizeof tv)){
	  perror("setsockopt");
	}
	/**/
	//Receive a reply from the server
	while((tam = recv(*socket, server_reply ,BUFSIZ, 0)) > 0) {
		resposta += server_reply;
		totalTam = totalTam + tam;
		//std::cerr << "Tamanho: " << tam << std::endl;
		if (header == 0) {
			content = strstr(server_reply,"\r\n\r\n");
			if (content != NULL) {
				header = 1;
				content +=4;
				ponteiro = content - server_reply;
			}
		} else {
			ponteiro = 0;
			content = server_reply;
			//puts(content);
		}
		//std::cerr << "Tamanho: " << tam << "content: " << sizeof(content)* strlen(content) << std::endl;

		//std::cerr << "server_reply: " << strlen(server_reply) << std::endl;;
		//std::cerr << "content: " << strlen(content) << std::endl;;
		//printf("Ponteiros: server_reply: %p content: %p \n",server_reply, content);
		//fwrite(content,tam,1,pFile);
		//std::cerr << content << std::endl;
		saida.write(content,tam-ponteiro);

		memset(server_reply,0,tam);
		totalTam2 = totalTam2 + tam;
	}

	//std::cerr << "TotalTam: " << totalTam << std::endl;
	//std::cerr << "Cabecalho: " << std::endl << header << std::endl;
	//std::cerr << "Imagem: " << str.length() << std::endl << str << std::endl;

	/** Printando a resposta que acumulou do recv
	 *  para confirmar que pegou todo conteudo da resposta do http
	* */
	//std::cerr << resposta;
	size_t pos = resposta.find("\r\n\r\n");
	string headerStr( resposta.begin(), resposta.begin()+pos );
	std::size_t found = headerStr.find("text/html");
	if (found != std::string::npos) {

		//boost::regex e("<\\s*A\\s+[^>]*href\\s*=\\s*\"([^\"]*)\"|<img.+?src=[\"'](.+?)[\"'].+?>",
		boost::regex e("<\\s*A\\s+[^>]*href\\s*=\\s*\"([^\"]*)\"",
				   boost::regbase::normal | boost::regbase::icase);

		// lista de string acumulando os links parcialmente
		// depois colocar nos atributos da classe de urlVisited...
		list<string> l;

		//std::cerr << "URL's encontradas " << ":" << endl;
		regex_split(back_inserter(l), resposta, e);

		boost::regex invalid("mailto:|javascript:");

		string temp;
		vector<string> aux;
		string nPath = path;
		while(l.size()){
			aux.clear();
			temp.clear();
			resposta = *(l.begin());
			l.pop_front();
			// Teste para verificar se no <a href> contem mailto: ou javascript:
			if(!(boost::regex_search(resposta, invalid))){
				if (!resposta.empty()){
					boost::regex absoluto("http|https");
					if(!(boost::regex_search(resposta, absoluto))){
						if (resposta.at(0) == '/') {
							resposta.replace(resposta.begin(), resposta.begin()+1,"");
						}
						temp += "http://";
						//std::cerr <<"temp 1 " << temp << endl;
						temp += host;
						//std::cerr <<"temp 2 " << temp << endl;

						boost::regex arquivo(".*");
						if (boost::regex_search(path, arquivo)) {
							boost::split_regex(aux, nPath, boost::regex("/"));
							for(int i = 0; i < (int)aux.size()-1; i++){
								temp += aux[i];
								if(i < (int)aux.size()-1){
									temp += "/";
								}
							}
						}
						else{
							temp += path;
						}
						//std::cerr <<"temp 3 " << temp << endl;
						resposta = temp + resposta;
					}
					//std::cerr <<"resposta " << resposta << endl;
					retorno.push_back(resposta);
				}
			}
		}
	}
	saida.close();

	return retorno;
}

void create_dir(char *host, char *path){
	int status;

	// Pega o diretorio padrao do user
	struct passwd *pw = getpwuid(getuid());
	char *homedir = pw->pw_dir;

	string diretorio;
	string host_dir = host;
	string path_dir = path;

	// /home/user
	diretorio += homedir;
	// /home/user/
	struct stat fileStat;

	diretorio += "/male_webcrawler/";
	if (stat(diretorio.c_str(),&fileStat) < 0) {
		if((status = mkdir(diretorio.c_str(), 0777)) < 0){
			std::cerr << "Erro ao criar o diretorio " << diretorio << endl;
		}

	}
	diretorio += host_dir;
	diretorio += "/";

	/**boost::regex test("/");
	if(boost::regex_search(path, test)){
		diretorio += "/";
	}**/

	if (stat(diretorio.c_str(),&fileStat) < 0) {
		if((status = mkdir(diretorio.c_str(), 0777)) < 0){
			std::cerr << "Erro ao criar o diretorio " << diretorio << endl;
		}
	}

	//cria os diretorios recursivamentes, a partir do host_dir
	vector<string> str_split;

	string extensao = "";
	string temp;
	boost::split_regex(str_split, path_dir,boost::regex("/"));
	temp.clear();
	temp += diretorio;

	for (int i = 0; i < (int)str_split.size(); i++) {
		//std::cerr << str_split[i] << endl;
		if (!str_split[i].empty()) {
			std::size_t found = str_split[i].find("?");
			if (found != std::string::npos || !extensao.empty()) {
				if (!extensao.empty())
					temp += "_";
				extensao = ".html";
				if(i < (int)str_split.size() -1){
					temp += str_split[i];
				}
			} else {

				if(i < (int)str_split.size() -1){
					temp += str_split[i] + "/";
				}
				if (stat(temp.c_str(),&fileStat) < 0) {
					//std::cerr << "temp: " << temp << endl;
					//if(temp[temp.size()-1] == '/'){
					//	temp[temp.size()-1] = '\0';
					//}
					//std::cerr <<"TEMP " << temp << endl;
					if((status = mkdir(temp.c_str(), 0777)) < 0){
						std::cerr << "Erro ao criar o diretorio " << temp << endl;
					}
					chmod(temp.c_str(), 0777);
				}
			}
		}
	}
	fullpath.clear();
	//fullpath = diretorio;
	fullpath += temp;
	fullpath += str_split[str_split.size()-1];
	fullpath += extensao;
}

void FazTudo(string url, int depth) {

	//std::cerr << "URL:" << url << std::endl;
	//boost::regex expr("^(?:http://)?([^/]+)(?:/?.*/?)/(.*)$");
	vector<string> str_split;

	boost::split_regex(str_split, url,boost::regex("/"));
	string domain;
	string path = "/";
	string file = str_split[str_split.size()-1];

	// Identifica se é link absoluto

	boost::regex absoluto("http|https");
	if((boost::regex_search(str_split[0], absoluto))){
		domain = str_split[2];
		for (int i = 3; i < (int)str_split.size();i++) {
			path += str_split[i];
			if (i < (int)str_split.size() - 1)
				path += "/";
		}
	} else {	// link relativo
		domain = host_atual;
		path = path_atual + str_split[0];
		//std::cerr << "LINK RELATIVO ARRUMADO " << domain << path << endl;
		//domain = str_split[0];
	}

	//if (domain.find("www") != std::string::npos) {
		//domain.replace(domain.begin(),domain.begin()+4,"");
	//}

	//std::cerr << "domain: " << domain << endl;
	//std::cerr << "path: " << path << endl;
	//std::cerr << "file: " << file << endl;
	char * host_test = (char *)domain.c_str();
	char * path_test = (char *)path.c_str();
	std::string new_url;
	bool existe = false;

	int socket_desc;
	struct sockaddr_in server;

	//http:///wp-content/uploads/2013/03/intranet-corporativa-200x200.jpg
	//digitaisdomarketing.com.br
	//wp-content/uploads/2013/03/intranet-corporativa-200x200.jpg
	//intranet-corporativa-200x200.jpg
	create_dir(host_test, path_test);
	socket_desc = create_socket();
	socket_address(&server, host_test);

	if ((int)server.sin_addr.s_addr == -1)
		return;

	socket_connect(&socket_desc, &server);

	std::vector<string> lista = receive_data(&socket_desc, host_test, path_test);
	for (int i=0; i < (int) lista.size();i++) {
		new_url = lista[i];
		std::cerr << "Verificando: " << new_url;
		 existe = false;
		for (int i = 0; i < (int) lista_urls_visitadas.size();i++) {
			if (lista_urls_visitadas[i] == new_url) {
				//std::cerr << "URL jah existe na lista:" << new_url << std::endl;
				//std::cerr << "Visitada " << lista_urls_visitadas[i] << endl;
				existe = true;
			}
		}
		if (!existe) {
			lista_urls_visitadas.push_back(new_url);
			//std::cerr << "Profundidade:" << depth << std::endl;
			//host_atual.clear();
			host_atual = domain;
			//path.clear();
			path_atual = path;
			//std::cerr << "host_atual " <<  host_atual << endl;
			//std::cerr << "path_atual " <<  path_atual << endl;
			std::cerr << " ... acessando profundidade: " << depth << endl;
			if (depth >= 0) {
				FazTudo(new_url,depth-1);
			}
		} else {
			std::cerr << " ... link existente" << endl;
		}
	}
	if((shutdown(socket_desc, 2)) < 0){
		std::cerr << "Erro ao fechar o socket" << endl;
	}
}

int main(int argc , char *argv[]) {
	string url;
	int depth;

	if(argc != 3){
		std::cerr << "parametros invalidos" << endl;
		return 0;
	}

	// Inserindo http, caso seja informada uma url sem http
	boost::regex teste("http|https");
	if(!(boost::regex_search(argv[1], teste))){
		url += "http://";
	}

	url += argv[1];
	std::cerr << url << endl;
	depth = atoi (argv[2]);

    //FazTudo("http://www.ausentesonline.com.br/imagem.php?src=img_turismo_oqueconhecer/museu_02.jpg",2);
    //FazTudo("http://lista10.org/cinema/os-10-filmes-mais-caros-da-historia-do-cinema/",1);
	FazTudo(url, depth);

	std::cout << "Lista URL visitadas:" << std::endl;
	for (int i = 0; i < (int)lista_urls_visitadas.size(); i++) {
		std::cout << lista_urls_visitadas[i] << std::endl;
	}
	/**
    http m_http("http://lista10.org/",1);
	m_http.start();
	std::vector<string> lista = m_http.GetUrlList();
	for (int i = 0; i < (int) lista.size();i++) {
		std::cerr << lista[i] << std::endl;
	}
	**/

	return 0;
}
