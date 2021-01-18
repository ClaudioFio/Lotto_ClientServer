#include "../header/lotto_server.h"


int esci(int client_sock){
	int ret;
	char buffer_out[BUFFER_SIZE];
	char buffer_in[BUFFER_SIZE];
	char username[BUFFER_SIZE];
	char password[BUFFER_SIZE];
	char sessionID[SESSION_SIZE];

	ret = getClientSessionID(username, sessionID, client_sock);
	if(ret == 1){
		printf("%d: !!! Error: esci() --> getClientSessionID()\n", pid);
		return 1;
	}
	//SessionID not valid
	if(ret == 2){
		strcpy(buffer_out, "Your sessionID is not valid");
		printf("%d:-> The client %s used a not valid sessionID\n", pid, username);
		ret = sendTCP(buffer_out, client_sock);
		if(ret != 0){
			printf("%d: !!! Error: esci() --> sendTCP()\n", pid);
			return 1;		
		}
		return 0;
	}

	strcpy(sessionID, "offline");

	ret = removeEntry(username, password);
	if(ret == 1){
		printf("%d: !!! Error: esci() --> removeEntry()\n", pid);
		return 1;	
	}	
	ret = insertEntry(username, password, sessionID);
	if(ret == 1){
		printf("%d: !!! Error: esci() --> insertEntry()\n", pid);
		return 1;	
	}

	// sending "waiting for closure" notification
	strcpy(buffer_out, "Server ready to close connection");
	printf("%d: SessionID not valid anymore for %s\n", pid, username);
	ret = sendTCP(buffer_out, client_sock);
	if(ret != 0){
		printf("%d: !!! Error: esci() --> sendTCP()\n", pid);
		return 1;		
	}
	// waiting for close(TCP_sock), receiving 0
	ret = recv(client_sock, (void*)buffer_in, sizeof(int), 0);

	if(ret == -1){
		fprintf(stderr, "%d:", pid);
		perror(" !!! Command recv() ");
		return 1;
	}
	if(ret == 0){
		printf("%d:-> Received 0 from client, terminating connection\n", pid);
		return 2;	
	}
	return 0;		
}