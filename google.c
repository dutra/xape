
/* Header's */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <sys/signal.h>
#include <unistd.h>
#include "config.h"
#include "google.h"

static FILE *socket_fp_g;

static int fd_g;

char * google(char *key) {
	FILE *socket_fr_g = 0;
	char socket_buffer_g[1026];
	struct sockaddr_in addr_g;
	struct hostent *host_g;
	int i;
	char term_g[4];
	host_g = gethostbyname("google.com.br");
	
	if (host_g == NULL) {
		printf("Erro: Host Desconhecido!!\n");
		return;	
	}
	if((fd_g = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error: socket");
		return;
	}
	memset((void *)&addr_g, 0, sizeof(addr_g));
	
	addr_g.sin_family = AF_INET;
	addr_g.sin_port = htons(80);
	memcpy((void *)&addr_g.sin_addr, host_g->h_addr, host_g->h_length);
	bzero(&(addr_g.sin_zero), 8);
	
	if (connect(fd_g, (struct sockaddr *)&addr_g, sizeof(addr_g)) == -1) {
		printf("Error: connect");
		close(fd_g);
	}
	if(!(socket_fp_g = fdopen(fd_g, "w")))perror("fp");
	if(!(socket_fr_g = fdopen(fd_g, "r")))perror("fr");
	printf("11111111\n");
	fprintf(socket_fp_g, "\n");
	fflush(socket_fp_g);
	fprintf(socket_fp_g, "GET /search?hl=pt-BR&q=%s&btnG=Pesquisar&meta=lr%3Dlang_pt HTTP/1.1",key);
	fflush(socket_fp_g);
	fprintf(socket_fp_g, "\n\n");
	fflush(socket_fp_g);
	printf("%s\n", key);
	memset(socket_buffer_g, 0, 1026);
	term_g[0]=48; term_g[1]=13; term_g[2]=10; term_g[3]='\0';
	while(fgets(socket_buffer_g, 1026, socket_fr_g)) {
			printf("\n%s\n",socket_buffer_g);
			if(!(strcmp(socket_buffer_g, term_g))){ printf("\nachei um 0\n"); break; }
			memset(socket_buffer_g, 0, 1026);
	}
  fclose(socket_fr_g);
	fclose(socket_fp_g);
	close(fd_g);
	return(0);
}
//exec -o head -n1 /dev/urandom

//char * parser_g(char *string) {
