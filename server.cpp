#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>
#include <map>
#include <thread>
#include <mutex>
#include <ctime>

#define MULTICAST_ADDR "239.0.0.1"
#define PORT 12345
#define BUFFER_SIZE 1024
#define LOG_INTERVAL 1
#define TIMEOUT_SECONDS 1

std::map<std::string, std::chrono::steady_clock::time_point> lastSeen;
std::mutex lastSeenMutex;

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    int reuse = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));

    sockaddr_in localAddr{};
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(PORT);
    localAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr*)&localAddr, sizeof(localAddr)) < 0) {
        perror("bind");
        return 1;
    }

    sockaddr_in mcastAddr{};
    mcastAddr.sin_family = AF_INET;
    mcastAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, MULTICAST_ADDR, &mcastAddr.sin_addr);

    char buffer[BUFFER_SIZE];

    while (true) {

        time_t current_time = time(nullptr);
        char* time_string = ctime(&current_time);
        time_string[strlen(time_string)-1] = '\0';
        std::cout << "SERVER: sending multicast ping: "<< time_string << std::endl;
        std::string ping = "ping: ";
        std::string msg = ping + time_string;
        sendto(sock, msg.c_str(), msg.size(), 0, (struct sockaddr*)&mcastAddr, sizeof(mcastAddr));

        auto start = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - start < std::chrono::seconds(2)) {
            sockaddr_in clientAddr{};
            socklen_t addrlen = sizeof(clientAddr);
            int len = recvfrom(sock, buffer, BUFFER_SIZE - 1, MSG_DONTWAIT, (struct sockaddr*)&clientAddr, &addrlen);
            if (len > 0) {
                buffer[len] = '\0';
                std::string ip = inet_ntoa(clientAddr.sin_addr);
                std::string reply(buffer);

                {
                    std::lock_guard<std::mutex> lock(lastSeenMutex);

                    if (reply == "pong") {
                        std::cout << "SERVER: " << ip << " is alive" << std::endl;
                        lastSeen[ip] = std::chrono::steady_clock::now();
                    }
                }
            }
        }

        {
            std::lock_guard<std::mutex> lock(lastSeenMutex);
            auto now = std::chrono::steady_clock::now();
            for (auto it = lastSeen.begin(); it != lastSeen.end(); ++it) {
                if (now - it->second > std::chrono::seconds(TIMEOUT_SECONDS)) {
                    std::cout << "SERVER: " << it->first << " is dead (no pong in " << TIMEOUT_SECONDS << " seconds)" << std::endl;
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(LOG_INTERVAL));
    }

    close(sock);
    return 0;
}