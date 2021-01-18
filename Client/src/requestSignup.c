#include "../header/lotto_client.h"

int requestSignup(char* command, int TCP_sock, char* IP_address)
{
	int ret;
	char server_reply[BUFFER_SIZE];

	//sending command <!signup username password>
	ret = sendTCP(command, TCP_sock);
	if(ret != 0){
		printf("Error requestSignup--> sendTCP(1)\n");
		return 1;		
	}
	//waiting for server to check the validity of the command
	ret = receiveTCP(server_reply, TCP_sock);
	if(ret != 0){
		printf("Error requestSignup --> receiveTCP(1)\n");
		return 1;		
	}	
	
	printf("%s\n", server_reply);
	return 0;
}