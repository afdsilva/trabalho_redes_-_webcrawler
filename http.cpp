/*
 * http.cpp
 *
 *  Created on: 04/07/2013
 *      Author: andref
 */

using namespace std;


#include "http.h"

http::http() {
	// TODO Auto-generated constructor stub
	//metodos precisam ser completados para não ter problema de "sobrescrita"
	this->socket_desc = socket(AF_INET , SOCK_STREAM , 0);

}
http::~http() {

}
bool http::Server(string url, int port ) {
	return true;
}
int main(int argc , char *argv[]) {

	int socket_desc;
	//int depth;
	char * message;
	char server_reply[2000];
	struct hostent * host;
    struct sockaddr_in server;
	
	/**
	if(argc != 3){
		std::cout << "erro de parametros" << std::endl;
		return 1;
	}
	**/
	
	//depth = atoi(argv[2]);
	
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);

    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    
	//host = gethostbyname(argv[1]);
    host = gethostbyname("www.cplusplus.com");
    
    memcpy(&server.sin_addr,host->h_addr_list[0],host->h_length);
	server.sin_family = AF_INET;
    server.sin_port = htons(80);

    if (connect(socket_desc, ( struct sockaddr *) & server, sizeof(server)) > 0) {
        cout << "erro de conexao" << endl;
    	return 1;
    }

    message = "GET / HTTP/1.0\r\n\r\n";


	if( send(socket_desc , message , strlen(message) , 0) < 0)
	{
		puts("Send failed");
		return 1;
	}
	puts("Data Send\n");

	int tam = 0;
	//Receive a reply from the server
	memset(server_reply,0,sizeof(server_reply));
	char * htmlcontent;
	int html = 0;
	string str;
	while( (tam = recv(socket_desc, server_reply , BUFSIZ, 0)) > 0) {
		if (html == 0) {
			htmlcontent = strstr(server_reply, "\r\n\r\n");
			if(htmlcontent != NULL) {
				html = 1;
				htmlcontent += 4;
			}
		} else {
			htmlcontent = server_reply;
		}
		if(html) {
			str += htmlcontent;
		}
		str += server_reply;

		memset(server_reply, 0, tam);
	}

	std::cout << str << std::endl;
	/** teste para baixar imagem
	ofstream imagem;
	imagem.open("/home/andref/Downloads/teste.jpg", ios::out | ios::binary);
	imagem << str;
	imagem.close();
	**/
	/**
	if(( tam = recv(socket_desc, server_reply , sizeof(server_reply), MSG_WAITALL)) < 0)
	{
		puts("recv failed");
	}
	std::cout << "Tamanho: " << tam << std::endl;
	puts("Reply received\n");
	puts(server_reply);
	**/
	//close(socket_desc);
    return 0;
}
