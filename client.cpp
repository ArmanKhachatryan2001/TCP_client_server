#include "include.h"
#include "client_prototype.h"

int main()
{
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    struct sockaddr_in server_address;
    std::memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    int result = connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (result == -1) {
        std::cerr << "Error connecting to server" << std::endl;
        return 1;
    }

    std::string name;
    std::cout << "Enter your name" << std::endl;
    std::getline(std::cin, name);
    name.push_back(':');
    name = "_" + name;
    int tmp = send(client_socket, name.c_str(), name.length(), 0);
    if (tmp == -1) {
        std::cout << "[ERROR] Message cannot be sent!\n";
        return 1;
    }
    name.push_back(' ');

    std::string str = "";
    std::cout << "enter message users:\n";

    std::thread t1(print, client_socket, name);
    while (true) {
        str = "";
        std::getline(std::cin, str);
        if (str == "ex") {
            t1.detach();
            str = "__came out__";
            str = name + str;
            send(client_socket, str.c_str(), str.size() + 1, 0);
            break;
        }

        fix_line(str);
        str = name + str;
        size_t res = send(client_socket, str.c_str(), str.size() + 1, 0);
        if (res == -1) {
            std::cout << "[ERROR] Message cannot be sent!\n";
            break;
        }
    }
    close(client_socket);
    return 0;
}





