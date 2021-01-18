#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

#define MAX_QUEUED_CONN 10
#define BUFFER_SIZE 2000
#define MAX_CMD_LENGTH 50
#define SESSION_SIZE 10


typedef struct PlayElem{

	char cities[11][BUFFER_SIZE];
	int Ncities;
	char numbers[10][3];
	int Nnumbers;
	char bets[5][BUFFER_SIZE];
	float Nbets[5];
}playElem;


int pidP, pid, pidEx;
struct flock client_rdlock;
struct flock client_wrlock;
struct flock client_unlock;
char usernameClient[BUFFER_SIZE];

int Extraction();
int executeCommand(char* command, int client_sock);
int signupClient(char* command, int client_sock);
int loginClient(char* command, int client_sock);
int invia_giocata(char* command, int client_sock);
int vedi_giocate(char* argument, int client_sock);
int vedi_estrazione(char* command, int client_sock);
int vedi_vincite(int client_sock);
int esci(int client_sock);

int receiveTCP(char* buffer_in, int sock);
int sendTCP(char* buffer_out, int sock);
int insertEntry(char* username, char* password, char* sessionID);
int getClientIP(char* IP_address_out, int client_sock);
int getClientSessionID(char* username_out, char* sessionID_out, int client_sock);
int removeEntry(char* username_in, char* password_in);
int seekEntry(char* in_user, char* in_password);
char* createSessionID(char sessionID[]);
int checkBlacklist(char* IP_address_in);
int checkClients(char* username_in, char* password_in);
int checkLogs(char* username_in, char* IP_address_in, int attempts_number_in);
int insertLogs(char* username_in ,char* IP_address_in, int attempts_number_in);
int insertBlacklist(char* IP_address_in);
int checkSessionID(char* username_in,char* sessionID_in);
int insertActualPlay(char* username, char* play_in);
int printPastPlay(char* username, int client_sock);
int printPastCities(char* cities_in, char* cities_out);
int printPastNumbers(char* numbers_in, char* numbers_out);
int printPastBets(char* bets_in, char* bets_out);
int printActualPlay(char* username, int client_sock);
int printActualCities(char* cities_in, char* cities_out);
int printActualNumbers(char* numbers_in, char* numbers_out);
int printActualBets(char* bets_in, char* bets_out);
int printExtraction(char* numberExtraction_in, char* City, char* username, int client_sock);
int printWinBets(char* bets_in, char* bets_out, float* moneyWin);
int printWin(char* username, int client_sock);
int printWinCities(char* cities_in, char* cities_out, char* timestamp);
int printWinNumbers(char* numbers_in, char* numbers_out);
int CheckExtraction();
int checkingPlay(char* cities_in, char* numbers_in, char* bets_in, playElem* ClientPlay);
void CalculateMoneyWin( playElem* ClientPlay);
int printCheckedPlay(char* username, playElem* ClientPlay);
int printPastExtraction();
int checkingPlayNumbers(char* numbers_in, playElem* ClientPlay);
int checkingPlayBets(char* bets_in,  playElem* ClientPlay);
int checkingPlayCities(char* cities_in, playElem* ClientPlay);
int factorial(int n);