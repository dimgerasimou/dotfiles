#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // For close()
#include <cstring>
#include <pthread.h>
#include <vector>
#include <mutex>
#include <algorithm>
 
using namespace std;
 
#define PORT 3535
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 15
 
vector<int> clients; //List of connected clients
mutex clients_mutex; //mutex for thread safety
 
void broadcastMessage(const string& message, int sender_socket);
void* handleClient(void* socket_desk);
 
int main() {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        cerr << "Error creating socket" << endl;
        return -1;
    }
 
    int opt = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        cerr << "Error setting socket options: " << strerror(errno) << endl;
        close(sock_fd);
        return -1;
    }
 
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
 
    if (bind(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Error binding socket" << endl;
        close(sock_fd);
        return -1;
    }
 
    if (listen(sock_fd, MAX_CLIENTS) < 0) {
        perror("listsen");
        return -1;
    };
 
    cout << "Listening on port " << PORT << "..." << endl;
 
    while (true) {
        int new_sock = accept(sock_fd, nullptr, nullptr);
        if (new_sock < 0) {
            cerr << "Error accepting connection: " << strerror(errno) << endl;
            continue; // Continue to accept new connections
        }
 
        cout << "---------------------------" << endl;
        cout << "Connection established." << endl;
        cout << "---------------------------" << endl;
 
        {
            lock_guard<mutex> lock(clients_mutex); // LOCK THE MUTEX USING lock_guard
            clients.push_back(new_sock); // ADD NEW CLIENT TO THE LIST
        }
 
        pthread_t thread;
        int *new_sock_ptr = new int(new_sock);
        if (pthread_create(&thread, nullptr, handleClient, &new_sock_ptr) < 0) {
            perror("Could not create thread");
            close(new_sock);
            continue;
        } else cout << "Thread created" << endl;
 
        if (pthread_detach(thread) != 0) {
            perror("Failed to detatch thread");
            continue;
        } else cout << "Thread detatched" << endl;
    }
    close(sock_fd); 
    return 0;
}
 
void broadcastMessage(const string& message, int sender_socket) {
    lock_guard<mutex> lock(clients_mutex);
    for (int client_socket : clients) {
        if (client_socket != sender_socket) { // Don't send the message back to the sender
            send(client_socket, message.c_str(), message.size(), 0);
        }
    }
}
 
void* handleClient(void* socket_desk) {
    string client_name = "Anon";
    int* sock = static_cast<int*>(socket_desk);
    char buffer[BUFFER_SIZE];
 
    memset(buffer, 0, BUFFER_SIZE); // Clear the buffer
 
    ssize_t bytes_received = recv(*sock, buffer, BUFFER_SIZE - 1, 0);
 
    if (bytes_received < 0) {
        cerr << "Error receiving name: " << strerror(errno) << endl;
    } else if (bytes_received == 0) {
        cout << "Name Nul." << endl;
    } else {
        client_name.assign(buffer, bytes_received);
    }
 
    while (true) {
        memset(buffer, 0, BUFFER_SIZE); // Clear the buffer
        bytes_received = recv(*sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received < 0) {
            cerr << "Error receiving message: " << strerror(errno) << endl;
        } else if (bytes_received == 0) {
            cout << "Client disconnected!" << endl;
            break;
        } else {
            string message = "@" + client_name + ": " + buffer;
            cout << message << endl;
            broadcastMessage(message, *sock);
        }
    }
 
    {
        lock_guard<mutex> lock(clients_mutex);
        auto it = find(clients.begin(), clients.end(), *sock);
        if (it != clients.end()) {
            cout << "client" << *it << " erased" << endl;
            clients.erase(it);
        }
    }
    close(*sock);
    delete sock;
    return nullptr;
}