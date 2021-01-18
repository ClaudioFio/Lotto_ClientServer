#include "../header/lotto_server.h"


int invia_giocata(char* command, int client_sock){

	int ret;
	char buffer_out[BUFFER_SIZE];
	char username[BUFFER_SIZE];
	char sessionID[SESSION_SIZE];


	ret = getClientSessionID(username, sessionID, client_sock);
	if(ret == 1){
		printf("%d: !!! Error: invia_giocata() --> getClientSessionID()\n", pid);
		return 1;
	}

	//SessionID not valid
	if(ret == 2){
		strcpy(buffer_out, "Your sessionID is not valid");
		printf("%d:-> The client %s used a not valid sessionID\n", pid, username);
		ret = sendTCP(buffer_out, client_sock);
		if(ret != 0){
			printf("%d: !!! Error: invia_giocata() --> sendTCP()\n", pid);
			return 1;		
		}
		return 0;
	}



	ret = insertActualPlay(username, command);
	if(ret == 1){
		printf("%d: !!! Error: invia_giocata() --> insertActualPlay()\n", pid);
		return 1;	
	}

	strcpy(buffer_out, "The client sent a play successfully");
	printf("%d:-> Operation completed successfully\n", pid);
	ret = sendTCP(buffer_out, client_sock);
	if(ret != 0){
		printf("%d: !!! Error: invia_giocata() --> sendTCP()\n", pid);
		return 1;		
	}
	return 0;



}

int checkSessionID(char* username_in,char* sessionID_in){
	int ret, filedes;
	char username[BUFFER_SIZE];
	char password[BUFFER_SIZE];
	char sessionID[SESSION_SIZE];

	FILE* fd;
 
	fd = fopen("./txt/clients.txt", "r");
	if(fd == NULL){
		printf("%d: !!! Error: OpenFile--> fopen()\n", pid);
		return 1;
	}
	//acquiring the lock
	filedes = fileno(fd);
	ret = fcntl(filedes, F_SETLKW, &client_rdlock);
	if(ret == -1){
		fprintf(stderr, "%d:", pid);
		perror(" !!! checkSessionID() --> fcntl()");
		fclose(fd);
		return 1;	
	}

	while(1)
	{
		ret = fscanf(fd, "%s %s %s\n", username, password, sessionID );
		if(ret == EOF){
			//Client not found
			printf("%d: !!! Error: Client %s not found\n", pid, username_in);
			fcntl(filedes, F_SETLKW, &client_unlock);			
			fclose(fd);
			return 1;		
		}
		if(ret != 3){
			printf("%d: !!! Error: checkSessionID() --> fscanf ret = %d\n", pid, ret);	
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 1;		
		}
		if(strcasecmp(username, username_in) == 0){
			//client found
			if(strcmp(sessionID, sessionID_in) == 0){
				//sessionID valid
				printf("%d: Valid sessionID !!\n", pid);
				fcntl(filedes, F_SETLKW, &client_unlock);
				fclose(fd);
				return 0;
			}else{
				printf("%d: sessionID not valid\n", pid);
				fcntl(filedes, F_SETLKW, &client_unlock);
				fclose(fd);
				return 2;
			}
		}
	}
	return 0;

}

int insertActualPlay(char* username, char* play_in){
	int ret, wr_len, filedes;
	int play_len, command_len, argument_len;
	char play[BUFFER_SIZE];
	char* argument;

	FILE* fd;

	strcpy(play, play_in);
    play_len = strlen(play);

    strtok(play, " ");
    command_len=  strlen(play);//lenght of "!invia_giocata "
    play_len -= command_len + 1;

	
	//saving informations into file "clients.txt"
	fd = fopen("./txt/actual_plays.txt", "a");
	if(fd == NULL){
		printf("%d: !!! Error: insertActualPlay() --> fopen()\n", pid);
		return 1;
	}

	filedes = fileno(fd);
	ret = fcntl(filedes, F_SETLKW, &client_wrlock);

	if(ret == -1){
		fprintf(stderr, "%d:", pid);
		perror(" !!! Error: insertActualPlay() ---> fcntl()");		
		fclose(fd);	
		return 1;
	}

	wr_len = strlen(username);
	ret = fprintf(fd, "%s ", username);
	if( ret != (wr_len + 1) || ret < 0){
		printf("%d: !!! Error: insertActualPlay() --> fprintf() ret = %d \n", pid, ret);
		fcntl(filedes, F_SETLKW, &client_unlock);	
		fclose(fd);
		return 1;	
	}

	while(play_len > 0 ){

    	argument = strtok(NULL, " ");
        argument_len = strlen(argument);
        play_len -= argument_len + 1;

        wr_len = strlen(argument);
                
        if(play_len <= 0)
        {
        	ret = fprintf(fd, "%s\n", argument);
        }else{
        	ret = fprintf(fd, "%s ", argument);
        }

	    if( ret != (wr_len + 1) || ret < 0){
			printf("%d: !!! Error: insertActualPlay() --> fprintf() ret = %d \n", pid, ret);
			fcntl(filedes, F_SETLKW, &client_unlock);	
			fclose(fd);
			return 1;	
		}

	}
 
	fcntl(filedes, F_SETLKW, &client_unlock);	
	fclose(fd);

	return 0;	

}