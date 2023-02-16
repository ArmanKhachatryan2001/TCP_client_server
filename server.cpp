#include "include.h"
#include "server_prototype.h"

int main()
{
    std::vector<FuncPtr> funcs;
    std::vector<int> client_array;
    std::map<std::string, int> client_name;

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    struct sockaddr_in server_address;
    std::memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    int result = bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    if (result == -1)
    {
        std::cerr << "Error binding socket to address" << std::endl;
        return 1;
    }

    result = listen(server_socket, SOMAXCONN);
    if (result == -1) {
        std::cerr << "Error listening on socket" << std::endl;
        return 1;
    }

    std::cout << "Server is waiting for clients\n";

    std::vector<std::thread> threads;
    std::thread client_oll(client_listen, std::ref(funcs), std::ref(client_array), std::ref(client_name), std::ref(threads), server_socket);

    std::string name = "Server: ";
    std::string server_write = "";

    while (true) {
        std::getline(std::cin, server_write);
        if (server_write == "ex") {
            break;
        }

        server_write = name + server_write;
        for (int i = 0; i < funcs.size(); ++i) {
            if (client_array[i] != -1) {
                size_t res2 = send(client_array[i], server_write.c_str(), server_write.size()+1, 0);
                if (res2 == -1) {
                    std::cout << "[ERROR] Message cannot be sent!\n";
                    break;
                }
            }
        }
        server_write = "";
    }

    for (auto& thread : threads) {
        thread.detach();
    }
    client_oll.detach();
    close(server_socket);
    return 0;
}




