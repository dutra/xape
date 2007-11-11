/* Headers */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/signal.h>
#include "db.h"
#define   PORT   20000
#define   BACKLOG       5
char * dcc_chat(void);
static void new(int);
static void envias(int);
static void enviaa(void);
static void wait(pthread_t *);
static int parser_dcc(int, char *);
static int fd, fdnew[5], socketopen=-1;
static char msg[6][2048], msg_all[2048], login[5][128];
static FILE *socket_fr[5], *socket_fp[5];
static struct sockaddr_in addr;
static int size;

void new(int i) {
	
	char socket_buffer[1026], user[128], senha[128];
	int bytes=0, b=0; int log;
	pthread_t pid[5];
	
	if(!(socket_fr[i] = fdopen(fdnew[i], "r")))perror("fr");
	strcpy(msg[i], "Login: ");
	while((fgets(socket_buffer, 1026, socket_fr[i]))) { 
		strcpy(user, socket_buffer);
		memset(socket_buffer, 0, 1026);
		strcpy(msg[i], "Senha: ");
		fflush(stdout);
		fgets(socket_buffer, 1026, socket_fr[i]);
		strcpy(senha, socket_buffer);
		fflush(stdout);
		for(b = 0; b < 128 && (user[b] != '\n') && (user[b] != '\0') && (user[b] != 13); b++);
		user[b] = '\0';
		for(b = 0; b < 128 && (senha[b] != '\n') && (senha[b] != '\0') && (senha[b] != 13); b++);
		senha[b] = '\0';
		printf("user: %s, senha: %s\n", user, senha);
		fflush(stdout);
		log = search_db(user, senha);
		
		fflush(stdout);
		if(log == -1) strcpy(msg[i], "Invalid Login, Try again\r\nLogin: ");
		else if(!log) strcpy(msg[i], "Invalid Password, TryAgain\r\nLogin: ");
		if(log == 1) {
			strcpy(msg[i], "Accepted Login and Password");
		
		break;
		}
		memset(socket_buffer, 0, 1026);
		fflush(stdout);
	}
	
	strcpy(login[i], user);
	snprintf(msg_all, 2048, "%s Online", login[i]);
	
	fflush(stdout);
	memset(socket_buffer, 0, 1026);
	while((fgets(socket_buffer, 1026, socket_fr[i]))) {
		usleep(10);
		for(b = 0; b < 1026; b++) if(socket_buffer[b] == '\n') socket_buffer[b] = '\0';
		if((strlen(socket_buffer)) != 0) {
		printf("%d\n", strlen(socket_buffer));
		printf("%s\n", socket_buffer);
		if(!(parser_dcc(i, socket_buffer)))
		snprintf(msg_all, 2048, "%s, %s", login[i], socket_buffer);
		}
		memset(socket_buffer, 0, 1026);
		
		
	}

        
	close(socket_fr[i]);
	close(fdnew[i]);
	snprintf(msg_all, 2048, "%s Offline", login[i]);
	memset(login[i], 0, 128);
}
int parser_dcc(int i, char *msg) {
	int retorno = 0;
	char tmp[2048], *ptmp, tmp2[2048];
	strcpy(tmp, msg);
	
	ptmp = strtok(tmp, " \0");
	if(ptmp == NULL) return;
	if(!(strcmp(ptmp, "@@"))) {
		retorno = 1;
		printf("Comando\n");
		ptmp = strtok(0, "[\0");
		if(ptmp == NULL) return;
		printf("a\n\n");
		if(!strcmp(ptmp, "quit")) {
		  ptmp = strtok(0, "]\0");
		  if(ptmp == NULL) return;
		  printf("quit: %s\n", ptmp);
		  snprintf(msg_all, 2048, "Quit: %s", ptmp);
		  fd=0;
		}
	}
	fflush(stdout);
	return retorno;
}
	

void envias(int i) {
	int b;
	if(!(socket_fp[i] = fdopen(fdnew[i], "w")))perror("fp");
	while(fdnew[i]) {
		usleep(10);
		if((strlen(msg[i])) != 0) {
			fprintf(socket_fp[i], "%s\r\n", msg[i]);
			fflush(socket_fp[i]);
			memset(msg[i], 0, 2048);
		}
	}
}
void enviaa(void) {
	int i, b;
	char tmp[128], *user_msg;
	while(1) {
		usleep(10);
		//b=0;
		//while(msg_all[b]) 
		//	if(msg_all[b] == ',') user_msg[b] = '\0';
		//	else user_msg[b] = msg_all[b++];
		strcpy(tmp, msg_all);
		user_msg = strtok(tmp, ",\0");
		for(i = 0; i < 5; i++) {
			if(((strlen(msg_all)) != 0) && ((strlen(login[i])) != 0) && (fdnew[i]) && (strcmp(user_msg, login[i]))) {

				fprintf(socket_fp[i], "%s\r\n", msg_all);
				fflush(socket_fp[i]);
			}
		}
	memset(msg_all, 0, 2048);
	}
}
	
void wait(pthread_t *pid) {
  int i;
  for(i=0; i<5; i++)
	{
		usleep(1);
		while(((fdnew[i] = accept(fd, (struct sockaddr *)&addr,&size)))<0)usleep(1);
		printf("CLIENT %d conectado\n", i);
		printf("Socket %d : %d\n", i, fdnew[i]);
		//socketopen=i;
		fflush(stdout);
		fflush(stdin);
		pthread_create(&(pid[i]),NULL, new, i);	
		pthread_create(&(pid[i+5]),NULL, envias, i);
		
	}
}

 char * dcc_chat(void) {
	pthread_t pid[10], pid_wait, pid_enviaa;


	
	
	int i=0;
	//bzero(&addr, sizeof(addr));  /* Zera a estrutura */
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(addr.sin_zero), 0, 8);
		 //bzero(&(addr.sin_zero), 8);

		 
	if((fd = socket(AF_INET, SOCK_STREAM, 0))==0)perror("socket");

	if((bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr)))==-1){ printf("bind: error\n"); return; }
	if(listen(fd, BACKLOG)==-1)perror("listen");
	size = sizeof (struct sockaddr_in);
	pthread_create(&(pid_enviaa),NULL, enviaa, NULL);
	pthread_create(&(pid_wait),NULL, wait, pid);
	
	
		 
	while(fd) sleep(1);
	for(i=0; i<5; i++) {
	  close(fdnew[i]);
	  pthread_kill(pid[i], SIGKILL);
	  pthread_kill(pid[i+5], SIGKILL);
	  close(socket_fr[i]);
	  close(socket_fp[i]);
	}
	  pthread_kill(pid_wait, SIGKILL);
	  pthread_kill(pid_enviaa, SIGKILL);
	close(fd);
	
//	:IsaqueFD!n=Isaque@201.78.14.72 PRIVMSG Xape :DCC CHAT chat 3377335880 59299
 }

