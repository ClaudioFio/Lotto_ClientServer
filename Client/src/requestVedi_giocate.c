#include "../header/lotto_client.h"

int requestVedi_giocate(char* command, int TCP_sock){

	int ret;
	int NTimes=0;
	char server_reply[BUFFER_SIZE];
	char parsedCommand[BUFFER_SIZE];
	char* argument;

	strcpy(parsedCommand, command);
	strtok(parsedCommand, " ");	
	argument = strtok(NULL, " ");
	ret=strlen(argument);

	if(( strcmp(argument, "0") != 0 ) && ( strcmp(argument, "1") != 0) ){
		printf("Not valid argument \n");
		return 0;
	}

	ret = sendTCP(command, TCP_sock);
	if(ret != 0){
		printf("Error requestInvia_giocata --> sendTCP()\n");
		return 1;		
	}

	ret = receiveTCP(server_reply, TCP_sock);
	if(ret != 0){
		printf("Error requestInvia_giocata --> receiveTCP()\n");
		return 1;		
	}
	
	if( strcmp(server_reply, "Waiting for username") == 0 )
	{
		//send username
		ret = sendTCP(username, TCP_sock);
		if(ret != 0){
			printf("Error requestInvia_giocata --> sendTCP()\n");
			return 1;		
		}

		ret = receiveTCP(server_reply, TCP_sock);
		if(ret != 0){
			printf("Error requestInvia_giocata --> receiveTCP()\n");
			return 1;		
		}

		if( strcmp(server_reply, "Waiting for SessionID") == 0 )
		{
			//send sessionID
			ret = sendTCP(sessionID, TCP_sock);
			if(ret != 0){
				printf("Error requestInvia_giocata --> sendTCP()\n");
				return 1;		
			}
	
		}

		while(1){
			ret = receiveTCP(server_reply, TCP_sock);
			if(ret != 0){
				printf("Error requestInvia_giocata --> receiveTCP()\n");
				return 1;		
			}
			if( ( strcmp(server_reply, "End of plays") == 0) || (strcmp(server_reply, "No past plays to show") == 0 )|| (strcmp(server_reply, "No actual plays to show") == 0 ) ){
				printf("%s\n", server_reply);
				return 0;
			}
			printPlay(server_reply, NTimes);
			NTimes++;
		

		}

		return 0;
		
	}

	printf("Error requestVedi_giocate()\n");
	return 1;

}

int printPlay(char* play_in, int NTimes){
	int play_len, argument_len;
	char play[BUFFER_SIZE];
	char output[BUFFER_SIZE];
	char tmp_output[BUFFER_SIZE];
	char* argument;
	int i=(1 + (NTimes*10));

	strcpy(play, play_in);
	play_len = strlen(play);


	strtok(play, ";");// first play
	argument_len=strlen(play);
	play_len -= argument_len + 1;


	snprintf(output, BUFFER_SIZE, "%d)%s\n", i, play);
	strcpy(tmp_output, output);
	

	while(play_len > 0){
		i++;
		argument = strtok(NULL, ";");
		argument_len = strlen(argument);
		play_len -= argument_len + 1;

		snprintf(tmp_output, BUFFER_SIZE, "%s%d)%s\n", output, i, argument );
		strcpy(output, tmp_output);
	}
	strcpy(play_in, output);
	printf(play_in);
	return 0;
}