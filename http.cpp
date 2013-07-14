/*
 * http.cpp
 *
 *  Created on: 04/07/2013
 *      Author: andref
 */

using namespace std;


#include "http.h"

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

int main(int argc , char *argv[]) {

	int socket_desc;
	//int depth;
	char * message;
	char server_reply[BUFSIZ];
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
    host = gethostbyname("ausentesonline.com.br");

    memcpy(&server.sin_addr,host->h_addr_list[0],host->h_length);
	server.sin_family = AF_INET;
    server.sin_port = htons(80);

    if (connect(socket_desc, ( struct sockaddr *) & server, sizeof(server)) > 0) {
        cout << "erro de conexao" << endl;
    	return 1;
    }

    //message = "GET / HTTP/1.1\r\nHost: www.pudim.com.br\r\n\r\n";
    //http://ausentesonline.com.br/img_turismo_oqueconhecer/03.jpg
   //message = "GET /index.php HTTP/1.1\r\nHost: ausentesonline.com.br\r\n\r\n";
   message = "GET /img_turismo_oqueconhecer/03.jpg HTTP/1.1\r\nHost: ausentesonline.com.br\r\n\r\n";

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
	/** teste para baixar imagem **/

	imagem.close();
	//fclose(pFile);
	/**/
	/**
	if(( tam = recv(socket_desc, server_reply , sizeof(server_reply), MSG_WAITALL)) < 0)
	{
		puts("recv failed");
	}
	std::cout << "Tamanho: " << tam << std::endl;
	puts(server_reply);
	**/
	puts("Reply received\n");
	//close(socket_desc);
    return 0;
}
