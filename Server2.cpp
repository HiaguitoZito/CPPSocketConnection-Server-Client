#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <future>

using namespace std;

struct Credentials{ // As credenciais.
	string Username;
	string Password;
	int Pin;
};

// Inicializadores de funções.
void ClientConnect(void* l, int socket);

void Deserialize(char* data, Credentials* cred);

struct listnode { // Data structure baseada em Nodes.
    std::future<void> f;
    listnode* next;
    listnode* prev;
    
    listnode(int socket, listnode* next = nullptr, listnode* prev = nullptr) {
        f = std::async(std::launch::async, ClientConnect, this, socket); // Inicializa a função "ClientConnect()" asyncronamente usando multithreading.
        this->next = next;
        this->prev = prev;
    }
    void AddNext(int socket) { // Adiciona nova Node.
        next = new listnode(socket, nullptr, this);
    }

    void DeleteNode() { // Deleta Node atual.
        prev->next = next;
        next->prev = prev;
    }
};

int main()
{
    int port = 1500;
        
    // Cria o Socket para comunicação.
    cout << "Creating socket Handle..." << endl;
    int socketHandle = socket(AF_INET, SOCK_STREAM, 0);
    
    if(socketHandle < 0){ // Válida se o Socket foi criado com sucesso.
    	cerr << "Error Creating the socket" << endl;
    	exit(1);
    }
    cout << "Socket Created." << endl;
    
    cout << "Creating the socketAddr..." << endl;
    
    sockaddr_in sockAddr;
    // Configura a struct do Endereço socket.
    sockAddr.sin_family = AF_INET; // IPV4
    sockAddr.sin_addr.s_addr = INADDR_ANY; // Qualquer Endereço.
    sockAddr.sin_port = htons(port); // htons converte o inteiro de 16-bits para um inteiro de 16-bits no padrão de rede.
    inet_pton(AF_INET, "0.0.0.0", &sockAddr.sin_addr); // Vincula o endereço de IP em "sockAddr.sin_addr".
    
    cout << "Binding the socket to a ip/port..." << endl;
    // Tenta vincular o handler do socket com a struct de endereço do socket.
    if(bind(socketHandle, (sockaddr*)&sockAddr, sizeof(sockAddr)) < 0){ 
    	cerr << "Error Binding the socket Address." << endl;
    	exit(1);
    }
    cout << "Binded." << endl;
    
    listnode* l = new listnode(socketHandle); // Cria uma nova possibilidade de conexão.
    
    while(true){
    	// loop para manter o servidor funcionando.
    }
    
    return 0;
}

void ClientConnect(void* l, int socket) {
    
    if(listen(socket, SOMAXCONN) < 0){ // Tenta escutar conexões vindo para o socket.
    	new listnode(socket);
    	cerr << "Error Listening to the socket address." << endl;
    	((listnode*)l)->DeleteNode();
    	return;
    }
    
    sockaddr_in client;
    socklen_t clientLen = sizeof(client);
    
    int clientSocket = accept(socket, (sockaddr*)&client, &clientLen); // Aceita a conexão recebida.
    
    if(clientSocket < 0){ // Se não conseguir aceitar retorna um erro.
    	new listnode(socket);
    	cerr << "Error Accepting the socket" << endl;
    	((listnode*)l)->DeleteNode();
    	return;
    }
    
    new listnode(socket); // Inicia nova possibilidade de conexão.
    
    int bufsize = 4096;
    char* buffer = new char[bufsize];
    
    while(true){
    
    	int br = recv(clientSocket, buffer, bufsize, 0); // Recebe dados do cliente.
    	if(br > 0){
    		Credentials cred;
    		
    		if(buffer[0] == 23) break; // Válida de a posição de index 0 do buffer não é um char de finalização de conexão, se for encerra a conexão com o cliente.
    		
    		Deserialize(buffer, &cred); // Converte oque recebeu do buffer para dados de uma struct aonde poderá ser usado para autenticação no nosso caso.
    		
            // Faz o display na tela dos dados recebidos.
    		cout << "Username recieved: " << cred.Username << endl;
    		cout << "Password recieved: " << cred.Password << endl;
    		cout << "   Pin   recieved: " << cred.Pin << endl;
    		
    		for(int i = 0; i < bufsize; i++) buffer[i] = 0;  // Limpa o buffer para uso posterior.
    		
    		string userCredential = "User";
    		string passCredential = "1234";

    		// Aceita ou recusa as credenciais recebidas dependendo de quais o servidor recebeu.
    		if(cred.Username == userCredential && cred.Password == passCredential) send(clientSocket, 
    			(char*)"Accepted", sizeof((char*)"Accepted"), 0);

    		else send(clientSocket, (char*)"Refused", sizeof((char*)"Refused"), 0);
    		
    	}
    }
    close(clientSocket); // Fecha o socket do cliente.
    
    return;
}

void Deserialize(char* data, Credentials* cred){
	string Username = "";
	string Password = "";
	
	while(*data != '\0'){ // Itera sobre todos os elementos dos dados recebidos e converte para um Nome de usuario.
		Username += *data;
		data++;
	}
	data++;
	
	while(*data != '\0'){ // Itera sobre todos os elementos dos dados recebidos e converte para uma Senha.
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
