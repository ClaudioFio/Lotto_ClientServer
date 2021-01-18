#include "../header/lotto_server.h"



char* createSessionID(char sessionID[]){
	
    char alphanum[] ="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    char tmp;
    srand(time(NULL));

    int i=0;
    while (i < SESSION_SIZE) {
        int r = rand()%(strlen(alphanum)-1);     
        tmp=alphanum[r];
        sessionID[i] = tmp;
       
        i++;
    }
    sessionID[SESSION_SIZE]=0;
    return sessionID;
}



int loginClient(char* command, int client_sock){
	int ret;
	int attempts_number=0;
	char buffer_out[BUFFER_SIZE];
	char IP_address[BUFFER_SIZE];
	char* username;
	char* password;
	char sessionID[SESSION_SIZE];

	strtok(command, " ");
	username = strtok(NULL, " ");
	password = strtok(NULL, " ");

	printf("%d:-> Updating client's info...\n", pid);
	ret = getClientIP(IP_address, client_sock);
	if(ret == 1){
		printf("%d: !!! Error: loginClient() --> getClientIP()\n", pid);
		return 1;
	}

	ret = checkBlacklist(IP_address);
	if(ret == 1){
		printf("%d: !!! Error: loginClient() --> checkBlacklist()\n", pid);
		return 1;	
	}
	//IP int the Blacklist
	if(ret == 2){
		strcpy(buffer_out, "Sorry, but you cannot log yet, your IP is in the Blacklist. Wait please!");
		printf("%d:-> Login failed, the IP: %s    must wait!\n", pid, IP_address);
		ret = sendTCP(buffer_out, client_sock);
		if(ret != 0){
			printf("%d: !!! Error: loginClient() --> sendTCP()\n", pid);
			return 1;		
		}
		return 0;
	}

	ret = checkClients(username, password);

	if(ret == 1){
		printf("%d: !!! Error: loginClient() --> checkClients()\n", pid);
		return 1;	
	}	
	//Username Not found
	if(ret == 2){
		strcpy(buffer_out, "This username doen't exist!");
		printf("%d:-> Login failed, username not found!\n", pid);
		ret = sendTCP(buffer_out, client_sock);
		if(ret != 0){
			printf("%d: !!! Error: loginClient() --> sendTCP()\n", pid);
			return 1;		
		}

		return 0;
	}
	//Wrong password
	if(ret == 3){
		strcpy(buffer_out, "Password not correct");
		printf("%d:-> Login failed, Password not correct!\n", pid);
		ret = checkLogs(username, IP_address, attempts_number);
		if(ret == 4){
			printf("%d: !!! Error: loginClient() --> checkLogs()\n", pid);
			return 1;	
		}	
		attempts_number = ret;
		attempts_number++;
		ret = insertLogs(username, IP_address, attempts_number);
		if(ret == 1){
			printf("%d: !!! Error: loginClient() --> insertLogs()\n", pid);
			return 1;	
		}	
		if(attempts_number == 3){
			insertBlacklist(IP_address);
			if(ret == 1){
				printf("%d: !!! Error: loginClient() --> insertLogs()\n", pid);
				return 1;	
			}
		}

		ret = sendTCP(buffer_out, client_sock);
		if(ret != 0){
			printf("%d: !!! Error: loginClient() --> sendTCP()\n", pid);
			return 1;		
		}
		return 0;
	}
	//client already online
	if(ret == 4){
		strcpy(buffer_out, "This user is already online!");
		printf("%d:-> Login failed, the Iuser %s is already online!\n", pid, username);
		
		ret = sendTCP(buffer_out, client_sock);
		if(ret != 0){
			printf("%d: !!! Error: loginClient() --> sendTCP()\n", pid);
			return 1;		
		}
		return 0;
	}
	//The client will do the login
	if(ret == 0 ){
		ret = checkLogs(username, IP_address, attempts_number);
		if(ret == 4){
			printf("%d: !!! Error: loginClient() --> checkLogs()\n", pid);
			return 1;	
		}	
		attempts_number = 0;
		ret = insertLogs(username, IP_address, attempts_number);
		if(ret == 1){
			printf("%d: !!! Error: loginClient() --> insertLogs()\n", pid);
			return 1;	
		}	
		createSessionID(sessionID);
		ret = removeEntry(username, password);

		if(ret == 1){
			printf("%d: !!! Error: loginClient() --> removeEntry()\n", pid);
			return 1;	
		}	
		ret = insertEntry(username, password, sessionID);
		if(ret == 1){
			printf("%d: !!! Error: loginClient() --> insertEntry()\n", pid);
			return 1;	
		}
		strcpy(buffer_out, "Password correct. Wait for sessionID !");
		printf("%d:-> Login in progress, sending sessionID!\n", pid);

		ret = sendTCP(buffer_out, client_sock);
		if(ret != 0){
			printf("%d: !!! Error: loginClient() --> sendTCP()\n", pid);
			return 1;		
		}

		ret = sendTCP(sessionID, client_sock);
		if(ret != 0){
			printf("%d: !!! Error: loginClient() --> sendTCP()\n", pid);
			return 1;		
		}

		strcpy(usernameClient, username);

		strcpy(buffer_out, "Login successful!");
		printf("%d:-> Client %s successfully logged!\n", pid, username);
		ret = sendTCP(buffer_out, client_sock);
		if(ret != 0){
			printf("%d: !!! Error: loginClient() --> sendTCP()\n", pid);
			return 1;		
		}
		return 0;
	}	
	return 1;
}


int checkBlacklist(char* IP_address_in){
	int ret, filedes;
	char IP_address[BUFFER_SIZE];
	long int time_stamp;
	char final_IP_address[BUFFER_SIZE];
	long int final_time_stamp;
	
	time_t ltime; 

	FILE* fd;
    
    ltime=time(NULL);		// get current time 
    ltime-=1800;			// minus 30 mins    
	
	fd = fopen("./txt/blacklist.txt", "r");
	if(fd == NULL){
		printf("%d: !!! Error: OpenFile--> fopen()\n", pid);
		return 1;
	}
	//acquiring the lock
	filedes = fileno(fd);
	ret = fcntl(filedes, F_SETLKW, &client_rdlock);
	if(ret == -1){
		fprintf(stderr, "%d:", pid);
		perror(" !!! checkBlacklist() --> fcntl()");
		fclose(fd);
		return 1;	
	}

	while(1)
	{
		ret = fscanf(fd, "%s %ld\n",IP_address, &time_stamp );
		if(ret == EOF){
			//Blacklist completely read
			break;	
		}
		if(ret != 2){
			printf("%d: !!! Error: checkBlacklist() --> fscanf ret = %d\n", pid, ret);	
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 1;		
		}
		if(strcmp(IP_address, IP_address_in) == 0){
			//IP found
			strcpy(final_IP_address, IP_address);
			final_time_stamp = time_stamp;
		}
		//Now we have the last <IP_address, time_stamp>
	}
	if(strcmp(final_IP_address, IP_address_in) == 0){
		//IP found
		if(final_time_stamp > ltime){
			//check if 30 mins passed
			printf("%d: The client with IP: %s  must wait !!\n", pid, final_IP_address);
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 2;
		}else{
			printf("%d: The client already waited 30 mins !!\n", pid);
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 0;
		}
	}
	//IP not in the Blacklist
	printf("%d:...Client's IP not in blacklist\n", pid);
	fcntl(filedes, F_SETLKW, &client_unlock);
	fclose(fd);
	return 0;
	
}

int checkClients(char* username_in, char* password_in){
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
		perror(" !!! checkClients() --> fcntl()");
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
			return 2;		
		}
		if(ret != 3){
			printf("%d: !!! Error: checkClients() --> fscanf ret = %d\n", pid, ret);	
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 1;		
		}
		if(strcasecmp(username, username_in) == 0){
			//client found
			if(strcmp(password, password_in) == 0){
				//password correct
				if(strcmp(sessionID, "offline") == 0){
					//client offline
					printf("%d: The client almost ready for the login !!\n", pid);
					fcntl(filedes, F_SETLKW, &client_unlock);
					fclose(fd);
					return 0;
				}else{
					//client already online
					printf("%d: The client already online !!\n", pid);
					fcntl(filedes, F_SETLKW, &client_unlock);
					fclose(fd);
					return 4;
				}
			}else{
				printf("%d: Wrong password !!\n", pid);
				fcntl(filedes, F_SETLKW, &client_unlock);
				fclose(fd);
				return 3;
			}
		}
	}
}

int checkLogs(char* username_in, char* IP_address_in, int attempts_number_in){
	int ret, filedes;
	char username[BUFFER_SIZE];
	char IP_address[BUFFER_SIZE];
	long int time_stamp;
	int attempts_number;
	char final_username[BUFFER_SIZE];
	char final_IP_address[BUFFER_SIZE];
	long int final_time_stamp;
	int final_attempts_number;
	
	time_t ltime; 

	FILE* fd;
    
    ltime=time(NULL);		// get current time 
	ltime-=1800;			// minus 30 mins    

	fd = fopen("./txt/logs.txt", "r");
	if(fd == NULL){
		printf("%d: !!! Error: OpenFile--> fopen()\n", pid);
		return 4;
	}
	//acquiring the lock
	filedes = fileno(fd);
	ret = fcntl(filedes, F_SETLKW, &client_rdlock);
	if(ret == -1){
		fprintf(stderr, "%d:", pid);
		perror(" !!! checkLogs() --> fcntl()");
		fclose(fd);
		return 4;	
	}

	while(1)
	{
		ret = fscanf(fd, "%s %s %d %ld\n", username, IP_address, &attempts_number, &time_stamp );
		if(ret == EOF){
			//Client's IP not in the Blacklist
			break;		
		}
		if(ret != 4){
			printf("%d: !!! Error: checkLogs() --> fscanf ret = %d\n", pid, ret);	
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 4;		
		}
		if(strcmp(username, username_in) == 0 && strcmp(IP_address, IP_address_in) == 0){
			//found <username, IP_address>
			strcpy(final_username, username);
			strcpy(final_IP_address, IP_address);
			final_attempts_number = attempts_number;
			final_time_stamp = time_stamp;
		}
		//Now we have the last <username, IP_address, attempts_number, time_stamp>
	}

	if(strcmp(final_username, username_in) == 0){
		//reset try after IP_timeout
		if(final_attempts_number == 3){
			final_attempts_number = 0;
		}
		//user found
		if(final_time_stamp > ltime){
			//check if 30 mins passed
			printf("%d: The client <%s , %s> logged less than 30 mins before !!\n", pid, final_username, final_IP_address);
			//attempts_number = final_attempts_number;
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return final_attempts_number;
		}else{
			printf("%d: The client <%s , %s> logged more than 30 mins before !!\n", pid, final_username, final_IP_address);
			//final_attempts_number = 0;
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 0;
		}
	}	
	//<username, IP> not in the Logs
	printf("%d:...Client not found in logs\n", pid);
	attempts_number_in = 0;
	fcntl(filedes, F_SETLKW, &client_unlock);
	fclose(fd);
	return 0;
}

int insertLogs(char* username_in ,char* IP_address_in, int attempts_number_in){
	int ret, wr_len, filedes;
    time_t ltime; 
    char attempts[10];
    char tempo[20];

	FILE* fd;
    
    ltime=time(NULL);

	fd = fopen("./txt/logs.txt", "a");
	if(fd == NULL){
		printf("%d: !!! Error: insertLogs() --> fopen()\n", pid);
		return 1;
	}
	filedes = fileno(fd);
	ret = fcntl(filedes, F_SETLKW, &client_wrlock);
	if(ret == -1){
		fprintf(stderr, "%d:", pid);
		perror(" !!! insertLogs() --> fcntl()");	
		fclose(fd);	
		return 1;
	}	

	sprintf(attempts, "%d", attempts_number_in);
	sprintf(tempo, "%ld", ltime);
	
	wr_len = strlen(username_in) + strlen(IP_address_in) + strlen(attempts) + strlen(tempo);

	ret = fprintf(fd, "%s %s %s %s\n", username_in, IP_address_in, attempts, tempo);

	if( (ret != (wr_len + 4) ) || (ret < 0) ){

		printf("%d: !!! Error: insertLogs() --> fprintf() ret = %d \n", pid, ret);
		fclose(fd);
		fcntl(filedes, F_SETLKW, &client_unlock);	
		return 1;	
	}

	printf("%d:...New log for the client %s \n", pid, username_in);
	fcntl(filedes, F_SETLKW, &client_unlock);
	fclose(fd);
	return 0; 
}

int insertBlacklist(char* IP_address_in){
	int ret, wr_len, filedes;
	time_t ltime; 
	char tempo[20];

	FILE* fd;
    
    ltime=time(NULL);

	fd = fopen("./txt/blacklist.txt", "a");
	if(fd == NULL){
		printf("%d: !!! Error: insertBlacklist() --> fopen()\n", pid);
		return 1;
	}
	filedes = fileno(fd);
	ret = fcntl(filedes, F_SETLKW, &client_wrlock);
	if(ret == -1){
		fprintf(stderr, "%d:", pid);
		perror(" !!! insertBlacklist() --> fcntl()");	
		fclose(fd);	
		return 1;
	}	

	sprintf(tempo, "%ld", ltime);
	
	wr_len = strlen(IP_address_in) + strlen(tempo);
	ret = fprintf(fd, "%s %s\n", IP_address_in, tempo);
	if( ret != (wr_len + 2) || ret < 0){
		printf("%d:  !!! Error: insertBlacklist() --> fprintf() ret = %d \n", pid, ret);
		fclose(fd);
		fcntl(filedes, F_SETLKW, &client_unlock);	
		return 1;	
	}
	printf("%d:...New IP in the Blacklist!! \n", pid);
	fcntl(filedes, F_SETLKW, &client_unlock);
	fclose(fd);
	return 0; 
}