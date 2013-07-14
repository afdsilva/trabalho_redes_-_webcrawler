#include "http.h"
#include <ostream>
#include <fstream>
using namespace std;

http::http() {
	// TODO Auto-generated constructor stub
	this->socket_desc = socket(AF_INET , SOCK_STREAM , 0);

}

http::~http() {
	// TODO Auto-generated destructor stub
}

bool http::Server(string url, int port ) {
	return true;
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
    
	message = build_request(host_test, path_test);
	//cout << message << endl;
	send_request(socket_desc, message); 
	
    return 0;
}



