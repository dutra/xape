#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

int search_db(char *login_s, char *passwd_s) {
	struct db_t {
		char login[255];
		char passwd[255];
	} db;
	int status=0;
	FILE *arq;
	arq = fopen(FILEUSERS, "rb");
	if(arq == NULL)
		exit(1);

	while(fread(&db, sizeof(db), 1, arq))
		if(!(strcmp(login_s, db.login)))
			if(!(strcmp(passwd_s, db.passwd))) {
				status=1;
				break;
			}
			else {
				status=0;
				break;
			}
		else
			status=-1;
	fclose(arq);
	return status;
}

int adduser(char *login, char *passwd) {

	struct db_t {
		char login[255];
		char passwd[255];
		} db;
		FILE *arq;
		
		strcpy(db.login, login);
		strcpy(db.passwd, passwd);
		arq = fopen(FILEUSERS, "a+b");
		if(arq == NULL)
			exit(1);
		fwrite(&db, sizeof(db), 1, arq);
		fclose(arq);
}


void save_log(char *line) {
	FILE *arq;
	if(!(arq = fopen(DIRLOG "log_" CHANNEL ".txt", "a+"))) { printf("error: open log file\n"); return; }
	fprintf(arq, "%s", line);
	//	printf("%s\n", line);
	fflush(arq);
	close(arq);
}
