#include "../header/lotto_server.h"

int vedi_giocate(char* argument, int client_sock){

	int ret;
	char buffer_out[BUFFER_SIZE];
	char username[BUFFER_SIZE];
	char sessionID[SESSION_SIZE];
	char parameter[1];

	ret = getClientSessionID(username, sessionID, client_sock);
	if(ret == 1){
		printf("%d: !!! Error: vedi_giocate() --> getClientSessionID()\n", pid);
		return 1;
	}
	//SessionID not valid
	if(ret == 2){
		strcpy(buffer_out, "Your sessionID is not valid");
		printf("%d:-> The client %s used a not valid sessionID\n", pid, username);
		ret = sendTCP(buffer_out, client_sock);
		if(ret != 0){
			printf("%d: !!! Error: vedi_giocate() --> sendTCP()\n", pid);
			return 1;		
		}
		return 0;
	}

	strcpy(parameter, argument);
       
	if(strcmp(parameter, "0") == 0 ){
		ret = printPastPlay(username, client_sock);
		if(ret == 1){
			printf("%d: !!! Error: vedi_giocate() --> printPastPlay()\n", pid);
			return 1;	
		}
	}else{ //parameter == 1	
		ret = printActualPlay(username, client_sock);
		if(ret == 1){
			printf("%d: !!! Error: vedi_giocate() --> printPastPlay()\n", pid);
			return 1;	
		}
	}
	return 0;
}

int printPastPlay(char* username, int client_sock){

	int ret, filedes, play_len, argument_len;
	int playsRowsNumber=0;
	char play[BUFFER_SIZE];
	char buffer_out[BUFFER_SIZE];
	char tmp_buffer_out[BUFFER_SIZE];
	char* cities;
	char* numbers;
	char* bets;
	memset(buffer_out, 0, BUFFER_SIZE);

	FILE* fd;

	char* path="./txt/";
	char* extension = ".txt";
	char UserFile[strlen(path)+ strlen(username)+strlen(extension)+1];
	snprintf( UserFile, sizeof( UserFile ), "%s%s%s", path, username, extension );
	fd = fopen(UserFile, "r"); 

	if(fd == NULL){
		printf("%d: !!! Error: printPastPlay() --> fopen()\n", pid);
		return 1;
	}

	filedes = fileno(fd);
	ret = fcntl(filedes, F_SETLKW, &client_rdlock);
	if(ret == -1){
		fprintf(stderr, "%d:", pid);
		perror(" !!! printPastPlay() ---> fcntl()");		
		fclose(fd);	
		return 1;
	}

	while(fgets(play, BUFFER_SIZE, fd) != NULL ){
		play_len = strlen(play);
	
		strtok(play, " ");// timestamp
		argument_len=strlen(play);
		play_len -= argument_len + 1;
		if(play_len <= 0){
			printf("%d: !!! Error: printPastPlay() --> play_len\n", pid);
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 1;	
		}

		cities = strtok(NULL, "-");
		argument_len = strlen(cities);
		play_len -= argument_len + 1;
		if(play_len <= 0){
			printf("%d: !!! Error: printPastPlay() --> play_len at cities\n", pid);
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 1;	
		}

		numbers = strtok(NULL, "-");
		argument_len = strlen(numbers);
		play_len -= argument_len + 1;
		if(play_len <= 0){
			printf("%d: !!! Error: printPastPlay() --> play_len at numbers\n", pid);
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 1;	
		}

		bets = strtok(NULL, "-");
		ret = printPastCities(cities, buffer_out);
		if(ret == 1){
			printf("%d: !!! Error: printPastPlay() --> printPastCities()\n", pid);
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 1;	
		}
		ret = printPastNumbers(numbers, buffer_out);
		if(ret == 1){
			printf("%d: !!! Error: printPastPlay() --> printPastNumbers()\n", pid);
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 1;	
		}

		ret = printPastBets(bets, buffer_out);
		if(ret == 1){
			printf("%d: !!! Error: printPastPlay() --> printPastBets()\n", pid);
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 1;	
		}
		//Now the buffer_out has the entire play
		
		playsRowsNumber++;
		snprintf(tmp_buffer_out, BUFFER_SIZE, "%s;", buffer_out);
		strcpy(buffer_out, tmp_buffer_out);
	
		//sending the first 10 rows of his past plays
		if(playsRowsNumber == 10 ){
			playsRowsNumber = 0;
			ret = sendTCP(buffer_out, client_sock);
			memset(buffer_out, 0, BUFFER_SIZE);
			if(ret != 0){
				printf("%d: !!! Error: printPastPlay() --> sendTCP()\n", pid);
				return 1;		
			}

		}
		
	}

	if( strlen(buffer_out) != 0){
		ret = sendTCP(buffer_out, client_sock);
		if(ret != 0){
			printf("%d: !!! Error: printPastPlay() --> sendTCP()\n", pid);
			return 1;		
		}
	}

	fcntl(filedes, F_SETLKW, &client_unlock);
	fclose(fd);

	printf("%d:-> The client %s has received his past plays\n", pid, username);

    if(strlen(buffer_out) == 0){
    	strcpy(buffer_out, "No past plays to show");
    }else{
    	strcpy(buffer_out, "End of plays");
    }
       
	ret = sendTCP(buffer_out, client_sock);
	if(ret != 0){
		printf("%d: !!! Error: printPastPlay() --> sendTCP()\n", pid);
		return 1;		
	}
	return 0;

}

int printPastCities(char* cities_in, char* cities_out){
	int cities_len, argument_len, win_len, command_len;
    char cities[BUFFER_SIZE];
    char tmp_cities[BUFFER_SIZE];
    char* win;
    char* argument;

    strcpy(cities, cities_in);
    cities_len=strlen(cities);
    strtok(cities, " ");// r
   	if (strcasecmp(cities, "r") != 0){
   		printf("%d: !!! Error: printPastCities() --> r divider  \n", pid);
		return 1;
	}
    command_len=strlen(cities);
    cities_len -= command_len + 1;

    while(cities_len > 0 ){
    	win= strtok(NULL, " ");
    	win_len = strlen(win);
        cities_len -= win_len + 1;
        argument = strtok(NULL, " ");
        argument_len = strlen(argument);
        cities_len -= argument_len + 1;
        
        snprintf(tmp_cities, BUFFER_SIZE, "%s %s", cities_out, argument);
        strcpy(cities_out, tmp_cities);     	
    }
    return 0;
}

int printPastNumbers(char* numbers_in, char* numbers_out){
	int number_len, argument_len, command_len;
    char numbers[BUFFER_SIZE];
    char tmp_numbers[BUFFER_SIZE];
    char* argument;

    strcpy(numbers, numbers_in);
    number_len=strlen(numbers);
    strtok(numbers, " ");// n
   	if (strcasecmp(numbers, "n") != 0){
   		printf("%d: !!! Error: printPastNumbers() --> n divider  \n", pid);
		return 1;
	}
    command_len=strlen(numbers);
    number_len -= command_len + 1;

    while(number_len > 0 ){
        argument = strtok(NULL, " ");
        argument_len = strlen(argument);
        number_len -= argument_len + 1;
        
        snprintf(tmp_numbers, BUFFER_SIZE, "%s %s", numbers_out, argument);
        strcpy(numbers_out, tmp_numbers);     	
    }
    return 0;
}

int printPastBets(char* bets_in, char* bets_out){
	int bets_len, argument_len, win_len, command_len;
    char bets[BUFFER_SIZE];
    char tmp_bets[BUFFER_SIZE];
    float bet;
    char writeBetting[5][BUFFER_SIZE] = {"Estratto", "Ambo", "Terno", "Quaterna", "Cinquina"};
    int betting=0;
    char* win;
    char* argument;

    strcpy(bets, bets_in);
    bets_len=strlen(bets);
    strtok(bets, " ");// i
   	if (strcasecmp(bets, "i") != 0){
   		printf("%d: !!! Error: printPastBets() --> i divider  \n", pid);
		return 1;
	}
    command_len=strlen(bets);
    bets_len -= command_len + 2;

    while(bets_len > 0 ){
        argument = strtok(NULL, " ");
        argument_len = strlen(argument);
        bets_len -= argument_len + 1;

        win= strtok(NULL, " ");
    	win_len = strlen(win);
        bets_len -= win_len + 1;

        bet = atof(argument);
        if(bet == 0){
        	betting++;
        	continue;
        }
        
        snprintf(tmp_bets, BUFFER_SIZE, "%s * %.2f %s", bets_out, bet, writeBetting[betting]);
        strcpy(bets_out, tmp_bets);   
        betting++;  	
    }
    return 0;
}


int printActualPlay(char* username, int client_sock){

	int ret, filedes, play_len, argument_len;
	int playsRowsNumber=0;
	char play[BUFFER_SIZE];
	char buffer_out[BUFFER_SIZE];
	char tmp_buffer_out[BUFFER_SIZE];
	char* cities;
	char* numbers;
	char* bets;
	memset(buffer_out, 0, BUFFER_SIZE);

	FILE* fd;

	fd = fopen("./txt/actual_plays.txt", "r"); 

	if(fd == NULL){
		printf("%d: !!! Error: printPastPlay() --> fopen()\n", pid);
		return 1;
	}

	filedes = fileno(fd);
	ret = fcntl(filedes, F_SETLKW, &client_rdlock);
	if(ret == -1){
		fprintf(stderr, "%d:", pid);
		perror(" !!! printPastPlay() ---> fcntl()");		
		fclose(fd);	
		return 1;
	}

	while(fgets(play, BUFFER_SIZE, fd) != NULL ){
		play_len = strlen(play);
	
		strtok(play, " ");	// username
		if( strcmp(play, username) != 0 ){
			continue;
		}

		argument_len=strlen(play);
		play_len -= argument_len + 1;
		if(play_len <= 0){
			printf("%d: !!! Error: printActualPlay() --> play_len\n", pid);
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 1;	
		}

		cities = strtok(NULL, "-");
		argument_len = strlen(cities);
		play_len -= argument_len + 1;
		if(play_len <= 0){
			printf("%d: !!! Error: printActualPlay() --> play_len at cities\n", pid);
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 1;	
		}

		numbers = strtok(NULL, "-");
		argument_len = strlen(numbers);
		play_len -= argument_len + 1;
		if(play_len <= 0){
			printf("%d: !!! Error: printActualPlay() --> play_len at numbers\n", pid);
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 1;	
		}

		bets = strtok(NULL, "-");

		ret = printActualCities(cities, buffer_out);
		if(ret == 1){
			printf("%d: !!! Error: printActualPlay() --> printActualCities()\n", pid);
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 1;	
		}
		ret = printActualNumbers(numbers, buffer_out);
		if(ret == 1){
			printf("%d: !!! Error: printActualPlay() --> printActualNumbers()\n", pid);
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 1;	
		}
		ret = printActualBets(bets, buffer_out);
		if(ret == 1){
			printf("%d: !!! Error: printActualPlay() --> printActualBets()\n", pid);
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 1;	
		}
		//Now the buffer_out has the entire play
		
		playsRowsNumber++;
		snprintf(tmp_buffer_out, BUFFER_SIZE, "%s;", buffer_out);
		strcpy(buffer_out, tmp_buffer_out);
	
		//sending the first 15 rows of his past plays
		if(playsRowsNumber == 10 ){
			playsRowsNumber = 0;
			ret = sendTCP(buffer_out, client_sock);
			memset(buffer_out, 0, BUFFER_SIZE);
			if(ret != 0){
				printf("%d: !!! Error: printActualPlay() --> sendTCP()\n", pid);
				return 1;		
			}

		}
		
	}

	fcntl(filedes, F_SETLKW, &client_unlock);
	fclose(fd);

	if( strlen(buffer_out) != 0){
		ret = sendTCP(buffer_out, client_sock);
		if(ret != 0){
			printf("%d: !!! Error: printActualPlay() --> sendTCP()\n", pid);
			return 1;		
		}
	}

	printf("%d:-> The client %s has received his actual plays\n", pid, username);

    if(strlen(buffer_out) == 0){
    	strcpy(buffer_out, "No actual plays to show");
    }else{
    	strcpy(buffer_out, "End of plays");
    }
       
	ret = sendTCP(buffer_out, client_sock);
	if(ret != 0){
		printf("%d: !!! Error: printActualPlay() --> sendTCP()\n", pid);
		return 1;		
	}
	return 0;

}

int printActualCities(char* cities_in, char* cities_out){
	int cities_len, argument_len, command_len;
    char cities[BUFFER_SIZE];
    char tmp_cities[BUFFER_SIZE];
    char* argument;

    strcpy(cities, cities_in);
    cities_len = strlen(cities);
    strtok(cities, " ");// r
   	if (strcasecmp(cities, "r") != 0){
   		printf("%d: !!! Error: printActualCities() --> r divider  \n", pid);
		return 1;
	}
    command_len = strlen(cities);
    cities_len -= command_len + 1;

    while(cities_len > 0 ){
        argument = strtok(NULL, " ");
        argument_len = strlen(argument);
        cities_len -= argument_len + 1;
        
        snprintf(tmp_cities, BUFFER_SIZE, "%s %s", cities_out, argument);
        strcpy(cities_out, tmp_cities);     	
    }
    return 0;
}

int printActualNumbers(char* numbers_in, char* numbers_out){
	int number_len, argument_len, command_len;
    char numbers[BUFFER_SIZE];
    char tmp_numbers[BUFFER_SIZE];
    char* argument;

    strcpy(numbers, numbers_in);
    number_len = strlen(numbers);
    strtok(numbers, " ");// n
   	if (strcasecmp(numbers, "n") != 0){
   		printf("%d: !!! Error: printActualNumbers() --> n divider  \n", pid);
		return 1;
	}
    command_len = strlen(numbers);
    number_len -= command_len + 1;

    while(number_len > 0 ){
        argument = strtok(NULL, " ");
        argument_len = strlen(argument);
        number_len -= argument_len + 1;
        
        snprintf(tmp_numbers, BUFFER_SIZE, "%s %s", numbers_out, argument);
        strcpy(numbers_out, tmp_numbers);     	
    }
    return 0;
}

int printActualBets(char* bets_in, char* bets_out){
	int bets_len, argument_len, command_len;
    char bets[BUFFER_SIZE];
    char tmp_bets[BUFFER_SIZE];
    float bet;
    char writeBetting[5][BUFFER_SIZE] = {"Estratto", "Ambo", "Terno", "Quaterna", "Cinquina"};
    int betting=0;
    char* argument;

    strcpy(bets, bets_in);
    bets_len = strlen(bets);
    strtok(bets, " ");// i
   	if (strcasecmp(bets, "i") != 0){
   		printf("%d: !!! Error: printActualBets() --> i divider  \n", pid);
		return 1;
	}
    command_len = strlen(bets);
    bets_len -= command_len + 1;

    while(bets_len > 0 ){
        argument = strtok(NULL, " ");
        argument_len = strlen(argument);
        bets_len -= argument_len + 1;

        bet = atof(argument);
        if(bet == 0){
        	betting++;
        	continue;
        }

        
        snprintf(tmp_bets, BUFFER_SIZE, "%s * %.2f %s", bets_out, bet, writeBetting[betting]);
        strcpy(bets_out, tmp_bets);   
        betting++;  	
    }
    return 0;
}


