#include "../header/lotto_server.h"

int vedi_estrazione(char* command_in, int client_sock){
	int ret, command_len, argument_len;
	char buffer_out[BUFFER_SIZE];
	char command[BUFFER_SIZE];
	char username[BUFFER_SIZE];
	char sessionID[SESSION_SIZE];
	char* numberExtraction;
	char* City;


	ret = getClientSessionID(username, sessionID, client_sock);
	if(ret == 1){
		printf("%d: !!! Error: vedi_vincite() --> getClientSessionID()\n", pid);
		return 1;
	}
	//SessionID not valid
	if(ret == 2){
		strcpy(buffer_out, "Your sessionID is not valid");
		printf("%d:-> The client %s used a not valid sessionID\n", pid, username);
		ret = sendTCP(buffer_out, client_sock);
		if(ret != 0){
			printf("%d: !!! Error: vedi_vincite() --> sendTCP()\n", pid);
			return 1;		
		}
		return 0;
	}
 
	strcpy(command, command_in);
	command_len = strlen(command);

	strtok(command, " ");// !vedi_estrazione
	argument_len=strlen(command);
	command_len -= argument_len + 1;

	numberExtraction = strtok(NULL, " ");// n
	argument_len = strlen(numberExtraction);
	command_len -= argument_len + 1;
	if(command_len > 0 ){	//Check if there is the third argument
		City = strtok(NULL, " ");
	}else{
		City = "Tutte";
	} 
	ret = printExtraction(numberExtraction, City, username, client_sock);
	if(ret == 1){
		printf("%d: !!! Error: vedi_estrazione() --> printExtraction()\n", pid);
		return 1;	
	}
	return 0;		
}

int printExtraction(char* numberExtraction_in, char* City, char* username, int client_sock){

	int ret, filedes;
	int extractionRowsNumber=0;
	int numberExtraction;
	char number0[2];
	char number1[2];
	char number2[2];
	char number3[2];
	char number4[2]; 
	char city[BUFFER_SIZE];
	char buffer_out[BUFFER_SIZE];
	char tmp_buffer_out[BUFFER_SIZE];
	memset(buffer_out, 0, BUFFER_SIZE);
	memset(tmp_buffer_out, 0, BUFFER_SIZE);

	FILE* fd;

	fd = fopen("./txt/past_extraction.txt", "r"); 

	if(fd == NULL){
		printf("%d: !!! Error: printExtraction() --> fopen()\n", pid);
		return 1;
	}

	filedes = fileno(fd);
	ret = fcntl(filedes, F_SETLKW, &client_rdlock);
	if(ret == -1){
		fprintf(stderr, "%d:", pid);
		perror(" !!! printExtraction() ---> fcntl()");		
		fclose(fd);	
		return 1;
	} 

	numberExtraction = atoi(numberExtraction_in);
	while(numberExtraction > 0)
	{ 
		ret = fscanf(fd, "%s %s %s %s %s %s\n", city, number0, number1, number2, number3, number4 );
		if(ret == EOF){
			printf("%d: No more extractions\n", pid);
			break;		
		}
 
		if(ret != 6){
			printf("%d: !!! Error: printExtraction() --> fscanf ret = %d\n", pid, ret);	
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 1;		
		}

		if(strcasecmp(City, "Tutte") == 0){
			snprintf(tmp_buffer_out, BUFFER_SIZE, "%s%s    \t%s %s %s %s %s\n", buffer_out, city, number0, number1, number2, number3, number4 );
			strcpy(buffer_out, tmp_buffer_out);
			extractionRowsNumber++;

			if(extractionRowsNumber == 11){//all cities in buffer_out, send them
				extractionRowsNumber = 0;
				numberExtraction--;
				ret = sendTCP(buffer_out, client_sock);
				memset(buffer_out, 0, BUFFER_SIZE);
				memset(tmp_buffer_out, 0, BUFFER_SIZE);
				if(ret != 0){
					printf("%d: !!! Error: printExtraction() --> sendTCP()\n", pid);
					fcntl(filedes, F_SETLKW, &client_unlock);
					fclose(fd);
					return 1;
				}
			}
		}else{//if the client specified a  City
			
			if (strcasecmp(City, city) == 0){
				snprintf(tmp_buffer_out, BUFFER_SIZE, "%s%s    \t%s %s %s %s %s\n", buffer_out, city,  number0, number1, number2, number3, number4 );
				strcpy(buffer_out, tmp_buffer_out);
				extractionRowsNumber++;	
				numberExtraction--;

				if(extractionRowsNumber == 11){ // send 11 rows
					extractionRowsNumber = 0;
					ret = sendTCP(buffer_out, client_sock);
					memset(buffer_out, 0, BUFFER_SIZE);
					memset(tmp_buffer_out, 0, BUFFER_SIZE);
					if(ret != 0){
						printf("%d: !!! Error: printExtraction() --> sendTCP()\n", pid);
						fcntl(filedes, F_SETLKW, &client_unlock);
						fclose(fd);
						return 1;		
					}
				}
			}else{
				continue;
			}
		}
	}
	fcntl(filedes, F_SETLKW, &client_unlock);
	fclose(fd);

	if( strlen(buffer_out) != 0){
		ret = sendTCP(buffer_out, client_sock);
		if(ret != 0){
			printf("%d: !!! Error: printExtraction() --> sendTCP()\n", pid);
			return 1;		
		}
	}

	printf("%d:-> The client %s has received the extractions\n", pid, username);

  	strcpy(buffer_out, "End of extractions");
    
	ret = sendTCP(buffer_out, client_sock);
	if(ret != 0){
		printf("%d: !!! Error: printExtraction() --> sendTCP()\n", pid);
		return 1;		
	}
	return 0;
}
