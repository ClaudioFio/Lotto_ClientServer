#include "../header/lotto_server.h"

int executeCommand(char* command, int client_sock)
{

	char parsedCommand[BUFFER_SIZE];	
	char* argument;
	int i, ret;	
	const char commands[7][MAX_CMD_LENGTH] = {	"!signup",
											"!login",
											"!invia_giocata",
											"!vedi_giocate",
											"!vedi_estrazione",
											"!vedi_vincite",
											"!esci" };		
	// parsing string for command
	strcpy(parsedCommand, command);
	strtok(parsedCommand, " ");	
	argument = strtok(NULL, " ");

	// searching for command's index
	for(i = 0; i<7; i++){
		if( strcmp(parsedCommand , commands[i]) == 0 )
			break;	
	}
	printf("%d:-> Command issued: %s\n", pid, command); 
	switch(i){
		case 0:
			ret = signupClient(command, client_sock);
			break;
		case 1:
			ret = loginClient(command, client_sock);
			break;
		case 2:
			ret = invia_giocata(command, client_sock);
			break;
		case 3:
			ret = vedi_giocate(argument, client_sock);
			break;
		case 4:
			ret = vedi_estrazione(command, client_sock);
			break; 
		case 5:
			ret = vedi_vincite(client_sock);
			break;
		case 6:
			ret = esci(client_sock);
			break;
		default: 
			printf("%d: !!! Error: unrecognized command\n", pid);
			ret = 1;
			break;	
	} 
	return ret;
}

int receiveTCP(char* buffer_in, int sock)
{
	uint16_t buff_size;	
	int ret;
	char passwordClient[BUFFER_SIZE];

	// Acknowledging client's command length 
	ret = recv(sock, (void*)&buff_size, sizeof(uint16_t), 0);
	if(ret == 0){

		if(strlen(usernameClient) != 0){
			ret = removeEntry(usernameClient, passwordClient);
			if(ret == 1){
				printf("%d: !!! Error: receiveTCP() --> removeEntry()\n", pid);
				return 1;	
			}	
			ret = insertEntry(usernameClient, passwordClient, "offline");
			if(ret == 1){
				printf("%d: !!! Error: receiveTCP() --> insertEntry()\n", pid);
				return 1;	
			}

		}
		printf("%d:-> Client closed the connection. Closing TCP_sock.\n", pid);
		return 1;	
	}		
	if(ret == -1){
		fprintf(stderr, "%d:", pid);
		perror(" !!! Length recv(): ");
		return 1;
	}			
	if(ret != sizeof(uint16_t)){
		printf("%d: !!! Warning: received %i bytes, instead of %lu\n", pid, ret, sizeof(uint16_t));
		return 1;
	}
	
	buff_size = ntohs(buff_size);
	// Receving actual command
	memset(buffer_in, 0, BUFFER_SIZE);
	ret = recv(sock, (void*)buffer_in, buff_size, 0);
	if(ret == -1){
		fprintf(stderr, "%d:", pid);
		perror(" !!! Command recv() ");
		return 1;	
	}			
	if(ret != buff_size){
		printf("%d: !!! Warning: received %i bytes, instead of %d\n", pid, ret, buff_size);
		return 1;
	}
	return 0;
}

int sendTCP(char* buffer_out, int sock)
{
	uint16_t buff_size;	
	int ret;
	
	//sending message length
	buff_size = htons(strlen(buffer_out));
	ret = send(sock, (void*)&buff_size, sizeof(uint16_t), 0);
	if(ret == -1){
		fprintf(stderr, "%d:", pid);
		perror(" !!! Length send() ");
		return 1;	
	}			
	if(ret != sizeof(uint16_t)){
		printf("%d: !!! Warning: sent %i bytes, instead of %lu\n", pid, ret, sizeof(uint16_t));
		return 1;
	}

	//sending actual command
	buff_size = ntohs(buff_size);
	ret = send(sock, (void*)buffer_out, buff_size, 0);
	if(ret == -1){
		fprintf(stderr, "%d:", pid);
		perror(" !!! Command send() ");
		return 1;	
	}	
	if(ret != buff_size){
		printf("%d: !!! Warning: sent %i bytes, instead of %d\n", pid, ret, buff_size);
		return 1;
	}	
	return 0;
}

int insertEntry(char* username, char* password, char* sessionID)
{						
	int ret, wr_len, filedes;

	FILE* fd;

	FILE* newUserFile;
	char* extension = ".txt";
	char* path="./txt/";

	//saving informations into file "clients.txt"
	fd = fopen("./txt/clients.txt", "a");
	if(fd == NULL){
		printf("%d: !!! Error: InsertEntry() --> fopen()\n", pid);
		return 1;
	}

	filedes = fileno(fd);
	ret = fcntl(filedes, F_SETLKW, &client_wrlock);

	if(ret == -1){
		fprintf(stderr, "%d:", pid);
		perror(" !!! insertEntry() ---> fcntl()");		
		fclose(fd);	
		return 1;
	}

	wr_len = strlen(username)+ strlen(password) + strlen(sessionID);
	ret = fprintf(fd, "%s %s %s\n", username, password, sessionID);

	if( ret != (wr_len + 3) || ret < 0){
		printf("%d: !!! Error: insertEntry() --> fprintf() ret = %d \n", pid, ret);
		fcntl(filedes, F_SETLKW, &client_unlock);	
		fclose(fd);
		return 1;	
	}

	char newUsername[strlen(path)+ strlen(username)+strlen(extension)+1];

	snprintf( newUsername, sizeof( newUsername ), "%s%s%s", path, username, extension );

	newUserFile = fopen(newUsername, "a");  

	fclose(newUserFile);

	fcntl(filedes, F_SETLKW, &client_unlock);	
	fclose(fd);

	return 0;		
}

int getClientIP(char* IP_address_out, int client_sock)
{
	int ret;
	char buffer_out[BUFFER_SIZE];
	
	// first we need to know the client's IP
	strcpy(buffer_out, "Waiting for informations");		
	ret = sendTCP(buffer_out, client_sock);
	if(ret != 0){
		printf("%d: !!! Error: getClientIP() --> sendTCP()\n", pid);
		return 1;		
	}
	// receiving IP	
	ret = receiveTCP(IP_address_out, client_sock);
	if( ret != 0){
		printf("%d: !!! Error: getClientIP() --> receiveTCP()\n", pid);
		return 1;		
	}

	return 0;
}

int getClientSessionID(char* username_out, char* sessionID_out, int client_sock)
{
	int ret;
	char buffer_out[BUFFER_SIZE];
	char username[BUFFER_SIZE];
	char sessionID[BUFFER_SIZE];
	
	
	// first we need to know the client's username
	strcpy(buffer_out, "Waiting for username");		
	ret = sendTCP(buffer_out, client_sock);
	if(ret != 0){
		printf("%d: !!! Error: getClientSessionID() --> sendTCP()\n", pid);
		return 1;		
	}
	// receiving username
	ret = receiveTCP(username, client_sock);
	if( ret != 0){
		printf("%d: !!! Error: getClientSessionID() --> receiveTCP()\n", pid);
		return 1;		
	}

	strcpy(buffer_out, "Waiting for SessionID");		
	ret = sendTCP(buffer_out, client_sock);
	if(ret != 0){
		printf("%d: !!! Error: getClientSessionID() --> sendTCP()\n", pid);
		return 1;		
	}
	// receiving SessionID
	ret = receiveTCP(sessionID, client_sock);
	if( ret != 0){
		printf("%d: !!! Error: getClientSessionID() --> receiveTCP()\n", pid);
		return 1;		
	}
    strcpy(username_out, username);
    strcpy(sessionID_out, sessionID);  

	return 0;
}

int removeEntry(char* username_in, char* password_in)
{

	int ret, wr_len, filedes, filedes1;
	char username[BUFFER_SIZE];
	char password[BUFFER_SIZE];
	char sessionID[SESSION_SIZE];
	
	FILE *fd, *fd1;
	
	//old file, to be eliminated after copy
	fd = fopen("./txt/clients.txt", "r");
	if(fd == NULL){
		fprintf(stderr, "%d:", pid);
		perror(" !!! removeEntry() --> fopen(fd)");
		return 1;
	}
            

	//acquiring the lock
	filedes = fileno(fd);
	ret = fcntl(filedes, F_SETLKW, &client_rdlock);
	if(ret == -1){
		fprintf(stderr, "%d:", pid);
		perror(" !!! removeEntry() --> fcntl(filedes2)");
		fclose(fd);
		return 1;
	}

	//new file
	fd1 = fopen("./txt/tmp_clients.txt", "w");
	if(fd1 == NULL){
		fprintf(stderr, "%d:", pid);
		perror(" !!! removeEntry() --> fopen(fd1)");
		fcntl(filedes, F_SETLKW, &client_unlock);		
		fclose(fd);
		return 1;
	}
	filedes1 = fileno(fd1);	
	ret = fcntl(filedes1, F_SETLKW, &client_wrlock);
	if(ret == -1){
		fprintf(stderr, "%d:", pid);
		perror(" !!! removeEntry() --> fcntl(fildes1)");	
		fcntl(filedes, F_SETLKW, &client_unlock);		
		fclose(fd);	
		fclose(fd1);
		return 1;
	}

	while(1)
	{
		ret = fscanf(fd, "%s %s %s\n", username, password, sessionID );
		if(ret == EOF){
			break;	
		}		

		if(ret != 3){
			fcntl(filedes, F_SETLKW, &client_unlock);
			fcntl(filedes1, F_SETLKW, &client_unlock);
			fclose(fd);
			fclose(fd1);
			return 1;		
		}
		if(strcmp(username_in, username) == 0){
			strcpy(password_in, password);
			continue;		
		}

		wr_len = strlen(username)+ strlen(password) + strlen(sessionID);            
		ret = fprintf(fd1, "%s %s %s\n", username, password, sessionID );
			
		if( ret != (wr_len + 3) || ret < 0){
			printf("%d: !!! Error: removeEntry --> fprintf(): ret = %d \n",pid, ret);
			fcntl(filedes, F_SETLKW, &client_unlock);
			fcntl(filedes1, F_SETLKW, &client_unlock);
			fclose(fd);
			fclose(fd1);
			return 1;	
		}	
	}


	fcntl(filedes, F_SETLKW, &client_unlock);
	fcntl(filedes1, F_SETLKW, &client_unlock);
	fclose(fd);
	fclose(fd1);
	//removing old file
	if( remove( "./txt/clients.txt" ) != 0 ){
		fprintf(stderr, "%d:", pid);
    	perror( " !!! Error deleting old clients.txt" );
	}
  	else
    	printf("%d:...Old clients.txt successfully deleted\n", pid );
	
	//renaming new file as old file
 	if ( rename ("./txt/tmp_clients.txt", "./txt/clients.txt") != 0 ){
		fprintf(stderr, "%d:", pid);
   		perror( " !!! Error renaming tmp_clients.txt" );
	}
	else
		printf("%d:...tmp_clients.txt successfully renamed\n", pid);


  	return 0;
}