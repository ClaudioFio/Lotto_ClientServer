#include "../header/lotto_client.h"

int requestVedi_vincite(char* command, int TCP_sock)
{
	int ret;
	char server_reply[BUFFER_SIZE];
	char previousTime[BUFFER_SIZE]="0";

	ret = sendTCP(command, TCP_sock);
	if(ret != 0){
		printf("Error requestVedi_vincite --> sendTCP()\n");
		return 1;		
	}

	ret = receiveTCP(server_reply, TCP_sock);
	if(ret != 0){
		printf("Error requestVedi_vincite --> receiveTCP()\n");
		return 1;		
	}
	
	if( strcmp(server_reply, "Waiting for username") == 0 )
	{
		//send username
		ret = sendTCP(username, TCP_sock);
		if(ret != 0){
			printf("Error requestVedi_vincite --> sendTCP()\n");
			return 1;		
		}

		ret = receiveTCP(server_reply, TCP_sock);
		if(ret != 0){
			printf("Error requestVedi_vincite --> receiveTCP()\n");
			return 1;		
		}

		if( strcmp(server_reply, "Waiting for SessionID") == 0 )
		{
			//send sessionID
			ret = sendTCP(sessionID, TCP_sock);
			if(ret != 0){
				printf("Error requestVedi_vincite --> sendTCP()\n");
				return 1;		
			}
	
		}

		while(1){
			ret = receiveTCP(server_reply, TCP_sock);
			if(ret != 0){
				printf("Error requestVedi_vincite --> receiveTCP()\n");
				return 1;		
			}
			if(strcmp(server_reply, "No wins to show") == 0 ){
				printf("%s\n", server_reply);
				return 0;
			}

			if( strcmp(server_reply, "Total win") == 0){
				ret = receiveTCP(server_reply, TCP_sock);
				if(ret != 0){
					printf("Error requestVedi_vincite --> receiveTCP()\n");
					return 1;		
				}
				printTotalWin(server_reply);
				return 0;
				
			}
			 
			printWin(server_reply, previousTime);

		}
		return 0;
	}

	printf("Error requestVedi_vincite()\n");
	return 1;

}

int printWin(char* play_in, char* previousTime){
	int play_len, argument_len;
	char play[BUFFER_SIZE];
	char actualTime[BUFFER_SIZE];
	char output[BUFFER_SIZE];
	char tmp_output[BUFFER_SIZE];
	char* argument;
	char* appo_time;
	long int ltime; 
   
	strcpy(play, play_in);
	play_len = strlen(play);
	strtok(play, " ");

	argument_len = strlen(play);
	play_len -= argument_len + 1;

	if(strcmp(play, previousTime) != 0){
		strcpy(previousTime, play);
		ltime = strtol(play, &appo_time, 10);
		strcpy(actualTime,(asctime( localtime(&ltime) ) ));
		printf("Extraction on %s\n", actualTime);
	}


	argument = strtok(NULL, ";");// first win without timestamp
	argument_len=strlen(argument);
	play_len -= argument_len + 1;

	snprintf(output, BUFFER_SIZE, "%s", argument);

	strcpy(tmp_output, output);
	while(play_len > 0){
		argument = strtok(NULL, " ");//timestamp
		argument_len = strlen(argument);
		play_len -= argument_len + 1;

		if(strcmp(argument, previousTime) != 0){
		strcpy(previousTime, argument);
		ltime = strtol(argument, &appo_time, 10);
		strcpy(actualTime,(asctime( localtime(&ltime) ) ));
		snprintf(tmp_output, BUFFER_SIZE, "%s\n************************************************************************\nExtraction on %s", output, actualTime );
		strcpy(output, tmp_output);
	}

		argument = strtok(NULL, ";");
		argument_len = strlen(argument);
		play_len -= argument_len + 2;

		snprintf(tmp_output, BUFFER_SIZE, "%s\n%s", output, argument );
		strcpy(output, tmp_output);
	}
	strcpy(play_in, output);
	printf(play_in);
	printf("\n************************************************************************\n");
	return 0;
}

void printTotalWin(char* totalWin_in){

	char totalWin[BUFFER_SIZE];
	char output[BUFFER_SIZE];
	char tmp_output[BUFFER_SIZE];
	char* argument;
	char writeBetting[5][BUFFER_SIZE] = {"ESTRATTO", "AMBO", "TERNO", "QUATERNA", "CINQUINA"};
    int betting=0;
    memset(output, 0, BUFFER_SIZE);

	strcpy(totalWin, totalWin_in);//Estratto
	strtok(totalWin, " ");



	snprintf(tmp_output, BUFFER_SIZE, "%sWins on %s: %s\n", output, writeBetting[betting], totalWin);
	strcpy(output, tmp_output);
	for(betting=1; betting<5; betting++){

		argument = strtok(NULL, " ");
		snprintf(tmp_output, BUFFER_SIZE, "%sWins on %s: %s\n", output, writeBetting[betting], argument);
		strcpy(output, tmp_output);

	}

	printf("%s\n", output);

}
