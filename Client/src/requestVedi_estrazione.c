#include "../header/lotto_client.h"

int requestVedi_estrazione(char* command_in, int TCP_sock)
{
	int ret, command_len, argument_len, number;
	char server_reply[BUFFER_SIZE];
	char command[BUFFER_SIZE];
	char* numberExtraction;
	char* City;

	strcpy(command, command_in);
	command_len = strlen(command);

	strtok(command, " ");// !vedi_estrazione
	argument_len=strlen(command);
	command_len -= argument_len + 1;

	numberExtraction = strtok(NULL, " ");// n
	argument_len = strlen(numberExtraction);
	command_len -= argument_len + 1;

	number = atoi(numberExtraction);
	if (number <= 0){	
		printf("Uncorrect number\n");
		return 0;
	}

	if(command_len > 0 ){	//Check if there is the third argument
		City = strtok(NULL, " ");
		ret = checkCity(City);
	 	if(ret == 1){
	 		printf("Uncorrect city\n");
	 		return 0;
	 	}
	}
		

	ret = sendTCP(command_in, TCP_sock);
	if(ret != 0){
		printf("Error requestVedi_estrazione --> sendTCP()\n");
		return 1;		
	}

	ret = receiveTCP(server_reply, TCP_sock);
	if(ret != 0){
		printf("Error requestVedi_estrazione --> receiveTCP()\n");
		return 1;		
	}
	
	if( strcmp(server_reply, "Waiting for username") == 0 )
	{
		//send username
		ret = sendTCP(username, TCP_sock);
		if(ret != 0){
			printf("Error requestVedi_estrazione --> sendTCP()\n");
			return 1;		
		}

		ret = receiveTCP(server_reply, TCP_sock);
		if(ret != 0){
			printf("Error requestVedi_estrazione --> receiveTCP()\n");
			return 1;		
		}

		if( strcmp(server_reply, "Waiting for SessionID") == 0 )
		{
			//send sessionID
			ret = sendTCP(sessionID, TCP_sock);
			if(ret != 0){
				printf("Error requestVedi_estrazione --> sendTCP()\n");
				return 1;		
			}
	
		}

		while(1){
			memset(server_reply, 0, BUFFER_SIZE);
			ret = receiveTCP(server_reply, TCP_sock);
			if(ret != 0){
				printf("Error requestVedi_estrazione --> receiveTCP()\n");
				return 1;		
			}
        
			if(strcmp(server_reply, "End of extractions") == 0 ){
				printf("%s\n", server_reply);

				return 0;
			}
			 
			printf("%s\n", server_reply);

		}
		return 0;
	}

	printf("Error requestVedi_estrazione()\n");
	return 1;

}
