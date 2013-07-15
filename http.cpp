#include "http.h"

using namespace std;

static std::vector<string> lista_urls;
static std::vector<string> lista_urls_visitadas;
static string fullpath;

http::http() {
}
http::http(string url, int port) {
}

http::~http() {

}
bool http::Server(string url, int port ) {
	bool retorno = true;
	try {
	} catch (int e) {
		retorno = false;
	}

	return retorno;
}

string http::ReceiveData() {
	string retorno;
	try {
	} catch (int e) {
		retorno.clear();
	}
	return retorno;
}
bool http::SendRequest() {
	bool retorno = true;
	try {
	} catch (int e) {
		retorno = false;
	}
	return retorno;
}

char * http::BuildQuery(char * host, char * page) {
	char * retorno = NULL;
	try {
	} catch (int e) {
		retorno = NULL;
	}
	return retorno;
}

/** 
 * 
 * FUNÇÕES PARCIAIS
 * 
 * **/
 
// CRIA O SOCKET 
int create_socket(){
	int socket_desc;
	
	if ((socket_desc = socket(AF_INET , SOCK_STREAM , 0))== -1){
		cout << "Não criou o socket" << endl;
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
		cout << "Erro gethostbyname" << endl;
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
        cout << "Erro de conexao" << endl;
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
		cout << "Falha no envio da requisição" << endl;
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
	vector<string> str_split;


	boost::split_regex(str_split, url,boost::regex("/"));
	string filename = str_split[str_split.size()-1] == "" ? "index.html" : str_split[str_split.size()-1];
	std::cout << "filename: " <<filename << std::endl;
	fullpath += "/";
	fullpath += filename;
	//std::cout << fullpath << std::endl;
	**/
	std::ofstream saida(fullpath.c_str(), ios::out);

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
	
	//Receive a reply from the server
	while( (tam = recv(*socket, server_reply ,BUFSIZ, 0)) > 0) {
		resposta += server_reply;
		totalTam = totalTam + tam;
		//std::cout << "Tamanho: " << tam << std::endl;
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
		//std::cout << "Tamanho: " << tam << "content: " << sizeof(content)* strlen(content) << std::endl;

		//std::cout << "server_reply: " << strlen(server_reply) << std::endl;;
		//std::cout << "content: " << strlen(content) << std::endl;;
		//printf("Ponteiros: server_reply: %p content: %p \n",server_reply, content);
		//fwrite(content,tam,1,pFile);
		//std::cout << content << std::endl;
		saida.write(content,tam-ponteiro);

		memset(server_reply,0,tam);
		totalTam2 = totalTam2 + tam;
	}
	
	//std::cout << "TotalTam: " << totalTam << std::endl;
	//std::cout << "Cabecalho: " << std::endl << header << std::endl;
	//std::cout << "Imagem: " << str.length() << std::endl << str << std::endl;
	 
	/** Printando a resposta que acumulou do recv
	 *  para confirmar que pegou todo conteudo da resposta do http
	* */ 
	//cout << resposta;
	size_t pos = resposta.find("\r\n\r\n");
	string headerStr( resposta.begin(), resposta.begin()+pos );



	if (headerStr.find("text/html") != std::string::npos) {

		//boost::regex e("<\\s*A\\s+[^>]*href\\s*=\\s*\"([^\"]*)\"|<img.+?src=[\"'](.+?)[\"'].+?>",
		boost::regex e("<\\s*A\\s+[^>]*href\\s*=\\s*\"([^\"]*)\"",
				   boost::regbase::normal | boost::regbase::icase);

		// lista de string acumulando os links parcialmente
		// depois colocar nos atributos da classe de urlVisited...
		list<string> l;

		//cout << "URL's encontradas " << ":" << endl;
		regex_split(back_inserter(l), resposta, e);

		while(l.size()){
			resposta = *(l.begin());
			l.pop_front();
			//cout << resposta << endl;
			if (!resposta.empty())
				retorno.push_back(resposta);
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
	diretorio += "/";
	diretorio += host_dir;
	diretorio += "/";

	struct stat fileStat;
	if (stat(diretorio.c_str(),&fileStat) < 0) {
		if((status = mkdir(diretorio.c_str(), 0777)) < 0){
			cout << "Erro ao criar o diretorio " << diretorio << endl;
		}
	}

	//cria os diretorios recursivamentes, a partir do host_dir
	vector<string> str_split;

	string extensao = "";
	string temp;
	boost::split_regex(str_split, path_dir,boost::regex("/"));

	for (int i = 0; i < (int)str_split.size(); i++) {
		cout << str_split[i] << endl;
		if (!str_split[i].empty()) {
			if (str_split[i].find("?") != std::string::npos || !extensao.empty()) {
				if (!extensao.empty())
					temp += "_";
				extensao = ".html";
				temp += str_split[i];
			} else {
				temp += str_split[i];
				if (stat(temp.c_str(),&fileStat) < 0) {
					cout << "temp: " << temp << endl;
					if((status = mkdir(temp.c_str(), 0777)) < 0){
						cout << "Erro ao criar o diretorio " << temp << endl;
					}
				}
			}
		}
	}
	fullpath = diretorio;
	fullpath += temp;
	fullpath += extensao;
	cout << "Saida: " << fullpath << endl;
}

void FazTudo(string url, int depth) {

	if (depth >= 0) {
		//std::cout << "URL:" << url << std::endl;
		//boost::regex expr("^(?:http://)?([^/]+)(?:/?.*/?)/(.*)$");
		vector<string> str_split;

		boost::split_regex(str_split, url,boost::regex("/"));
		string domain;
		string path = "/";
		string file = str_split[str_split.size()-1];
		if (str_split[0].find("http") != std::string::npos) {
			domain = str_split[2];
			for (int i = 3; i < (int)str_split.size();i++) {
				path += str_split[i];
				if (i < (int)str_split.size() - 1)
					path += "/";
			}
		} else {
			domain = str_split[0];
		}
		if (domain.find("www") != std::string::npos) {
			domain.replace(domain.begin(),domain.begin()+4,"");
		}
		//cout << "domain: " << domain << endl;
		//cout << "path: " << path << endl;
		//cout << "file: " << file << endl;
		char * host_test = (char *)domain.c_str();
		char * path_test = (char *)path.c_str();

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
		std::string new_url;
		bool existe = false;
		for (int i=0; i < (int) lista.size();i++) {
			new_url = lista[i];
			cout << new_url << endl;
			existe = false;
			for (int i = 0; i < (int) lista_urls_visitadas.size();i++) {
				if (lista_urls_visitadas[i] == new_url) {
					std::cout << "URL jah existe na lista:" << new_url << std::endl;
					existe = true;
				}
			}
			if (!existe) {
				lista_urls_visitadas.push_back(new_url);
				std::cout << "Profundidade:" << depth << std::endl;
				FazTudo(new_url,depth-1);
			}
		}

	    if((shutdown(socket_desc, 2)) < 0){
			cout << "Erro ao fechar o socket" << endl;
		}
	}
}

int main(int argc , char *argv[]) {

    //FazTudo("http://www.ausentesonline.com.br/imagem.php?src=img_turismo_oqueconhecer/museu_02.jpg",2);
    FazTudo("http://www.peliculasimportadas.com.br",2);

	return 0;
}
