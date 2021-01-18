#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 2000
#define MAX_COMM_LENGTH 50
#define SESSION_SIZE 10

char username[BUFFER_SIZE];
char sessionID[SESSION_SIZE];


void showMenu();
int requestCommand(char* command, int TCP_sock, char* IP_address);
int requestSignup(char* command, int TCP_sock, char* IP_address);
int requestLogin(char* command, int TCP_sock, char* IP_address);
int requestInvia_giocata(char* command, int TCP_sock);
int requestVedi_giocate(char* command, int TCP_sock);
int requestVedi_estrazione(char* command, int TCP_sock);
int requestVedi_vincite(char* command, int TCP_sock);
int requestEsci(char* command, int TCP_sock);

int receiveTCP(char* buffer_in, int sock);
int sendTCP(char* buffer_out, int sock);
int checkPlay(char* play_in);
int checkCities(char* cities_in);
int checkCity(char* argument);
int checkNumbers(char* numbers_in);
int checkBets(char* bets_in);
int printPlay(char* play_in, int NTimes);
int printWin(char* play_in, char* previousTime);
void printTotalWin(char* totalWin_in);