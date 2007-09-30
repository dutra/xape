#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include "config.h"

//#define MYPORT 27015    // the port users will be connecting to

char * infocs(char *user, char *sv, int porta, char *canalg)
{
	
	int sockfd;
	struct sockaddr_in their_addr; // connector's address information
	struct hostent *he;
	struct timeval tv;
	int numbytes, ipsv=0;
	char text[1024], *cptext=0, ip[1024], namesv[1024], map[1024], sv2[128];
	FILE *socket_fr = 0;
	int tam_cliaddr, i, parser=0, retval;
	fd_set rfds;
	if(!porta)porta=0;

	
	//strcpy(sv, "201.79.173.242");

	strcpy(sv2, sv);
	
	for(i=3;i<128;i++)sv2[i]='\0';
	

	if(atoi(sv2)){ printf("1"); their_addr.sin_addr.s_addr = inet_addr(sv); }
	else {
	//struct sockaddr_in cliaddr
	if ((he = gethostbyname(sv)) == NULL) {  // get the host info
	memset(text, 0, 1024);
	snprintf(text, 1024, "PRIVMSG %s :%s, Host not found\r\n", canalg, user);
	cptext = text;
	return cptext;

	}
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	}
	if ((sockfd = socket (AF_INET, SOCK_DGRAM, 0)) == -1) {
	memset(text, 0, 1024);
	snprintf(text, 1024, "PRIVMSG %s :%s, Host not found\r\n", canalg, user);
	cptext = text;
	return cptext;

}

	their_addr.sin_family = AF_INET;     // host byte order
	their_addr.sin_port = htons(porta); // short, network byte order

	memset(&(their_addr.sin_zero), '\0', 8);  // zero the rest of the struct

	

	snprintf(text, sizeof(text), "ÿÿÿÿTSource Engine Query");
	
	     if ((numbytes = sendto (sockfd, text, strlen(text), 0, (struct sockaddr *) &their_addr, sizeof(struct sockaddr))) == -1) {
		     return;
	     }

	     printf("sent %d bytes to %s\n", numbytes, inet_ntoa (their_addr.sin_addr));
	             

		     memset(text, 0, 1024);
		     tam_cliaddr = sizeof(their_addr);

				 FD_ZERO(&rfds);
				 FD_SET(sockfd, &rfds);
				 tv.tv_sec = 2;
				 tv.tv_usec = 0;
				 retval = select(1, &rfds, NULL, NULL, &tv);

				 if(retval == -1) {
					 memset(text, 0, 1024);
					 snprintf(text, 1024, "PRIVMSG %s :%s, Host isn't a CounterStrike's Server\r\n", canalg, user, ip);
					 cptext = text;
					 return cptext;
				 }
				 else
					 if(retval) {

		     recvfrom(sockfd,text,1023,0,(struct sockaddr *)&their_addr, &tam_cliaddr);
		     
		     
		     cptext = (text+5);
		     while(1) {
			     if(!parser){
				     for(i=0;i<1024;i++) {
				     	ip[i]=*cptext;
					cptext++;
				     	if(*cptext=='\0') { cptext++; parser++; break; }
					
			     	     }
				     
			     }
			     if(parser==1) { 
				     for(i=0;i<1024;i++) {
					     namesv[i]=*cptext;
					     cptext++;
					     if(*cptext=='\0') { cptext++; parser++; break; }
					     
				     }
			
		     }
		     
		     if(parser==2) { 
			     for(i=0;i<1024;i++) {
				     map[i]=*cptext;
				     cptext++;
				     if(*cptext=='\0') { cptext++; parser++; break; }
					     
			     }
			
		     }
		     if(parser==3)break;
	}
		     
	close(sockfd);
	memset(text, 0, 1024);
	snprintf(text, 1024, "PRIVMSG %s :%s, Ip: %s - Title: %s - Map: %s\r\n", canalg, user, ip, namesv, map);
	cptext = text;
	return cptext;
					 }
					 else {
						 memset(text, 0, 1024);
						 snprintf(text, 1024, "PRIVMSG %s :%s, Host isn't a CounterStrike's Server\r\n", canalg, user);
						 cptext = text;
						 return cptext;
					 }
				 close(sockfd);
}
