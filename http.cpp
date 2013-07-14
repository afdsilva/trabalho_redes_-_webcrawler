#include "http.h"
#include <ostream>
#include <fstream>
using namespace std;

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
		exit(1);
	}
	
	host_ip = inet_ntoa(*(struct in_addr *)host->h_addr_list[0]);
	
	return host_ip;
}

// IP PRO SOCKET PORTA 80
void socket_address(struct sockaddr_in *server, char *host){
	char *host_ip;
	host_ip = get_ip(host);

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
 
    request = (char *) malloc(strlen(host) + strlen(path) + strlen(request_temp) - 5);
    sprintf(request, request_temp, path, host);

    return request;
}

// ENVIA A REQUISIÇÃO
void send_request(int *socket, char *get) {
	cout << get << endl;
	if(send(*socket, get, strlen(get), 0) < 0){
		cout << "Falha no envio da requisição" << endl;
		return ;
	}
}

// RECEBE OS DADOS
void receive_data(int *socket, char *host, char *path){
	char server_reply[BUFSIZ];
   	char *message;
   	
   	//message = "GET /img_turismo_oqueconhecer/03.jpg HTTP/1.1\r\nHost: ausentesonline.com.br\r\n\r\n";
	message = build_request(host, path);

	//std::ofstream imagem("/home/andref/Downloads/teste_webcrawler.html", ios::out | ios::binary);
	std::ofstream saida("/home/mario/teste_webcrawler", ios::out);

	send_request(socket, message);
	
	int tam = 0;
	memset(server_reply,0,sizeof(server_reply));
	int header = 0;
	char * content = NULL;

	int totalTam = 0;
	int totalTam2 = 0;
	unsigned int ponteiro = 0;
	
	//Receive a reply from the server
	while( (tam = recv(*socket, server_reply ,BUFSIZ, 0)) > 0) {
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
		std::cout << content << std::endl;
		saida.write(content,tam-ponteiro);

		memset(server_reply,0,tam);
		totalTam2 = totalTam2 + tam;
	}
	std::cout << "TotalTam: " << totalTam << std::endl;
	//std::cout << "Cabecalho: " << std::endl << header << std::endl;
	//std::cout << "Imagem: " << str.length() << std::endl << str << std::endl;

	saida.close();
}

int main(int argc , char *argv[]) {

	int socket_desc;
    struct sockaddr_in server;
	char *host_test = "www.climatempo.com.br";
	char *path_test = "/";
	
	/**
	if(argc != 3){
		std::cout << "erro de parametros" << std::endl;
		return 1;
	}
	int depth;
	depth = atoi(argv[2]);
	host = gethostbyname(argv[1]);
	**/

	
	socket_desc = create_socket();
	socket_address(&server, host_test);
	socket_connect(&socket_desc, &server); 
    receive_data(&socket_desc, host_test, path_test);
    
    if((shutdown(socket_desc, 2)) < 0){
		cout << "Erro ao fechar o socket" << endl;
	}
    
	return 0;
}
