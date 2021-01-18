#include "../header/lotto_server.h"

int vedi_vincite(int client_sock){

	int ret;
	char buffer_out[BUFFER_SIZE];
	char username[BUFFER_SIZE];
	char sessionID[SESSION_SIZE];


	ret = getClientSessionID(username, sessionID, client_sock);
	if(ret == 1){
		printf("%d: !!! Error: vedi_vincite() --> getClientSessionID()\n", pid);
		return 1;
	}
	//SessionID not valid
	if(ret == 2){
		strcpy(buffer_out, "Your sessionID is not valid");
		printf("%d:-> The client %s used a not valid sessionID\n", pid, username);
		ret = sendTCP(buffer_out, client_sock);
		if(ret != 0){
			printf("%d: !!! Error: vedi_vincite() --> sendTCP()\n", pid);
			return 1;		
		}
		return 0;
	}

	ret = printWin(username, client_sock);
	if(ret == 1){
		printf("%d: !!! Error: vedi_vincite() --> printWin()\n", pid);
		return 1;	
	}
	return 0;		
}


int printWinBets(char* bets_in, char* bets_out, float* moneyWin){
	int bets_len, argument_len, win_len, command_len;
    char bets[BUFFER_SIZE];
    char tmp_bets[BUFFER_SIZE];
    float winf;
    char writeBetting[5][BUFFER_SIZE] = {"Estratto", "Ambo", "Terno", "Quaterna", "Cinquina"};
    int betting=0;
    char* win;
    char* argument;

    strcpy(bets, bets_in);
    bets_len=strlen(bets);
    strtok(bets, " ");// i
   	if (strcasecmp(bets, "i") != 0){
		return 1;
	}
    command_len=strlen(bets);
    bets_len -= command_len + 2;

    snprintf(tmp_bets, BUFFER_SIZE, "%s  >>", bets_out);
    strcpy(bets_out, tmp_bets);

    while(bets_len > 0 ){
        argument = strtok(NULL, " ");
        argument_len = strlen(argument);
        bets_len -= argument_len + 1;

        win= strtok(NULL, " ");
    	win_len = strlen(win);
        bets_len -= win_len + 1;

        winf = atof(win);
        if(winf == 0){
        	betting++;
        	continue;
        }
        moneyWin[betting] += winf ;
        snprintf(tmp_bets, BUFFER_SIZE, "%s  %s %.2f", bets_out, writeBetting[betting], winf);
        strcpy(bets_out, tmp_bets);  
        betting++;  	
    }
    return 0;
}

int printWin(char* username, int client_sock){

	int ret, filedes, play_len, argument_len;
	int i;
	int playsRowsNumber=0;
	float moneyWin[5]={0, 0 ,0, 0, 0};
	char play[BUFFER_SIZE];
	char buffer_out[BUFFER_SIZE];
	char tmp_buffer_out[BUFFER_SIZE];
	char totalWin[BUFFER_SIZE];
	char tmp_totalWin[BUFFER_SIZE];
	char* cities;
	char* numbers;
	char* bets;
	memset(buffer_out, 0, BUFFER_SIZE);
	memset(totalWin, 0, BUFFER_SIZE);

	FILE* fd;

	char* path="./txt/";
	char* extension = ".txt";
	char UserFile[strlen(path)+ strlen(username)+strlen(extension)+1];
	snprintf( UserFile, sizeof( UserFile ), "%s%s%s", path, username, extension );
	fd = fopen(UserFile, "r"); 

	if(fd == NULL){
		printf("%d: !!! Error: printWin() --> fopen()\n", pid);
		return 1;
	}

	filedes = fileno(fd);
	ret = fcntl(filedes, F_SETLKW, &client_rdlock);
	if(ret == -1){
		fprintf(stderr, "%d:", pid);
		perror(" !!! printWin() ---> fcntl()");		
		fclose(fd);	
		return 1;
	}

	while(fgets(play, BUFFER_SIZE, fd) != NULL ){

		play_len = strlen(play);
		strtok(play, " ");// timestamp
		argument_len=strlen(play);
		play_len -= argument_len + 1;
		if(play_len <= 0){
			printf("%d: !!! Error: printWin() --> play_len\n", pid);
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 1;	
		}

		cities = strtok(NULL, "-");
		argument_len = strlen(cities);
		play_len -= argument_len + 1;
		if(play_len <= 0){
			printf("%d: !!! Error: printWin() --> play_len at cities\n", pid);
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 1;	
		}

		numbers = strtok(NULL, "-");
		argument_len = strlen(numbers);
		play_len -= argument_len + 1;
		if(play_len <= 0){
			printf("%d: !!! Error: printWin() --> play_len at numbers\n", pid);
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 1;	
		}

		bets = strtok(NULL, "-");

		ret = printWinCities(cities, buffer_out, play);
		if(ret == 1){
			printf("%d: !!! Error: printWin() --> printWinCities()\n", pid);
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 1;	
		}
		if (ret == 2)
		{	
			continue;
		}
		ret = printWinNumbers(numbers, buffer_out);
		if(ret == 1){
			printf("%d: !!! Error: printWin() --> printWinNumbers()\n", pid);
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 1;	
		}
		ret = printWinBets(bets, buffer_out, moneyWin);
		if(ret == 1){
			printf("%d: !!! Error: printWin() --> printWinBets()\n", pid);
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 1;	
		}
		//Now the buffer_out has the entire win
		playsRowsNumber++;
		snprintf(tmp_buffer_out, BUFFER_SIZE, "%s;", buffer_out);
		strcpy(buffer_out, tmp_buffer_out);
		//sending the first 10 rows of his wins
		if(playsRowsNumber == 10 ){
			playsRowsNumber = 0;
			ret = sendTCP(buffer_out, client_sock);
			memset(buffer_out, 0, BUFFER_SIZE);
			if(ret != 0){
				printf("%d: !!! Error: printWin() --> sendTCP()\n", pid);
				return 1;		
			}
		}	

	}
	fcntl(filedes, F_SETLKW, &client_unlock);
	fclose(fd);

	if( strlen(buffer_out) != 0){
		ret = sendTCP(buffer_out, client_sock);
		if(ret != 0){
			printf("%d: !!! Error: printWin() --> sendTCP()\n", pid);
			return 1;		
		}
	}

    if(strlen(buffer_out) == 0){
    	strcpy(buffer_out, "No wins to show");
    }else{
    	//sending the total win
    	strcpy(buffer_out, "Total win");
    	ret = sendTCP(buffer_out, client_sock);
		if(ret != 0){
			printf("%d: !!! Error: printWin() --> sendTCP()\n", pid);
			return 1;		
		}

		for(i=0; i<5; i++){
			snprintf(tmp_totalWin, BUFFER_SIZE, "%s %.2f", totalWin, moneyWin[i]);
			strcpy(totalWin, tmp_totalWin);
		}
		ret = sendTCP(totalWin, client_sock);
		if(ret != 0){
			printf("%d: !!! Error: printWin() --> sendTCP()\n", pid);
			return 1;		
		}

		printf("%d:-> The client %s has received his wins\n", pid, username);
		return 0;

    }
	ret = sendTCP(buffer_out, client_sock);
	if(ret != 0){
		printf("%d: !!! Error: printWin() --> sendTCP()\n", pid);
		return 1;		
	}


	return 0;


}


int printWinCities(char* cities_in, char* cities_out, char* timestamp){
	int cities_len, argument_len, win_len, command_len;
    char cities[BUFFER_SIZE];
    char appo_cities[BUFFER_SIZE];
    char tmp_cities[BUFFER_SIZE];
    char* win;
    char* argument;
    int check=0;
    memset(appo_cities, 0, BUFFER_SIZE);

    strcpy(cities, cities_in);
    cities_len=strlen(cities);
    strtok(cities, " ");// r
   	if (strcasecmp(cities, "r") != 0){
   		printf("%d: !!! Error: printWinCities() --> i divider  \n", pid);
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
        if( strcmp(win, "0") == 0 ){
        	continue;
        }
        check++;
        snprintf(tmp_cities, BUFFER_SIZE, "%s %s", appo_cities, argument); // only cities
        strcpy(appo_cities, tmp_cities);     	
    }
    if(check == 0){
    	return 2;
    }
    snprintf(tmp_cities, BUFFER_SIZE, "%s %s", timestamp, appo_cities); // timestamp + cities
    strcpy(appo_cities, cities_out);
    snprintf(cities_out, BUFFER_SIZE, "%s %s", appo_cities, tmp_cities);//old citiesout + new(timestamp+ cities)
    return 0;
}

int printWinNumbers(char* numbers_in, char* numbers_out){
	int number_len, argument_len, command_len;
    char numbers[BUFFER_SIZE];
    char tmp_numbers[BUFFER_SIZE];
    char* argument;

    strcpy(numbers, numbers_in);
    number_len=strlen(numbers);
    strtok(numbers, " ");// n
   	if (strcasecmp(numbers, "n") != 0){
   		printf("%d: !!! Error: printWinNumbers() --> n divider  \n", pid);
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


