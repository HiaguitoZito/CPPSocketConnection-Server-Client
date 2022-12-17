#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

using namespace std;
typedef struct Credentials{
	string Username;
	string Password;
	int Pin;
} Credentials;

void Deserialize(char* data, Credentials* cred);

int main()
{
    // init the variables.
    int port = 1500;
    
    
    // Creates the socket.
    cout << "Creating socket Handle..." << endl;
    int socketHandle = socket(AF_INET, SOCK_STREAM, 0);
    
    if(socketHandle < 0){ // Checks if the socket was sucessufully created.
    	cerr << "Error Creating the socket" << endl;
    	exit(1);
    }
    cout << "Socket Created." << endl;
    
    // Bind the socket to a IP/PORT.
    
    cout << "Creating the socketaddr..." << endl;
    
    sockaddr_in sockAddr;
    // Configures the sockAddr struct.
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = INADDR_ANY;
    sockAddr.sin_port = htons(port);
    inet_pton(AF_INET, "0.0.0.0", &sockAddr.sin_addr);
    
    cout << "Binding the socket to a ip/port..." << endl;
    // tries to bind the sockAddr to the socketHandle, if it can't it returns an error
    if(bind(socketHandle, (sockaddr*)&sockAddr, sizeof(sockAddr)) < 0){ 
    	cerr << "Error Binding the socket Address." << endl;
    	exit(1);
    }
    cout << "Binded." << endl;
    
    cout << "Listening to incoming connections..." << endl;
    if(listen(socketHandle, SOMAXCONN) < 0){ // Try to listen to connections.
    	cerr << "Error Listening to the socket address." << endl;
    	exit(1);
    }
    cout << "Connected to the port: " << port << endl;
    
    sockaddr_in client;
    socklen_t clientLen = sizeof(client);
    
    cout << "Accepting the connection..." << endl;
    int clientSocket = accept(socketHandle, (sockaddr*)&client, &clientLen); // Accepts the connection.
    
    if(clientSocket < 0){ // if it couldn't accept then returns an error.
    	cerr << "Error Accepting the socket" << endl;
    	exit(1);
    }
    cout << "Accepted!" << endl;
    close(socketHandle);
    
    int bufsize = 4096;
    char* buffer = new char[bufsize];
    
    while(true){
    
    	int br = recv(clientSocket, buffer, bufsize, 0);
    	if(br > 0){
    		Credentials cred;
    		
    		if(buffer[0] == 23) break; // checks if buffer[0] isn't the "end of stream" byte.
    		
    		Deserialize(buffer, &cred); // Converts the buffer into usable data and stores it in a struct.
    		
    		cout << "Username recieved: " << cred.Username << endl;
    		cout << "Password recieved: " << cred.Password << endl;
    		cout << "   Pin   recieved: " << cred.Pin << endl;
    		
    		for(int i = 0; i < bufsize; i++) buffer[i] = 0; 
    		
    		// Accepts or refuse the credentials.
    		string userCredential = "User";
    		string passCredential = "1234";
    		
    		if(cred.Username == userCredential && cred.Password == passCredential) send(clientSocket, 
    			(char*)"Accepted", sizeof((char*)"Accepted"), 0);
    		else send(clientSocket, (char*)"Refused", sizeof((char*)"Refused"), 0);
    		
    	}
    }
    
    close(clientSocket);
        
    return 0;
}

void Deserialize(char* data, Credentials* cred){
	string Username = "";
	string Password = "";
	
	while(*data != '\0'){ // Iterates through the buffer while it isn't string end char and gets it's data(string).
		Username += *data;
		data++;
	}
	data++;
	
	while(*data != '\0'){ // Iterates through the buffer while it isn't string end char and gets it's data(string).
		Password += *data;
		data++;
	}
	data++;
	
	cred->Username = Username;
	cred->Password = Password;
	cred->Pin = 0;
	
	cred->Pin += *data * 256;
	data++;
	cred->Pin += *data + 256;
}
