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

char *build_request(char *host, char *path) {
    char *request;
    char request_temp[] = "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n";
 
    request = (char *) malloc(strlen(host) + strlen(path) + strlen(request_temp) - 5);
    sprintf(request, request_temp, path, host);
	
    return request;
}

void send_request(int socket, char *get) {
	
	if(send(socket, get, strlen(get), 0) < 0){
		cout << "Falha no envio da requisição" << endl;
		return ;
	}
}

int main(int argc , char *argv[]) {

	int socket_desc;
	//int depth;

	char *message;
	struct hostent *host;
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
	
    if ((socket_desc = socket(AF_INET , SOCK_STREAM , 0))== -1){
        cout << "Não criou o socket" << endl;
    }
    
    host = gethostbyname(host_test);
    
    memcpy(&server.sin_addr,host->h_addr_list[0],host->h_length);
	server.sin_family = AF_INET;
    server.sin_port = htons(80);
    
    if (connect(socket_desc, ( struct sockaddr *) & server, sizeof(server)) > 0) {
        cout << "Erro de conexao" << endl;
    	return 1;
    }
    /** Algoritmo de Teste de Download, esta baixando imagens perfeitamente, porem tras lixo quando baixa HTML;
     * necessario testar com outros arquivos, porem se for só html, talvez nao tenha problemas;
     * Devido a união do meu (Andre) algoritmo e o do Mario, pode ta meio estranho, que ta usando uma parte de
     * um algoritmo e parte de outro, mas ta funcionando
     **/
    /**/
	char server_reply[BUFSIZ];
   	//message = "GET /img_turismo_oqueconhecer/03.jpg HTTP/1.1\r\nHost: ausentesonline.com.br\r\n\r\n";
	message = build_request(host_test, path_test);

	//std::ofstream imagem("/home/andref/Downloads/teste_webcrawler.html", ios::out | ios::binary);
	std::ofstream imagem("/home/andref/Downloads/teste_webcrawler.jpg", ios::out | ios::binary);

	//FILE * pFile;
	//pFile = fopen("/home/andref/Downloads/pudim.jpg", "w");

	if( send(socket_desc , message , strlen(message) , 0) < 0) {
		std::cout << "ERRO ao conectar" << std::endl;
		return 1;
	}

	int tam = 0;
	memset(server_reply,0,sizeof(server_reply));
	int header = 0;
	char * content = NULL;

	int totalTam = 0;
	int totalTam2 = 0;
	unsigned int ponteiro = 0;

	//Receive a reply from the server
	while( (tam = recv(socket_desc, server_reply ,BUFSIZ, 0)) > 0) {
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
		imagem.write(content,tam-ponteiro);

		memset(server_reply,0,tam);
		totalTam2 = totalTam2 + tam;
	}
	std::cout << "TotalTam: " << totalTam << std::endl;
	//std::cout << "Cabecalho: " << std::endl << header << std::endl;
	//std::cout << "Imagem: " << str.length() << std::endl << str << std::endl;

	imagem.close();

	//fclose(pFile);
	/** /
	if(( tam = recv(socket_desc, server_reply , sizeof(server_reply), MSG_WAITALL)) < 0)
	{
		puts("recv failed");
	}
	std::cout << "Tamanho: " << tam << std::endl;
	puts(server_reply);

	//close(socket_desc);
    
	//cout << message << endl;
	//send_request(socket_desc, message);
	**/
	return 0;
}
