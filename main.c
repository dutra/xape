/*Desenvolvido por Isaque Franca Dutra, sob a GNU GPL*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <sys/signal.h>
#include <unistd.h>
#include "parser.h"
#include "config.h"
#include "db.h"

FILE *socket_fp;
FILE *socket_fr;
int pvtall=0, fd, quit=0;

int main(void) {
	int i;	
	char socket_buffer[1026];
	//	FILE *socket_fr = 0;
	pthread_t pid;
	struct sockaddr_in addr;
	struct hostent *host;
	
	char buffer[1026], texto[1024];
	host = gethostbyname(NET);
	
	if (host == NULL) {
		printf("Erro: Host Desconhecido!!\n");
		exit(1);
	}
	
	
	if((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Erro Socket Main");
		exit(1);
	}
	memset((void *)&addr, 0, sizeof(addr));
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(6667);
	memcpy((void *)&addr.sin_addr, host->h_addr, host->h_length);
	bzero(&(addr.sin_zero), 8);
	
	if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		printf("Erro Connect Main");
		close(fd);
	}
	if(!(socket_fp = fdopen(fd, "w")))printf("Erro fp main");
	if(!(socket_fr = fdopen(fd, "r")))printf("Erro fr main");
	
	fprintf(socket_fp, "USER " NICK " \"\" \"10.0.0.0\" :" NICK "\r\nNICK " NICK "\r\n");
	fflush(socket_fp);
	fprintf(socket_fp, "NickServ identify %s\r\n",NSPASSWD);
	fflush(socket_fp);
	sleep(2);
	fprintf(socket_fp, "JOIN " CHANNEL "\r\n");
	fflush(socket_fp);
	memset(buffer, 0, 1026);
	while(fgets(socket_buffer, 1026, socket_fr)) {
		printf("\n%s\n",socket_buffer);
		save_log(socket_buffer);
		parser_feedline(socket_buffer);
		memset(buffer, 0, 1026);
	}

	fclose(socket_fr);
	fclose(socket_fp);
	close(fd);
	return(0);
}

//exec -o head -n1 /dev/urandom
//USER Xape "" "10.0.0.0" :Xape
//NICK Xape
