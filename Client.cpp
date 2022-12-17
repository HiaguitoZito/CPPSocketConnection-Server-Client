#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <math.h>
#include <string>

using namespace std;

typedef struct Credentials{
	string Username;
	string Password;
	int Pin;
} Credentials;

void AskForCredentials(Credentials* cred);
void Serialize(Credentials* cred, char* data);

int main()
{
	// initiate all variables.
    int server;
    int port = 1500;
    int bufsize = 4096;
    char* buffer = new char[bufsize];
    char* ip = "127.0.0.1";
    
    // Starts socket in AF_INET(IPV4) using SOCK_STREAM 0(TCP/IP).
    server = socket(AF_INET, SOCK_STREAM, 0); 
    
	if(server < 0){
		cerr << "Error creating Socket." << endl;
		exit(1);
	}
	cout << "Socket Created." << endl;
	
	struct sockaddr_in server_addr;
	
	server_addr.sin_family = AF_INET; // Sets the address to IPV4 address.
	server_addr.sin_port = htons(port); // Gets the port and convert it to network 16-bit number.
	inet_pton(AF_INET, ip, &server_addr.sin_addr); // Converts the IP address(text) to it's binary form.
	
	cout << "Connecting..." << endl;
	if(connect(server, (struct sockaddr*)&server_addr, sizeof server_addr) == 0)
		cout << "Connected to the port: " << port << endl;
	
	bool isExit = false;
	
	do{
		// User Interface:
		Credentials cred;
		AskForCredentials(&cred);
		
		Serialize(&cred, buffer); // Converts the struct to array of byte and stores it in the buffer.
		
		send(server, buffer, bufsize, 0); // Sends the buffer to the socket stream
		
		// Clears the buffer for later use.
		delete[] buffer;
		buffer = new char[bufsize];
		for(int i = 0; i < bufsize; i++) buffer[i] = 0;
		
		// Recieves output from the server.
		cout << "Server: ";
		int BytesRecieved = recv(server, buffer, bufsize, 0);
		if(BytesRecieved != 0){
			cout << buffer << endl;
		}
		cout << endl;
		
	}while(!isExit);
	
	delete[] buffer; // Clears the memory.
}

void AskForCredentials(Credentials* cred){
		string Username, Password;
		
		cout << "\033[1;31mPlese insert login information.\033[0m\n" << endl;
		
		cout << "\033[1;91mUsername: \033[0m";
		cin >> Username;
		
		cout << "\033[1;91mPassword: \033[0m";
		cin >> Password;
		
		cred->Username = Username;
		cred->Password = Password;
		cred->Pin = 1234; // Test PIN.
}

void Serialize(Credentials* cred, char* data){
	const char* Username = cred->Username.c_str();
	const char* Password = cred->Password.c_str();
	
	for(int i = 0; i < cred->Username.length(); i++){ // Iterates through the "data" pointer and add the characters into a string.
		*data = Username[i];
		data++;
	}
	*data = '\0'; // End of text char.
	data++;
	
	for(int i = 0; i < cred->Password.length(); i++){ // Iterates through the "data" pointer and add the characters into a string.
		*data = Password[i];
		data++;
	}
	*data = '\0'; // End of text char.
	data++;
	
	*data = (int)floor(cred->Pin / (double)256);
	data++;
	*data = (char)(cred->Pin % (short)256);
}
