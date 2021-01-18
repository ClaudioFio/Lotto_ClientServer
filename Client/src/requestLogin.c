#include "../header/lotto_client.h"

int requestLogin(char* command, int TCP_sock, char* IP_address){

	int ret;
	char server_reply[BUFFER_SIZE];
	char* argument;
	char tmp_sessionID[SESSION_SIZE];

	//sending command <!signup username password>
	ret = sendTCP(command, TCP_sock);
	if(ret != 0){
		printf("Error requestLogin --> sendTCP(1)\n");
		return 1;		
	}
	//waiting the reply from the server to give the IP
	ret = receiveTCP(server_reply, TCP_sock);
	if(ret != 0){
		printf("Error requestLogin --> receiveTCP(1)\n");
		return 1;		
	}
	//if the client is already registered
	if( strcmp(server_reply, "Waiting for informations") == 0 )
	{
		//send ip 
		ret = sendTCP(IP_address, TCP_sock);
		if(ret != 0){
			printf("Error requestLogin --> sendTCP(2)\n");
			return 1;		
		}
		
		//if the client has to wait because of the wrong password 
		//or the client is getting the sessionID
		ret = receiveTCP(server_reply, TCP_sock);
		if(ret != 0){
			printf("Error requestLogin --> receiveTCP(2)\n");
			return 1;		
		}
		
		if(strcmp(server_reply, "Password correct. Wait for sessionID !") == 0){
			//receive sessionID
			ret = receiveTCP(tmp_sessionID, TCP_sock);
			if(ret != 0){
				printf("Error requestSignup --> receiveTCP(3)\n");
				return 1;	
			}	
			//receive the reply from the server(Login succes)
			ret = receiveTCP(server_reply, TCP_sock);
			if(ret != 0){
				printf("Error requestSignup --> receiveTCP(4)\n");
				return 1;	
			}	
			// initializing global variable "username", "sessionID"
			strtok(command, "\n");
			strtok(command, " ");
			argument = strtok(NULL, " ");	
			strcpy(username, argument);	
			strcpy(sessionID, tmp_sessionID);				
		}	
	}	

	
	printf("%s\n", server_reply);
	return 0;	
}
