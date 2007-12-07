
/* Header's */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <sys/signal.h>
#include <unistd.h>
#include <regex.h>
#include "config.h"
#include "wiki.h"

static FILE *socket_fp_wiki;

static int fd_wiki;

int wiki(char *key, char *stringr) {
	FILE *socket_fr_wiki = 0;
	char socket_buffer_wiki[262144], error[128], temp[4096];
	int i;
	struct sockaddr_in addr_wiki;
	struct hostent *host_wiki;
	host_wiki = gethostbyname("wikipedia.org");
	
	if (host_wiki == NULL) {
		printf("Erro: Host Desconhecido!!\n");
	}
	if((fd_wiki = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return;
	}
	memset((void *)&addr_wiki, 0, sizeof(addr_wiki));
	
	addr_wiki.sin_family = AF_INET;
	addr_wiki.sin_port = htons(80);
	memcpy((void *)&addr_wiki.sin_addr, host_wiki->h_addr, host_wiki->h_length);
	bzero(&(addr_wiki.sin_zero), 8);
	
	if (connect(fd_wiki, (struct sockaddr *)&addr_wiki, sizeof(addr_wiki)) == -1) {
		close(fd_wiki);
	}
	if(!(socket_fp_wiki = fdopen(fd_wiki, "w")))return;
	if(!(socket_fr_wiki = fdopen(fd_wiki, "r")))return;

	for(i=0; i<strlen(key); i++)
		if(key[i]==0x20)
			key[i]='_';

	fprintf(socket_fp_wiki, "GET /w/index.php?title=%s&redirect=yes HTTP/1.1\n",key);
	fprintf(socket_fp_wiki, "Host: en.wikipedia.org\n");
	fprintf(socket_fp_wiki, "User-Agent: Xape BotIRC\n\n\n");
	
	fflush(socket_fp_wiki);

	printf("%s\n", key);
	memset(socket_buffer_wiki, 0, 262144);
	while(fgets(temp, 4096, socket_fr_wiki)) {
		fflush(socket_fp_wiki);
		strcat(socket_buffer_wiki, temp);
		memset(temp, 0, 4096);
	}
	fprintf(socket_fp_wiki, "Connection: close\n");
	fflush(socket_fp_wiki);
  fclose(socket_fr_wiki);
	fclose(socket_fp_wiki);
	close(fd_wiki);

	parser(socket_buffer_wiki, key, stringr);

	return(0);
}
//exec -o head -n1 /dev/urandom

int parser(char *string, char *key, char *stringr) {
	char regex[128], *temp, *temp2, tempr[262144];
	regex_t preg;
	regmatch_t pmatch;
	int i;
	snprintf(regex, 128, "<p>[^<]*<b>[^<]+</b>.*</p>");

	if(regcomp(&preg, regex, REG_EXTENDED)) { printf("error: regcomp\n"); return 1; }
	if(regexec(&preg, string, 1, &pmatch, 0)) { printf("error: regexec"); return 1; }

	temp = (string+pmatch.rm_so);

	*(string+pmatch.rm_eo+4) = '\0';

	if(temp==NULL) return 1;

	strcpy(tempr, temp);
	temp = strstr(tempr, "</p>");
	if(temp == NULL) return 1;
	*temp = '\0';
	//	strcpy(stringr, temp);
	printf("%s\n", tempr);

	

	snprintf(regex, 128, "<[^>]+>");
	if(regcomp(&preg, regex, REG_EXTENDED)) { printf("error: regcomp\n"); return 1; }
	while(!(regexec(&preg, tempr, 1, &pmatch, 0))) { 
		strcpy((tempr+pmatch.rm_so), (tempr+pmatch.rm_eo));
	}
	snprintf(stringr, 4096, "%s", tempr);
	snprintf(tempr, 4096, " More: http://pt.wikipedia.org/w/index.php?title=%s&redirect=yes", key);
	strcat(stringr, tempr);
	printf("%s\n", stringr);

	return 0;
}
