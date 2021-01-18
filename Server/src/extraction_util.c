#include "../header/lotto_server.h"

int Extraction()
{
	int ret, wr_len, filedes, i;
	int NumberOfCity, indexOneToNinety, temp_number, randomIndex;
    int OneToNinety[89];
    char chosenNumber[5][2];

    FILE* fd;

	const char City[11][BUFFER_SIZE] = {"Bari", "Cagliari", "Firenze", "Genova", "Milano", "Napoli", "Palermo", "Roma", "Torino", "Venezia", "Nazionale"};	

	fd = fopen("./txt/actual_extraction.txt", "w"); 

	if(fd == NULL){
		printf("%d: !!! Error: Extraction() --> fopen()\n", pidEx);
		return 1;
	}

	filedes = fileno(fd);
	ret = fcntl(filedes, F_SETLKW, &client_wrlock);
	if(ret == -1){
		fprintf(stderr, "%d:", pidEx);
		perror(" !!! Extraction() ---> fcntl()");		
		fclose(fd);	
		return 1;
	} 

    srand(time(NULL));
    for(NumberOfCity = 0; NumberOfCity < 11; NumberOfCity++){
    	//fill the array with numbers from 1 to 90
        for(indexOneToNinety = 0; indexOneToNinety < 90; indexOneToNinety++){
            OneToNinety[indexOneToNinety] = indexOneToNinety + 1;
        }
		
		// shuffle the first 5 position of the array    
       	for (indexOneToNinety = 0; indexOneToNinety < 5; indexOneToNinety++) { 

		    temp_number = OneToNinety[indexOneToNinety];
		    randomIndex = rand() % 90;

		    OneToNinety[indexOneToNinety] = OneToNinety[randomIndex];
		    OneToNinety[randomIndex] = temp_number;
		}
            
		//convert number in strings
		wr_len = 0;
		for(i = 0; i < 5; i++){
			snprintf(chosenNumber[i], BUFFER_SIZE, "%d", OneToNinety[i]);
			wr_len += strlen(chosenNumber[i]);
		}            
		
		wr_len += strlen(City[NumberOfCity]);

		ret = fprintf(fd, "%s %d %d %d %d %d\n", City[NumberOfCity], OneToNinety[0], OneToNinety[1], OneToNinety[2], OneToNinety[3], OneToNinety[4]);
		if( ret != (wr_len + 6) || ret < 0){
			printf("%d: !!! Error: Extraction() --> fprintf() ret = %d \n", pidEx, ret);
			fcntl(filedes, F_SETLKW, &client_unlock);	
			fclose(fd);
			return 1;	
		}
	}

	fcntl(filedes, F_SETLKW, &client_unlock);	
	fclose(fd);

	printf("************************************************************************\n\t\t\tNEW EXTRACTION\n");

	return 0;
}

int CheckExtraction(){

	int ret, filedesAp, play_len, argument_len;
    char play[BUFFER_SIZE];
    char* cities;
    char* numbers;
    char* bets;

    playElem ClientPlay;

	FILE* actualPlays;

	// Open the file of the clients plays
	actualPlays = fopen("./txt/actual_plays.txt", "r"); 

	if(actualPlays == NULL){
		printf("%d: !!! Error: CheckExtraction() --> fopen()\n", pidEx);
		return 1;
	}

	filedesAp = fileno(actualPlays);
	ret = fcntl(filedesAp, F_SETLKW, &client_rdlock);
	if(ret == -1){
		fprintf(stderr, "%d:", pidEx);
		perror(" !!! CheckExtraction() ---> fcntl()");		
		fclose(actualPlays);	
		return 1;
	} 

	while(fgets(play, BUFFER_SIZE, actualPlays) != NULL ){
		strtok(play, "\n");
		play_len = strlen(play);

		strtok(play, " ");// username
		argument_len=strlen(play);
		play_len -= argument_len + 1;
		if(play_len <= 0){
			printf("%d: !!! Error: CheckExtraction() --> play_len\n", pidEx);
			fcntl(filedesAp, F_SETLKW, &client_unlock);
			fclose(actualPlays);
			return 1;	
		}

		cities = strtok(NULL, "-");
		argument_len = strlen(cities);
		play_len -= argument_len + 1;
		if(play_len <= 0){
			printf("%d: !!! Error: CheckExtraction() --> play_len at cities\n", pidEx);
			fcntl(filedesAp, F_SETLKW, &client_unlock);
			fclose(actualPlays);
			return 1;	
		}

		numbers = strtok(NULL, "-");
		argument_len = strlen(numbers);
		play_len -= argument_len + 1;
		if(play_len <= 0){
			printf("%d: !!! Error: CheckExtraction() --> play_len at numbers\n", pidEx);
			fcntl(filedesAp, F_SETLKW, &client_unlock);
			fclose(actualPlays);
			return 1;	
		}

		bets = strtok(NULL, "-");

		ret = checkingPlay(cities, numbers, bets, &ClientPlay);
		if(ret == 1){
			printf("%d: !!! Error: CheckExtraction() --> checkingPlay()\n", pidEx);
			return 1;	
		}

		CalculateMoneyWin(&ClientPlay);
		ret = printCheckedPlay(play, &ClientPlay);
		if(ret == 1){
			printf("%d: !!! Error: CheckExtraction() --> printCheckedPlay()\n", pidEx);
			return 1;	
		}		
	}
	fcntl(filedesAp, F_SETLKW, &client_unlock);
	fclose(actualPlays);
	
	actualPlays = fopen("./txt/actual_plays.txt", "w"); 

	if(actualPlays == NULL){
		printf("%d: !!! Error: CheckExtraction() --> fopen()\n", pidEx);
		return 1;
	}

	filedesAp = fileno(actualPlays);
	ret = fcntl(filedesAp, F_SETLKW, &client_wrlock);
	if(ret == -1){
		fprintf(stderr, "%d:", pidEx);
		perror(" !!! CheckExtraction() ---> fcntl()");		
		fclose(actualPlays);	
		return 1;
	} 

	if( (ret != 0) || (ret < 0) ){
		printf("%d: !!! Error: CheckExtraction() --> fprintf() ret = %d \n", pidEx, ret);
		fcntl(filedesAp, F_SETLKW, &client_unlock);
		fclose(actualPlays);
		return 1;	
	}

	fcntl(filedesAp, F_SETLKW, &client_unlock);
	fclose(actualPlays);

	ret = printPastExtraction();
	if(ret == 1){
		printf("%d: !!! Error: CheckExtraction() --> printPastExtraction()\n", pidEx);
		return 1;	
	} 
	printf("************************************************************************\n");


	return 0;

}

int checkingPlay(char* cities_in, char* numbers_in, char* bets_in, playElem* ClientPlay){
	
	int ret, filedesEx, i, j;
	char extractNumber[5][3];
	int winBetIndex = -1;
	int tmp_WinBetIndex;
	int cityIndex = 0;
	int Tutte = 0;
	char extractCity[BUFFER_SIZE];
	char tmp_city[BUFFER_SIZE];
	
	for(i = 0; i < 5; i++){
		ClientPlay->Nbets[i] = 0.00;
		strcpy(ClientPlay->bets[i], "0");
	}

	FILE* extractionFile;

	// Open the actual extraction File
	extractionFile = fopen("./txt/actual_extraction.txt", "r"); 

	if(extractionFile == NULL){
		printf("%d: !!! Error: checkingPlay() --> fopen()\n", pidEx);
		return 1;
	}

	filedesEx = fileno(extractionFile);
	ret = fcntl(filedesEx, F_SETLKW, &client_rdlock);
	if(ret == -1){
		fprintf(stderr, "%d:", pidEx);
		perror(" !!! checkingPlay() ---> fcntl()");		
		fclose(extractionFile);	
		return 1;
	} 

	//fill the struct
	ret = checkingPlayNumbers(numbers_in, ClientPlay);
	if(ret == 1){
		printf("%d: !!! Error: checkingPlay() ---> checkingPlayNumbers\n", pidEx);
		fcntl(filedesEx, F_SETLKW, &client_unlock);
		fclose(extractionFile);
		return 1;	
	}	

	ret = checkingPlayBets(bets_in, ClientPlay);
	if(ret == 1){
		printf("%d: !!! Error: checkingPlay() ---> checkingPlayBets\n", pidEx);
		fcntl(filedesEx, F_SETLKW, &client_unlock);
		fclose(extractionFile);
		return 1;	
	}	

	ret = checkingPlayCities(cities_in, ClientPlay);

	if(ret == 1){
		printf("%d: !!! Error: checkingPlay() ---> checkingPlayCities\n", pidEx);
		fcntl(filedesEx, F_SETLKW, &client_unlock);
		fclose(extractionFile);
		return 1;	
	}	

	if ( strcasecmp(ClientPlay->cities[cityIndex], "Tutte") == 0 ){
		Tutte = 1;
	}

	while(cityIndex < ClientPlay->Ncities){
		
		ret = fseek(extractionFile, 0, SEEK_SET);
		if (ret != 0)
		{
			printf("%d: !!! Error: fseek \n", pidEx);
			fcntl(filedesEx, F_SETLKW, &client_unlock);
			fclose(extractionFile);
			return 1;	
		}

	    while(1)
		{

			ret = fscanf(extractionFile, "%s %s %s %s %s %s\n", extractCity, extractNumber[0], extractNumber[1], extractNumber[2], extractNumber[3], extractNumber[4] );
			if(ret == EOF){
				//City not found
				printf("%s\n", extractCity);
				printf("%d: !!! Error: City not found\n", pidEx);
				fcntl(filedesEx, F_SETLKW, &client_unlock);
				fclose(extractionFile);
				return 1;		
			}
			if(ret != 6){
				printf("%d: !!! Error: checkingPlay() --> fscanf ret = %d\n", pidEx, ret);	
				fcntl(filedesEx, F_SETLKW, &client_unlock);
				fclose(extractionFile);
				return 1;		
			}
			//City match
			if( Tutte == 1 || (strcasecmp(ClientPlay->cities[cityIndex], extractCity) == 0)){

				//check how many numbers the client guessed on a city
				winBetIndex = -1;
				tmp_WinBetIndex = 0;
				for(i = 0; i < ClientPlay->Nnumbers; i++){
					for(j = 0; j < 5; j++ ){
						if( strcmp(ClientPlay->numbers[i], extractNumber[j]) == 0 ){
							if( strcmp( ClientPlay->bets[tmp_WinBetIndex], "0") != 0	){	//if the client betted on that combination
								winBetIndex = tmp_WinBetIndex;	//It will save the hight guessed bet
							}
							tmp_WinBetIndex++;	//How many numbes the client guessed
							
						}
					}
				}
				if(winBetIndex != -1){ // if the client won some combination
	
					//Save how many  "Estratto", "Ambo", etc, the client won
					ClientPlay->Nbets[winBetIndex] += 1;
					if( Tutte == 1){
						strcpy(ClientPlay->cities[0], "1 Tutte");
						cityIndex++;
						if(cityIndex < 10){
							continue;
						}
						fcntl(filedesEx, F_SETLKW, &client_unlock);
						fclose(extractionFile);
						return 0;
					}


					strcpy(tmp_city, ClientPlay->cities[cityIndex]);
					snprintf(ClientPlay->cities[cityIndex], BUFFER_SIZE, "1 %s", tmp_city);	//So I can say that the client won on this particular city
				}
				else{
					if( Tutte == 1){
						if(strcasecmp(ClientPlay->cities[0], "Tutte") == 0){
							strcpy(ClientPlay->cities[0], "0 Tutte");
							cityIndex++;
							continue;
						}
						if(cityIndex < 10){
							cityIndex++;
							continue;
						}
						fcntl(filedesEx, F_SETLKW, &client_unlock);
						fclose(extractionFile);
						return 0;
					}


					strcpy(tmp_city, ClientPlay->cities[cityIndex]);
					snprintf(ClientPlay->cities[cityIndex], BUFFER_SIZE, "0 %s", tmp_city);	//The client didn't win on this city
				}
				cityIndex++;
				break;
			}
		}
	}
	fcntl(filedesEx, F_SETLKW, &client_unlock);
	fclose(extractionFile);
	return 0;
	// Now we the struct is complete	1) array(numbers) with all the numbers that the client played
	//									2) value(Nnumbers) with how many numbers the client played
	//									3) array(Nbets) with all of the bet that the client won
	//									4) array(bets) with the client's bets
	//									5) array(cities) with the cities and the information that says if the client won on that city
	//									6) value(Ncities) with how many cities the client played
}

int checkingPlayNumbers(char* numbers_in, playElem* ClientPlay){
	int number_len, argument_len;
	char numbers[BUFFER_SIZE];
	char* argument;

	ClientPlay->Nnumbers = 0;
	strcpy(numbers, numbers_in);

	number_len = strlen(numbers);
    strtok(numbers, " ");// n
   	if (strcasecmp(numbers, "n") != 0){
   		printf("%d: !!! Error: checkingPlayNumbers() --> n divider\n", pidEx);
		return 1;
	}
    argument_len = strlen(numbers);
    number_len -= argument_len + 1;

    while(number_len > 0 ){
        argument = strtok(NULL, " ");
        argument_len = strlen(argument);
        number_len -= argument_len + 1;

        strcpy(ClientPlay->numbers[ClientPlay->Nnumbers], argument);
        ClientPlay->Nnumbers++;   
    }
    return 0;

    //	Now we fill the struct values	1) array(numbers) with all the numbers that the client played
    //									2) Nnumbers that has how many numbers the client played
}

int checkingPlayBets(char* bets_in,  playElem* ClientPlay){

	int bets_len, argument_len;
	char bets[BUFFER_SIZE];
	char* argument;
	int betIndex = 0;

	strcpy(bets, bets_in);
	
    //save wich bet the client made
    strcpy(bets, bets_in);
    bets_len = strlen(bets);
    strtok(bets, " ");// i
   	if (strcasecmp(bets, "i") != 0){
   		printf("%d: !!! Error: checkingPlayBets() --> i divider\n", pidEx);
		return 1;
	}
    argument_len = strlen(bets);
    bets_len -= argument_len + 1;

    while(bets_len > 0 ){
        argument = strtok(NULL, " ");
        argument_len = strlen(argument);
        bets_len -= argument_len + 1;
        strcpy(ClientPlay->bets[betIndex], argument);
        betIndex++;
   	}
   	return 0;
   	//	Now we fill the struct values	1) array(bets) in wich there are the bet that the client made 
}

int checkingPlayCities(char* cities_in, playElem* ClientPlay){
	int cities_len, argument_len;
	char cities[BUFFER_SIZE];
	char* argument;

	strcpy(cities, cities_in);
	ClientPlay->Ncities = 0;
	
	cities_len = strlen(cities);
    strtok(cities, " ");// r
   	if (strcasecmp(cities, "r") != 0){
   		printf("%d: !!! Error: checkingPlayCities() --> r divider\n", pidEx);
		return 1;
	}

    argument_len = strlen(cities);
    cities_len -= argument_len + 1;
    //Analize one city at a time
    while(cities_len > 0 ){
    	argument = strtok(NULL, " ");
        argument_len = strlen(argument);
        cities_len -= argument_len + 1;
        if( strcasecmp(argument, "Tutte") == 0){
        	strcpy(ClientPlay->cities[ClientPlay->Ncities], "Tutte");
        	ClientPlay->Ncities = 11;
        	break;
        }

        strcpy(ClientPlay->cities[ClientPlay->Ncities], argument);
        ClientPlay->Ncities++;
    }
	return 0;
    //	Now we fill the struct values	1) array(cities) with all the cities that the client played
    //									2) value Ncities with how many cities the client played
}

void CalculateMoneyWin( playElem* ClientPlay){

	int NbetsIndex, n, k, SminusK, possibleGeneration;
	float combinationMoneyWin[5] = { 11.23, 250, 4500, 120000, 6000000};
	float winningBet;
	float final_Win;

	for(NbetsIndex = 0; NbetsIndex < ClientPlay->Nnumbers; NbetsIndex++){
		winningBet = atof(ClientPlay->bets[NbetsIndex]);
		//Combination without repetition
		n = factorial(ClientPlay->Nnumbers);
		k = factorial(NbetsIndex + 1);
		SminusK = factorial( (ClientPlay->Nnumbers) - (NbetsIndex + 1) );
		possibleGeneration = n /( k * SminusK);

		final_Win = (winningBet * ClientPlay->Nbets[NbetsIndex] * combinationMoneyWin[NbetsIndex]) / (possibleGeneration * ClientPlay->Ncities);
		ClientPlay->Nbets[NbetsIndex] = final_Win;
	}
	// Now in the struct, the array "NbetsIndex" has the wins of the play
}

int printCheckedPlay(char* username, playElem* ClientPlay){

	int ret, wr_len, filedesU, i;
	char* extension = ".txt";
	char* path="./txt/";
	time_t ltime; 
	char tmp_argument[BUFFER_SIZE];
	char tmp_argument_bet[BUFFER_SIZE];
 	
 	ltime=time(NULL);

	FILE* userFile;

	// create the path and open the user File
	char usernamePath[strlen(path)+ strlen(username)+strlen(extension)+1];
	snprintf( usernamePath, sizeof( usernamePath ), "%s%s%s", path, username, extension );
	userFile = fopen(usernamePath, "a");  

	if(userFile == NULL){
		printf("%d: !!! Error: printCheckedPlay() --> fopen()\n", pidEx);
		return 1;
	}

	filedesU = fileno(userFile);
	ret = fcntl(filedesU, F_SETLKW, &client_wrlock);
	if(ret == -1){
		fprintf(stderr, "%d:", pidEx);
		perror(" !!! printCheckedPlay() ---> fcntl()");		
		fclose(userFile);	
		return 1;
	} 

	sprintf(tmp_argument, "%ld", ltime);
	
	wr_len = strlen(tmp_argument);
	ret = fprintf(userFile, "%s -r ", tmp_argument);

	if( (ret != (wr_len + 4) ) || (ret < 0) ){
		printf("%d: !!! Error: printCheckedPlay() --> fprintf()_timestamp ret = %d \n", pidEx, ret);
		fcntl(filedesU, F_SETLKW, &client_unlock);	
		fclose(userFile);
		return 1;	
	}

	for(i = 0; i < ClientPlay->Ncities; i++){

		wr_len = strlen(ClientPlay->cities[i]);
		ret = fprintf(userFile, "%s ", ClientPlay->cities[i]);

		if( (ret != (wr_len + 1) ) || (ret < 0) ){
			printf("%d: !!! Error: printCheckedPlay() --> fprintf()_cities ret = %d \n", pidEx, ret);
			fcntl(filedesU, F_SETLKW, &client_unlock);	
			fclose(userFile);
			return 1;	
		}
		if( (strcasecmp( ClientPlay->cities[0], "0 Tutte") == 0 ) || ( strcasecmp(ClientPlay->cities[0], "1 Tutte") == 0 ) ) {
			break;
		}
	}

	ret = fprintf(userFile, "-n ");

	if( (ret != 3) || (ret < 0) ){
		printf("%d: !!! Error: printCheckedPlay() --> fprintf()_N.divider ret = %d \n", pidEx, ret);
		fcntl(filedesU, F_SETLKW, &client_unlock);	
		fclose(userFile);
		return 1;	
	}

	for(i = 0; i < ClientPlay->Nnumbers; i++){

		//sprintf(tmp_argument, "%s", ClientPlay->numbers[i]);
		wr_len = strlen(ClientPlay->numbers[i]);
		ret = fprintf(userFile, "%s ", ClientPlay->numbers[i]);

		if( (ret != (wr_len + 1) ) || (ret < 0) ){
			printf("%d: !!! Error: printCheckedPlay() --> fprintf()_numbers ret = %d \n", pidEx, ret);
			fcntl(filedesU, F_SETLKW, &client_unlock);	
			fclose(userFile);
			return 1;	
		}
	}
	
	ret = fprintf(userFile, "-i ");

	if( (ret != 3) || (ret < 0) ){
		printf("%d: !!! Error: printCheckedPlay() --> fprintf()_I.divider ret = %d \n", pidEx, ret);
		fcntl(filedesU, F_SETLKW, &client_unlock);	
		fclose(userFile);
		return 1;	
	}

	for(i = 0; i < 5; i++){

		sprintf(tmp_argument, "%s", ClientPlay->bets[i]);
		sprintf(tmp_argument_bet, "%.2f", ClientPlay->Nbets[i]);

		wr_len = strlen(tmp_argument) + strlen(tmp_argument_bet);
		ret = fprintf(userFile, "%s %s ", tmp_argument, tmp_argument_bet);

		if( (ret != (wr_len + 2) ) || (ret < 0) ){
			printf("%d: !!! Error: printCheckedPlay() --> fprintf()_bets ret = %d \n", pidEx, ret);
			fcntl(filedesU, F_SETLKW, &client_unlock);	
			fclose(userFile);
			return 1;	
		}
	}
	ret = fprintf(userFile, "\n");

	if( (ret != 1) || (ret < 0) ){
		printf("%d: !!! Error: printCheckedPlay() --> fprintf()_NewLine ret = %d \n", pidEx, ret);
		fcntl(filedesU, F_SETLKW, &client_unlock);	
		fclose(userFile);
		return 1;	
	}


	fcntl(filedesU, F_SETLKW, &client_unlock);
	fclose(userFile);
	return 0; 
}

int printPastExtraction(){

	int ret, wr_len, filedesTMP, filedesPST, filedesAE;
	char extraction[BUFFER_SIZE];

    FILE* actual_extraction;
	FILE *tmp_past_extraction;
	FILE *past_extraction;

	//actual_extraction to copy
	actual_extraction = fopen("./txt/actual_extraction.txt", "r");

	if(actual_extraction == NULL){
		printf("%d: !!! Error: printPastExtraction() --> fopen()\n", pidEx);
		return 1;
	}

	filedesAE = fileno(actual_extraction);
	ret = fcntl(filedesAE, F_SETLKW, &client_rdlock);
	if(ret == -1){
		fprintf(stderr, "%d:", pidEx);
		perror(" !!! printPastExtraction() ---> fcntl()");		
		fclose(actual_extraction);	
		return 1;
	} 

	//new file
	tmp_past_extraction = fopen("./txt/tmp_past_extraction.txt", "a");
	if(tmp_past_extraction == NULL){
		fprintf(stderr, "%d:", pidEx);
		perror(" !!! printPastExtraction() --> fopen(tmp_past_extraction)");	
		fcntl(filedesAE, F_SETLKW, &client_unlock);	
		fclose(actual_extraction);
		return 1;
	}

	filedesTMP = fileno(tmp_past_extraction);	
	ret = fcntl(filedesTMP, F_SETLKW, &client_wrlock);
	if(ret == -1){
		fprintf(stderr, "%d:", pidEx);
		perror(" !!! printPastExtraction() --> fcntl(filedesTMP)");	
		fcntl(filedesAE, F_SETLKW, &client_unlock);	
		fclose(actual_extraction);
		fclose(tmp_past_extraction);
		return 1;
	}
	//Print the actual extraction in a new file
	while(fgets(extraction, BUFFER_SIZE, actual_extraction) != NULL ){		
		wr_len = strlen(extraction);
		ret = fprintf(tmp_past_extraction, "%s", extraction);

		if( (ret != wr_len ) || (ret < 0) ){
			printf("%d: !!! Error: printPastExtraction() --> fprintf(0) ret = %d \n", pidEx, ret);
			fcntl(filedesTMP, F_SETLKW, &client_unlock);	
			fcntl(filedesAE, F_SETLKW, &client_unlock);	
			fclose(actual_extraction);
			fclose(tmp_past_extraction);
			return 1;	
		}
	}
	//Close actual_extraction
	fcntl(filedesAE, F_SETLKW, &client_unlock);	
	fclose(actual_extraction);

	//Open past_extraction
	past_extraction = fopen("./txt/past_extraction.txt", "r");
	if(past_extraction == NULL){
		fprintf(stderr, "%d:", pidEx);
		perror(" !!! printPastExtraction() --> fopen(past_extraction)");	
		return 1;
	}

	filedesPST = fileno(past_extraction);	
	ret = fcntl(filedesPST, F_SETLKW, &client_rdlock);
	if(ret == -1){
		fprintf(stderr, "%d:", pidEx);
		perror(" !!! printPastExtraction() --> fcntl(filedesPST)");	
		fcntl(filedesTMP, F_SETLKW, &client_unlock);	
		fclose(tmp_past_extraction);
		fclose(past_extraction);
		return 1;
	}

	//print the old extraction in the new file
	while(fgets(extraction, BUFFER_SIZE, past_extraction) != NULL ){	
		wr_len = strlen(extraction);
		ret = fprintf(tmp_past_extraction, "%s", extraction);

		if( (ret != wr_len ) || (ret < 0) ){
			printf("%d: !!! Error: printPastExtraction() --> fprintf(1) ret = %d \n", pidEx, ret);
			fcntl(filedesTMP, F_SETLKW, &client_unlock);
			fcntl(filedesPST, F_SETLKW, &client_unlock);	
			fclose(tmp_past_extraction);
			fclose(past_extraction);
			return 1;	
		}
	}

	fcntl(filedesTMP, F_SETLKW, &client_unlock);
	fcntl(filedesPST, F_SETLKW, &client_unlock);	
	fclose(tmp_past_extraction);
	fclose(past_extraction);

	//removing old file
	if( remove( "./txt/past_extraction.txt" ) != 0 ){
		fprintf(stderr, "%d:", pidEx);
    	perror( " !!! Error deleting old past_extraction.txt" );
	}
	
	//renaming new file as old file
 	if ( rename ("./txt/tmp_past_extraction.txt", "./txt/past_extraction.txt") != 0 ){
		fprintf(stderr, "%d:", pidEx);
   		perror( " !!! Error renaming tmp_past_extraction.txt" );
	}

  	return 0;
}

int factorial(int n)
{
  if (n < 0) return -1;

  if (n == 0) return 1;
  else return n*factorial(n-1);
}