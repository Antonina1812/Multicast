// #include <iostream>
// #include <string>
// #include <cstring>
// #include <unistd.h>
// #include <arpa/inet.h>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <sys/types.h>

// #define MULTICAST_ADDR "239.0.0.1"
// #define PORT 12345
// #define BUFFER_SIZE 1024

// int main() {
//     int sock = socket(AF_INET, SOCK_DGRAM, 0);
//     if (sock < 0) {
//         perror("socket");
//         return 1;
//     }

//     int reuse = 1;
//     if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0) {
//         perror("setsockopt SO_REUSEADDR");
//         return 1;
//     }

//     sockaddr_in localAddr{};
//     localAddr.sin_family = AF_INET;
//     localAddr.sin_port = htons(PORT);
//     localAddr.sin_addr.s_addr = htonl(INADDR_ANY);

//     if (bind(sock, (struct sockaddr*)&localAddr, sizeof(localAddr)) < 0) {
//         perror("bind");
//         return 1;
//     }

//     ip_mreq mreq{};
//     mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDR);
//     mreq.imr_interface.s_addr = htonl(INADDR_ANY);
//     if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq)) < 0) {
//         perror("setsockopt IP_ADD_MEMBERSHIP");
//         return 1;
//     }

//     char buffer[BUFFER_SIZE];
//     // Получаем IP через системный вызов
//     FILE* fp = popen("ip -4 addr show eth0 | grep -oP '(?<=inet\\s)\\d+(\\.\\d+){3}'", "r");
//     char myIP[INET_ADDRSTRLEN];
//     fgets(myIP, sizeof(myIP), fp);
//     pclose(fp);

//     // Удаляем перенос строки
//     myIP[strcspn(myIP, "\n")] = 0;
//     std::cout << "[CLIENT "<< myIP <<"] Started and waiting for multicast messages..." << std::endl;

//     while (true) {
//         sockaddr_in senderAddr{};
//         socklen_t senderLen = sizeof(senderAddr);

//         int len = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0, (sockaddr*)&senderAddr, &senderLen);
//         if (len < 0) continue;

//         buffer[len] = '\0';
//         std::string msg(buffer);
//         std::string server_ip = inet_ntoa(senderAddr.sin_addr);

//         std::cout << "[CLIENT] Received: " << msg << " from server" << std::endl;

//         if (msg.find("ping: ") == 0) {
//             std::string pong = "pong";
//             // Получаем IP через системный вызов
//             FILE* fp = popen("ip -4 addr show eth0 | grep -oP '(?<=inet\\s)\\d+(\\.\\d+){3}'", "r");
//             char myIP[INET_ADDRSTRLEN];
//             fgets(myIP, sizeof(myIP), fp);
//             pclose(fp);
    
//             // Удаляем перенос строки
//             myIP[strcspn(myIP, "\n")] = 0;

//             sendto(sock, pong.c_str(), pong.size(), 0, (sockaddr*)&senderAddr, sizeof(senderAddr));
//             std::cout << "[CLIENT " << myIP << "] Sent pong to server" << std::endl;
//         }
//     }

//     close(sock);
//     return 0;
// }

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define MULTICAST_ADDR "239.0.0.1"
#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    int reuse = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0) {
        perror("setsockopt SO_REUSEADDR");
        return 1;
    }

    sockaddr_in localAddr{};
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(PORT);
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr*)&localAddr, sizeof(localAddr)) < 0) {
        perror("bind");
        return 1;
    }

    ip_mreq mreq{};
    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDR);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq)) < 0) {
        perror("setsockopt IP_ADD_MEMBERSHIP");
        return 1;
    }

    char buffer[BUFFER_SIZE];
    FILE* fp = popen("ip -4 addr show eth0 | grep -oP '(?<=inet\\s)\\d+(\\.\\d+){3}'", "r");
    char myIP[INET_ADDRSTRLEN];
    fgets(myIP, sizeof(myIP), fp);
    pclose(fp);

    myIP[strcspn(myIP, "\n")] = 0;
    std::cout << "[CLIENT "<< myIP <<"] Started and waiting for multicast messages..." << std::endl;

    while (true) {
        sockaddr_in senderAddr{};
        socklen_t senderLen = sizeof(senderAddr);

        int len = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0, (sockaddr*)&senderAddr, &senderLen);
        if (len < 0) continue;

        buffer[len] = '\0';
        std::string msg(buffer);
        std::string server_ip = inet_ntoa(senderAddr.sin_addr);

        std::cout << "[CLIENT] Received: " << msg << " from server" << std::endl;

        if (msg.find("ping: ") == 0) {
            std::string pong = "pong";
            FILE* fp = popen("ip -4 addr show eth0 | grep -oP '(?<=inet\\s)\\d+(\\.\\d+){3}'", "r");
            char myIP[INET_ADDRSTRLEN];
            fgets(myIP, sizeof(myIP), fp);
            pclose(fp);
            myIP[strcspn(myIP, "\n")] = 0;

            sendto(sock, pong.c_str(), pong.size(), 0, (sockaddr*)&senderAddr, sizeof(senderAddr));
            std::cout << "[CLIENT " << myIP << "] Sent pong to server" << std::endl;
        }
    }

    close(sock);
    return 0;
}