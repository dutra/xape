#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "infocs.h"
#include "config.h"
#include "dcc.h"
#include "wiki.h"
#include "google.h"

extern FILE *socket_fp, *socket_fr;
extern int pvtall, fd;
char msgpvt[1024];
void parser_feedline(char *line) {
	char *comm = 0, *user=0, user2[1026], *canalg, canaltmp[1026], pingtmp[1026], *ping;
	int i;
	strcpy(canaltmp,line);
	canalg = strtok(canaltmp, " ");
	canalg = strtok(0, " \n");
	canalg = strtok(0, " \n");
	//	if(canalg==NULL) return;
	strcpy(user2, line);
	strcpy(pingtmp, line);
	ping = strtok(pingtmp, " ");
	//	if(ping==NULL) return;
	printf("\nPING: %s", ping);
	for(i=0; i<1026 && ping[i]; i++)
		printf("%d ", ping[i]);
	user = (strtok(user2, "!\r\n")+1);
	comm = (strtok(line, " \r\n"));
	//	if(comm == NULL) return;
	comm = (strtok(0, " \r\n"));
	printf("\nCOMM: %s\n", comm);
	for(i=0; i<1026 && comm[i]; i++)
		printf("%d ", comm[i]);

	//	if(comm == NULL) return;
	if(!strcmp(comm, "PRIVMSG")) {
		
		comm = (strtok(0, ":\r\n"));
		if(comm == NULL) return;
		comm = (strtok(0, " \r\n"));
		if(comm == NULL) return;
		printf("PRIVMSG> %s", comm);
		parser_functions(comm, user, canalg);
		}
	/*		if(!strcmp(comm, "PING")) {
			strtok(0, ":");
			fprintf(socket_fp, "PONG :%s\r\n", strtok(0, "\r\n"));
			fflush(socket_fp);
			}*/
		if(!strcmp(comm, "JOIN")) {
			if(pvtall) {
				fprintf(socket_fp, "PRIVMSG %s :%s\r\n", user, msgpvt);
				fflush(socket_fp);	
			}
		}
		if(!strcmp(ping, "PING")) {
			printf("PONG\n");
			fprintf(socket_fp, "PONG :%s\r\n", strtok(0, "\r\n"));
			fflush(socket_fp);
		}
				
}

void parser_functions(char *comm, char *user, char *canalg) {
	
	if(!strcmp(comm, "@@")) {
		if(!strcmp(user, ADMIN)) {
			comm = (strtok(0, "[\r\n"));
			if(comm == NULL) return;
		
			if(!strcmp(comm, "join")) {
			
			
				comm = strtok(0, "]\r\n");
						
				fprintf(socket_fp, "JOIN %s\r\n", comm);
				fflush(socket_fp);
			}
			if(!strcmp(comm, "part")) {
			
			
				comm = strtok(0, "]\r\n");
						
				fprintf(socket_fp, "PART %s\r\n", comm);
				fflush(socket_fp);
			}
			if(!strcmp(comm, "pvtall")) {
				char lig[8];
			
				comm = strtok(0, ",\r\n");
				strcpy(lig,comm);

				if(!strcmp(lig, "on")) {
					comm = strtok(0, "]\r\n");
					fprintf(socket_fp, "PRIVMSG %s :%s, PVTALL Enabled! - %s\r\n", canalg, user, comm);
					fflush(socket_fp);
					strcpy(msgpvt, comm);
					pvtall=1;
				}
				else {
					fprintf(socket_fp, "PRIVMSG %s :%s, PVTALL Disabled!\r\n", canalg, user);
					fflush(socket_fp);
					pvtall=0;
				}
				printf("pvtall %s", comm);
			}
			if(!strcmp(comm, "msg")) {
				char dest[255];
				comm = strtok(0, ",\r\n");
				strcpy(dest, comm);
				comm = strtok(0, "]\r\n");
				fprintf(socket_fp, "PRIVMSG %s :%s\r\n", dest, comm);
				fflush(socket_fp);
			}
			if(!strcmp(comm, "setop")) {
				char canal[128];
			
				comm = strtok(0, ",\r\n");
				strcpy(canal, comm);
				comm = strtok(0, "]\r\n");
				fprintf(socket_fp, "MODE %s +o %s\r\nPRIVMSG %s :%s, %s is OP\r\n",canal,comm,canalg,user,comm);
				fflush(socket_fp);
			}
			if(!strcmp(comm, "deop")) {
				char canal[128];
			
				comm = strtok(0, ",\r\n");
				strcpy(canal, comm);
				comm = strtok(0, "]\r\n");
				fprintf(socket_fp, "MODE %s -o %s\r\nPRIVMSG %s :%s, %s isn't OP\r\n",canal,comm,canalg,user,comm);
				fflush(socket_fp);
			}
			if(!strcmp(comm, "setvoice")) {
				char canal[128];
			
				comm = strtok(0, ",\r\n");
				strcpy(canal, comm);
				comm = strtok(0, "]\r\n");
				fprintf(socket_fp, "MODE %s +v %s\r\nPRIVMSG %s :%s, %s is Voice\r\n",canal,comm,canalg,user,comm);
				fflush(socket_fp);
			}
			if(!strcmp(comm, "devoice")) {
				char canal[128];
			
				comm = strtok(0, ",\r\n");
				strcpy(canal, comm);
				comm = strtok(0, "]\r\n");
				fprintf(socket_fp, "MODE %s -v %s\r\nPRIVMSG %s :%s, %s isn't Voice\r\n",canal,comm,canalg,user,comm);
				fflush(socket_fp);
			}
			if(!strcmp(comm, "kick")) {
				char canal[128],userkick[128];
			
				comm = strtok(0, ",\r\n");
				strcpy(canal, comm);
				comm = strtok(0, ",\r\n");
				strcpy(userkick, comm);
				comm = strtok(0, "]\n");
				fprintf(socket_fp, "KICK %s %s %s\r\n",canal,userkick,comm);
				fflush(socket_fp);
			}
			if(!strcmp(comm, "quit")) {
				char canal[128],userkick[128];
				comm = strtok(0, "]\r\n");
				fprintf(socket_fp, "QUIT :%s\r\n",comm);
				printf("quit: %s", comm);
				fflush(socket_fp);
				//			fclose(socket_fp);
				//			fclose(socket_fr);
				//			fclose(fd);
				//			printf("\nQUIT\n");
			}
			
			if(!strcmp(comm, "adduser")) {
			char login[255], passwd[255];
			comm = strtok(0, ",\n");
			snprintf(login, 255, "%s", comm);
			comm = strtok(0, "]\n");
			snprintf(passwd, 255, "%s", comm);
			adduser(login, passwd);
			fprintf(socket_fp, "PRIVMSG %s :%s, User successfully added\r\n", canalg, user);
			fflush(socket_fp);
		}
		}
		
		else {
			fprintf(socket_fp, "PRIVMSG %s :%s, Your aren't my administrator!\r\n", canalg, user);
			fflush(socket_fp);
		}
	}
	

	if(!strcmp(comm, "!!")) {
		printf("\n\n!!\n\n");
		comm = (strtok(0, "[\r\n"));
		if(comm == NULL) return;
		
		if(!strcmp(comm, "quad")) {
			comm = (strtok(0, "]\r\n"));
			if(comm == NULL) return;
			printf("%s", comm);
			fprintf(socket_fp, "PRIVMSG %s :%s, Quad %ld is %ld\r\n", canalg, user,  atoi(comm), (atol(comm)*atol(comm)));
			fflush(socket_fp);
				
		}
		if(!strcmp(comm, "ver")) {
			fprintf(socket_fp, "PRIVMSG %s :%s, Xape BotIrc version %s - by IsaqueFD\r\n", canalg, user, VERSION);
			fflush(socket_fp);
		}
		if(!strcmp(comm, "infocs")) {
			char sv[128];
			int port;
			comm = (strtok(0, ":"));
			if(comm == NULL) {
				printf("comm\n");
				fprintf(socket_fp, "PRIVMSG %s :%s, You forget the host, please, try again\r\n", canalg, user);
				fflush(socket_fp);
				return;
				}

			strcpy(sv, comm);
			comm = (strtok(0, "]"));
			if(comm == NULL) {
				printf("comm\n");
				fprintf(socket_fp, "PRIVMSG %s :%s, You forget the port, please, try again\r\n", canalg, user);
				fflush(socket_fp);
				return;
				}			
			port = atoi(comm);
			fprintf(socket_fp, "%s", infocs(user, sv, port, canalg));
			fflush(socket_fp);
		}
		if(!strcmp(comm, "wiki")) {
			char key[128], stringr[4096];
			int i=0, j=0;
			comm = (strtok(0, "]\r\n"));
			if(comm == NULL) {
				printf("comm\n");
				fprintf(socket_fp, "PRIVMSG %s :%s, You forget the key, please, try again\r\n", canalg, user);
				fflush(socket_fp);
				return;		
			}
			strcpy(key, comm);
			wiki(key, stringr);
			if(strlen(stringr) > 450)
				for(i=0; i<strlen(stringr); i+=450) {
					fprintf(socket_fp, "PRIVMSG %s :%s, ", canalg, user);
					for(j=0; j<450; j++)
						fprintf(socket_fp, "%c", stringr[j+i]);
					fprintf(socket_fp, "\r\n");
				}
			else
					fprintf(socket_fp, "PRIVMSG %s :%s, %s\r\n", canalg, user, stringr);
			fflush(socket_fp);
			
		}
		/*if(!strcmp(comm, "google")) {
			char key[128];
			comm = strtok(0, "]\r\n");
			strcpy(key, comm);
			fprintf(socket_fp, "PRIVMSG %s :%s, Mod Google On with key %s\r\n", canalg, user, key);
			fflush(socket_fp);
			google(key);
			}*/
		if(!strcmp(comm, "dccchat")) {
			static int lig=0;
			
			pthread_t pid;
			if(!lig)pthread_create(&(pid),NULL, dcc_chat, 0);
			lig = 1;
			char *p1, *p2, *p3, *p4, ip[16], ipa[13];
			int ip1, ip2, ip3, ip4;
			strcpy(ip, IP);
			p1=strtok(ip, ".");
			p2=strtok(0, ".");
			p3=strtok(0, ".");
			p4=strtok(0, "\0");
			ip1=atoi(p1);
			ip2=atoi(p2);
			ip3=atoi(p3);
			ip4=atoi(p4);
			ip1 = ip1<<24;
			ip2=ip2<<16;
			ip3=ip3<<8;
			ip1=(ip1|ip2|ip3|ip4);
			if(!strcmp(user, ADMIN))fprintf(socket_fp, "PRIVMSG %s :%cDCC CHAT chat 2130706433 20000%c%c%c\r\n",user,1,1,1,13,10);
			
			else fprintf(socket_fp, "PRIVMSG %s :%cDCC CHAT chat %d 20000%c%c%c\r\n",user,1,ip1,1,13,10);
			
			fflush(socket_fp);
		}
		
	}
}
