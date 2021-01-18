#include "../header/lotto_client.h"

int requestEsci(char* command, int TCP_sock)
{
	int ret;
	char server_reply[BUFFER_SIZE];

	ret = sendTCP(command, TCP_sock);
	if(ret != 0){
		printf("Error requestEsci --> sendTCP()\n");
		return 1;		
	}

	ret = receiveTCP(server_reply, TCP_sock);
	if(ret != 0){
		printf("Error requestEsci --> receiveTCP()\n");
		return 1;		
	}
	
	if( strcmp(server_reply, "Waiting for username") == 0 )
	{
		//send username
		ret = sendTCP(username, TCP_sock);
		if(ret != 0){
			printf("Error requestEsci --> sendTCP()\n");
			return 1;		
		}

		ret = receiveTCP(server_reply, TCP_sock);
		if(ret != 0){
			printf("Error requestEsci --> receiveTCP()\n");
			return 1;		
		}

		if( strcmp(server_reply, "Waiting for SessionID") == 0 )
		{
			//send sessionID
			ret = sendTCP(sessionID, TCP_sock);
			if(ret != 0){
				printf("Error requestEsci --> sendTCP()\n");
				return 1;		
			}
	
		}
	}
	//waiting the reply from the server to be ready to close the connection
	ret = receiveTCP(server_reply, TCP_sock);
	if(ret != 0){
		printf("Error requestEsci --> receiveTCP()\n");
		return 1;		
	}

	if( strcmp(server_reply, "Server ready to close connection") == 0){
		// return to main from executeCommand with 2, signaling the graceful
		// end of connection with the server
		printf("%s\n", server_reply);		
		return 2;	
	}
	

	printf("Error requestEsci()\n");
	return 1;
}
