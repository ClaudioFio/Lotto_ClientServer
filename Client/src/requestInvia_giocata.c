#include "../header/lotto_client.h"

int requestInvia_giocata(char* command, int TCP_sock){

	int ret;
	char server_reply[BUFFER_SIZE];

	//check if the client could send a valid play
	ret = checkPlay(command);
	//Not valid City
	if(ret == 1){
		printf("Fail: City not valid!\n");
		return 0;		
	}
	//Number not valid
	if(ret == 2){
		printf("Fail: Number not correct!\n");
		return 0;		
	}
	// Bet not correct
	if(ret == 3){
		printf("Fail: Bet not correct!\n");
		return 0;		
	}
	//Play not complete
	if(ret == 4){
		printf("Fail: Play not correct!\n");
		return 0;		
	}

	//The Play is correct, so it can be send
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

		ret = receiveTCP(server_reply, TCP_sock);
		if(ret != 0){
			printf("Error requestInvia_giocata --> receiveTCP()\n");
			return 1;		
		}

		printf("%s\n", server_reply);
		return 0;
		
	}

	printf("Error requestInvia_giocata()\n");
	return 1;

}


int checkPlay(char* play_in){
	int ret, play_len, argument_len;
	int bet_check;
	int check_number;
	int check=0;	
	char play[BUFFER_SIZE];
	//parsed play
	char* cities;
	char* numbers;
	char* bets;

	strcpy(play, play_in);
	play_len = strlen(play)-4;

	strtok(play, " ");// !invia_giocata
	argument_len=strlen(play);
	play_len -= argument_len + 1;
	if(play_len <= 0){
		return 4;
	}

	cities = strtok(NULL, "-");
	argument_len = strlen(cities);
	play_len -= argument_len + 1;
	if(play_len <= 0){
		return 4;
	}

	numbers = strtok(NULL, "-");
	argument_len = strlen(numbers);
	play_len -= argument_len + 1;
	if(play_len < 0){
		return 4;
	}

	bets = strtok(NULL, "-");
	argument_len = strlen(bets);
	play_len -= argument_len + 1;
	if(play_len > 0){
		return 4;
	}

	ret = checkCities(cities);
	if(ret == 1){
         return 1;
   	}else{
   		check++;
   	}

	ret = checkNumbers(numbers);
	if(ret == 6){
         return 2;
   	}else{
   		check++;
   		check_number = ret;
   	}

	ret = checkBets(bets);
	if(ret == 6){
         return 3;
   	}else{
   		check++;
   		bet_check = ret;
   	}

   	if(check != 3){
    	return 4;
    }
    
    if(check_number < bet_check){
    	return 3;
    }

    return 0;

}

int checkCities(char* cities_in){
    int ret, cities_len, argument_len, command_len, i;
    char cities[BUFFER_SIZE];
    char betted_cities[11][BUFFER_SIZE];
    char* argument;
    int city_index = 0;

    strcpy(cities, cities_in);
    cities_len=strlen(cities);
    strtok(cities, " ");// r
    if (strcasecmp(cities, "r") != 0){
		return 1;
	}
    command_len=strlen(cities);
    cities_len -= command_len + 1;

    for(i =0; i < 11; i++){
    	strcpy(betted_cities[i], "-1");
    }

    while(cities_len > 0 ){
    	i=0;
        argument = strtok(NULL, " ");
        argument_len = strlen(argument);
        cities_len -= argument_len + 1;
        ret = checkCity(argument);
     	if(ret == 1){//City not in the list
     		return 1;
     	}
     	if(ret == 2){//City == tutte
     		if(cities_len > 0){
     			return 1;
     		}
     		if(city_index != 0){//If the client betted on "Tutte" and something else
     			return 1;
     		}
     		return 0;
     	}
     	while( strcmp(betted_cities[i], "-1") != 0 ){
     		if(strcasecmp(betted_cities[i], argument) == 0 ){//If the client betted on a city more than once
     			return 1;
     		}
     		i++;
     	}
     	strcpy(betted_cities[i], argument);
     	city_index++;       
    }
    return 0;
}

int checkCity(char* argument){
	int i;

	const char City[11][BUFFER_SIZE] = {"Bari", "Cagliari", "Firenze", "Genova", "Milano", "Napoli", "Palermo", "Roma", "Torino", "Venezia", "Nazionale"};	
	
	if (strcasecmp(argument, "tutte") == 0){
    	return 2;
    }

	for(i=0; i<11; i++){
	    if(strcasecmp(City[i], argument)==0)
	    	return 0;
	}
	return 1;
}

int checkNumbers(char* numbers_in){
    int numbers_len, argument_len, command_len, i;
    int number;
    int client_numers[10]={-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    char numbers[BUFFER_SIZE];
    char* argument;
    int check_number=0;

    strcpy(numbers, numbers_in);
    numbers_len=strlen(numbers);
    strtok(numbers, " ");// n
    if (strcasecmp(numbers, "n") != 0){
		return 6;
	}
    command_len=strlen(numbers);
    numbers_len -= command_len + 1;

    while(numbers_len > 0 ){
        argument = strtok(NULL, " ");
        argument_len = strlen(argument);
        numbers_len -= argument_len + 1;
        
        number = atoi(argument);
     	if(number <= 0 || number > 90 ){
     		return 6; 
     	}
     	for(i=0; i<10; i++){
			if(client_numers[i] == -1){
				client_numers[i]=number;
				break;
			}
			if (client_numers[i] == number)
			{
				return 6;
			}
    	}
     	check_number++;
    }
    if(check_number > 10 || check_number == 0){
    	return 6;
    }

    return check_number;
}

int checkBets(char* bets_in){
    int bets_len, argument_len, command_len;
    int bet_check_total=0;
    float bet;
    char bets[BUFFER_SIZE];
    char* argument;

    strcpy(bets, bets_in);
    bets_len=strlen(bets);
    strtok(bets, " ");// i
    if (strcasecmp(bets, "i") != 0){
		return 6;
	}
    command_len=strlen(bets);
    bets_len -= command_len + 1;

    while(bets_len > 0 ){
        argument = strtok(NULL, " ");
        argument_len = strlen(argument);
        bets_len -= argument_len + 1;
        
        bet = atof(argument);
         	if(bet < 0 ){
         		return 6;
         	}
            if(bet > 0){
                bet_check_total++;
            }
        
    }
     if(bet_check_total == 0){
    	return 6;
    }
    return bet_check_total;
}
