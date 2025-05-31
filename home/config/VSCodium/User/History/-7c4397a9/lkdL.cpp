#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // For close()
#include <string>
#include <cstring>
#include <unistd.h>
 
using namespace std;
 
#define BUFFER_SIZE 1024
#define PORT 3535
 
 
void* serverResponds(void* socket_desk);
 
int main() {
    //char client_name[] = "Alkis";
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        cerr << "Error creating socket" << endl;
        return -1;
    }
 
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
 
    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Error connecting to server" << endl;
        return -1;
    }
 
    cout << "Connected to server" << endl;
 
    pthread_t thread;
    int* sock_ptr = new int(sock_fd);
    if (pthread_create(&thread, nullptr, serverResponds, sock_ptr) < 0) {
        perror("Could not create thread");
    }
 
    pthread_detach(thread);
 
    //char message[BUFFER_SIZE];
    string message;
    cout << "Name: ";
    cin >> message;
    int bytes_sent = send(sock_fd, message.c_str(), message.length(), 0);
    if (bytes_sent < message.length() ) cerr << "Name wasn't sent sucessfully";
    int i = 0;
    while (++i) {
        //if (i != 1) cout << "> ";
        getline(cin, message);
        if (message == "exit") break;
        bytes_sent = send(sock_fd, message.c_str(), message.length(), 0);
        if (bytes_sent < message.length() ) cerr << "Not all bytes were sent";
    }
    close(sock_fd);
    return 0;
}
 
void* serverResponds(void* socket_desk) {
    int* sock = static_cast<int*>(socket_desk);
 
    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, BUFFER_SIZE); // Clear the buffer
        int bytes_received = recv(*sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received < 0) {
            cerr << "Error receiving message from server: " << strerror(errno) << endl;
        } else if (bytes_received == 0) {
            cout << "\n---\nServer disconnected.\n---\n" << endl;
            close(*sock);
            exit(-1);
        } else {
            buffer[bytes_received] = '\0';
            cout << buffer << endl;
        }
    }
    close(*sock);
    delete sock;
    return nullptr;
}

