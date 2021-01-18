#include "../header/lotto_server.h"

int signupClient(char* command, int client_sock)
{
	int ret, seek_result;
	char buffer_out[BUFFER_SIZE];
	char* username;
	char* password;
	char sessionID[SESSION_SIZE]="offline";

	strtok(command, " ");
	username = strtok(NULL, " ");
	password = strtok(NULL, " ");
	
	seek_result = seekEntry(username, password); // we check if client is already registered
	if(seek_result == 1){
		printf("%d: !!! Error: registerClient() --> seekEntry()\n", pid);
		return 1;	
	}	
	
	// client already registered
	if( seek_result == 2){
		strcpy(buffer_out, "Client already registered");
	}

	// new client
	if( seek_result == 0 )
	{
		printf("%d:...Registering new client...\n", pid);

		ret = insertEntry(username, password, sessionID);
		if(ret == 1){
			printf("%d: !!! Error: registerClient() --> insertNewEntry(0)\n", pid);
			return 1;		
		}
	
		printf("%d:-> Client %s has been registered!\n", pid, username);
		strcpy(buffer_out, "Registration successful!");
	}
	
	ret = sendTCP(buffer_out, client_sock);
	if(ret != 0){
		printf("%d: !!! Error: signupClient() --> sendTCP()\n", pid);
		return 1;		
	}
	return 0;
}


int seekEntry(char* in_user, char* in_password)
{
	int ret, filedes;
	char username[BUFFER_SIZE];
	char password[BUFFER_SIZE];
	char sessionID[SESSION_SIZE];
	
	FILE* fd;
	
	fd = fopen("./txt/clients.txt", "r");
	if(fd == NULL){
		printf("%d: !!! Error: OpenFile--> fopen()\n", pid);
		return 1;
	}
	//acquiring the lock
	filedes = fileno(fd);
	ret = fcntl(filedes, F_SETLKW, &client_rdlock);
	if(ret == -1){
		fprintf(stderr, "%d:", pid);
		perror(" !!! seekEntry() --> fcntl()");
		fclose(fd);
		return 1;	
	}
	while(1)
	{
		ret = fscanf(fd, "%s %s %s\n", username, password, sessionID );
		if(ret == EOF){
			//new client
			printf("%d:...Client %s not found\n", pid, in_user);
			fcntl(filedes, F_SETLKW, &client_unlock);			
			fclose(fd);
			return 0;		
		}
		if(ret != 3){
			printf("%d: !!! Error: seekEntry --> fscanf ret = %d\n", pid, ret);	
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 1;		
		}
		if(strcasecmp(username, in_user) == 0){
			//client already registered
			printf("%d:...Client %s already registered\n", pid, in_user);
			fcntl(filedes, F_SETLKW, &client_unlock);
			fclose(fd);
			return 2;
		}
	}
	
	fcntl(filedes, F_SETLKW, &client_unlock);			
	fclose(fd);
	return 0;
}