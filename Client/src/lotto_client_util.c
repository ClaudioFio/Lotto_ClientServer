#include "../header/lotto_client.h"

void showMenu()
{
	printf(" *******************  LOTTO  ******************* \n");
	printf("The following commands are available to be executed: \n\n");
	printf(" -- !help <comamand> --> shows the command's details.\n");
	printf(" -- !signup <username> <password> --> registers a new user.\n");
	printf(" -- !login <username> <password> --> logs an existing user.\n");
	printf(" -- !invia_giocata <g> --> sends the lotto tab <g> to the server.\n");
	printf(" -- !vedi_giocate <tipe> --> shows the history of your plays.\n");		
	printf("\t When <tipe> = 0 shows your previous plays,\n");
	printf("\t When <tipe> = 1 shows your actual plays that will be extract soon.\n");
	printf(" -- !vedi_estrazione <n> <ruota> --> shows the last <n> extraction for each city <ruota>.\n");
	printf(" -- !vedi_vincite --> shows the history of your wins.\n");
	printf(" -- !esci --> disconnects from server.\n\n");

	return;
}

int requestCommand(char* command, int TCP_sock, char* IP_address)
{
	char parsedCommand[BUFFER_SIZE];	
	char* argument;
	char* pwdargument;
	int i, ret;	
	const char commands[7][MAX_COMM_LENGTH] = {	"!signup",
											"!login",
											"!invia_giocata",
											"!vedi_giocate",
											"!vedi_estrazione",
											"!vedi_vincite",
											"!esci" };	

	//removing newline character at end of command
	strtok(command, "\n");

	strcpy(parsedCommand, command);

	//parsing string for command and argument	
	strtok(parsedCommand, " ");
	argument = strtok(NULL, " ");
	pwdargument = strtok(NULL, " ");
	
	// searching for command index
	for(i = 0; i<7; i++){
		if( strcmp(parsedCommand, commands[i]) == 0 )
			break;	
	}
	switch(i){
		case 0: // signup
			if(argument == NULL ){
				printf("Missing username\n");
				ret = 0;
				break;
			}
			if( (strcmp(argument, "tmp_clients") == 0) || ( strcmp(argument, "tmp_past_extraction") == 0) ){
				printf("Username not valid\n");
				ret = 0;
				break;
			}
			if(strcmp(username, "") != 0){
				printf("You need to be unregistered to issue this command\n");
				ret = 0;
				break;			
			}
			if(pwdargument == NULL){
				printf("Missing password\n");
				ret = 0;
				break;
			}
			ret = requestSignup(command, TCP_sock, IP_address);
			break;
		case 1: // login
			if(argument == NULL){
				printf("Missing username\n");
				ret = 0;
				break;
			}
			if(strcmp(username, "") != 0){
				printf("You need to be disconnected to issue this command\n");
				ret = 0;
				break;			
			}
			if(pwdargument == NULL){
				printf("Missing password\n");
				ret = 0;
				break;
			}
			ret = requestLogin(command, TCP_sock, IP_address);
			break;
		case 2:	// invia_giocata
			if(strcmp(username, "") == 0){
				printf("You need to be connected to issue this command\n");
				ret = 0;
				break;
			}
			ret = requestInvia_giocata(command, TCP_sock);
			break;

		case 3: // vedi_giocate
			if(strcmp(username, "") == 0){
				printf("You need to be registered to issue this command\n");
				ret = 0;
				break;			
			}	
			if(argument == NULL){
				printf("Missing argument\n");
				ret = 0;
				break;
			}		
			ret = requestVedi_giocate(command, TCP_sock);
			break;
		case 4:	// vedi_estrazione
			if(strcmp(username, "") == 0){
				printf("You need to be connected to issue this command\n");
				ret = 0;
				break;			
			}
			if(argument == NULL){
				printf("Missing number of extraction\n");
				ret = 0;
				break;
			}
				ret = requestVedi_estrazione(command, TCP_sock);
			break;
		case 5:	// vedi_vincite
			if(strcmp(username, "") == 0){
				printf("You need to be connected to issue this command\n");
				ret = 0;
				break;			
			}
			ret = requestVedi_vincite(command, TCP_sock);
			break;
		case 6:	// esci
			if(strcmp(username, "") == 0){
				printf("You need to be connected to issue this command\n");
				ret = 0;
				break;			
			}
			ret = requestEsci(command, TCP_sock);
			break;
		default:
			printf("Unrecognized command\n");
			ret = 0;
			break;	
	}

	return ret;
}

int receiveTCP(char* buffer_in, int sock)
{
	uint16_t buff_size;	
	int ret;

	// Acknowledging client's command length 
	ret = recv(sock, (void*)&buff_size, sizeof(uint16_t), 0);
	if(ret == 0){
		printf("Server closed the connection. Closing client_sock.\n");
		return 1;	
	}		
	if(ret == -1){
		perror("Length recv()");
		return 1;
	}			
	if(ret != sizeof(uint16_t)){
		printf("Warning: received %i bytes, instead of %lu\n", ret,sizeof(uint16_t));
		return 1;
	}
	
	buff_size = ntohs(buff_size);
	// Receving actual command
	memset(buffer_in, 0, BUFFER_SIZE);
	ret = recv(sock, (void*)buffer_in, buff_size, 0);
	if(ret == -1){
		perror("Command recv()");
		return 1;	
	}			
	if(ret != buff_size){
		printf("Warning: received %i bytes, instead of %d\n", ret, buff_size);
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
		perror("Length send()");
		return 1;	
	}			
	if(ret != sizeof(uint16_t)){
		printf("Warning: sent %i bytes, instead of %lu\n", ret, sizeof(uint16_t));
		return 1;
	}

	//sending actual command
	buff_size = ntohs(buff_size);
	ret = send(sock, (void*)buffer_out, buff_size, 0);
	if(ret == -1){
		perror("Command send()");
		return 1;	
	}			
	if(ret != buff_size){
		printf("Warning: sent %i bytes, instead of %d\n", ret, buff_size);
		return 1;
	}	
	return 0;
}