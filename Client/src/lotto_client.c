#include "../header/lotto_client.h"

int main(int argc, char *argv[])
{
	int TCP_sock, ret;
	char* input_str;
	char buffer_out[BUFFER_SIZE];
	struct sockaddr_in sv_addr;
	
	if(argc != 3){
		printf("Error: incorrect number of arguments. \nCorrect sintax : ./lotto_client <server_IP> <server_TCP_port>\n");
		return 1;	
	}	

	//inizialing structures for TCP connection with the server
	memset(&sv_addr, 0, sizeof(sv_addr));
	sv_addr.sin_family = AF_INET;
	sv_addr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &sv_addr.sin_addr);
	
	if( (TCP_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
		perror("TCP socket() ");
		return 1;	
	} 
	
	if( (ret = connect(TCP_sock, (struct sockaddr*)&sv_addr, sizeof(sv_addr))) == -1 ){
		perror("connect() ");
		close(TCP_sock);
		return 1;
	}

	// connection successful, showing menu options
	printf("\nConnection to server on IP %s , port %s successful!\n\n", argv[1], argv[2]);
	showMenu();

	while(1)
	{
		while(1) // getting a command or a new istant message
		{
			printf("%s > ", username);
			memset(buffer_out, 0, BUFFER_SIZE);
			input_str = fgets(buffer_out, BUFFER_SIZE, stdin);
			if(input_str == NULL){
				printf("Error main --> fgets()\n ");	
				continue;
			}
			break;
		}
		// removing heading blank spaces
		while(*input_str == ' ')
			input_str++;
				
		strcpy(buffer_out, input_str);

		//if command is "!help", we serve it client side	
		if(strcmp("!help\n", buffer_out) == 0){
			showMenu();		
			continue;
		}
		// recognize command and send it
		ret = requestCommand(buffer_out, TCP_sock, argv[1]);
		if(ret == 1){
			printf("Error requestCommand()\n");
			break;		
		}
		if(ret == 2){
			printf("Closing socket TCP and terminating client process...\n");
			break;	
		}
	}
	close(TCP_sock);
	printf("See you next time! \n");
	return 0;
}
