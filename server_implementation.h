bool syntax_name(const std::string& name)
{
    if (name[0] == '_' && name[name.size() - 1] == ':') {
        return true;
    }
    return false;
}

bool find_username(std::string line, const std::vector<int>& client_array,const std::map<std::string, int>& client_name, int client_socket)
{
    std::stringstream cut(line);
    std::string cut_name = "";
    std::string tmp;
    cut >> cut_name;
    cut >> tmp;

    auto it = client_name.find(tmp);
    if (it != client_name.end()) {
        line = "";
        std::getline(cut, line);
        cut_name += line;

        int client_server = client_name.at(tmp);
        bool flag = false;
        for (int i = 0; i < client_array.size(); ++i) {
            if (client_array[i] == client_server) {
                flag = true;
                break;
            }
        }
        if (flag) {
            size_t result = send(client_server, cut_name.c_str(), cut_name.size() + 1, 0);
            if (result == -1) {
                std::cout << "[ERROR] Message cannot be sent!\n";
                return false;
            }
            return false;
        } else {
            line = "Server: there is no user named ";
            line += tmp;
            send(client_socket, line.c_str(), line.size() + 1, 0);
            return false;
        }
    } else if (syntax_name(tmp)) {
            line = "Server: there is no user named ";
            line += tmp;
            size_t result = send(client_socket, line.c_str(), line.size() + 1, 0);
            if (result == -1) {
                std::cout << "[ERROR] Message cannot be sent!\n";
            }
            return false;
    }
    return true;
}

void server(int client_socket, std::vector<int>& client_array, std::map<std::string, int>& client_name)
{
    while (true) {
        char arr[1024];
        size_t res1 = recv(client_socket, arr, sizeof(arr), 0);
        if (res1 == -1)
        {
            std::cerr << "Error receiving message" << std::endl;
            return;
        }
        if (res1 > 1) {

            std::cout << arr << '\n';

            if (find_username(arr, std::ref(client_array), std::ref(client_name), client_socket)) {
                for (int i = 0; i < client_array.size(); ++i) {
                    if (client_array[i] != client_socket && client_array[i] != -1) {
                        size_t result = send(client_array[i], arr, sizeof(arr), 0);
                        if (result == -1) {
                            std::cout << "[ERROR] Message cannot be sent!\n";
                            break;
                        }
                    }
                }
            }
        } else if (res1 == 0){
            for (int i = 0; i < client_array.size(); ++i) {
                if (client_array[i] == client_socket) {
                    auto it = client_name.begin();
                    while (it != client_name.end()) {
                        if (it->second != client_socket) {
                            ++it;
                        } else {
                            it->second = -1;
                            break;
                        }
                    }
                    client_array[i] = -1;
                }
            }
            return;
        }
    }
}

std::string client_name_line(int client_socket, std::map<std::string, int>& client_name, const std::vector<int>& client_array)
{
    std::string name = "";
    auto it = client_name.begin();
    while (it != client_name.end()) {
        if (it->second != client_socket) {
            name += it->first;
            name += "\n";
        }
        ++it;
    }

    if (name != "") {
        name.pop_back();
    }
    return name;
}

void message_client(const std::vector<int>& client_array, std::map<std::string, int>& client_name)
{
     int index = client_array.size() - 1;
     if (index > 0) {
         auto it = client_name.begin();
         int client_socket = client_array[index];
         while (it != client_name.end()) {
             if (it->second == client_array[index]) {
                 break;
             } else {
                 ++it;
             }
         }
         int i = 0;
         for (i; i < client_array.size() - 1; ++i) {
             if (client_array[i] != - 1) {
                 std::string line = "User ";
                 line += it->first;
                 line += " is logged in";
                 size_t result = send(client_array[i], line.c_str(), line.size() + 1, 0);
                 if (result == - 1) {
                     std::cout << "[ERROR] Message cannot be sent!\n";
                 }
             }
         }

         std::string name = client_name_line(client_array[i], client_name, client_array);
         if (client_array[i] != - 1) {
            std::string online = "The specified user or users are online.\n" + name;
            size_t result = send(client_array[i], online.c_str(), online.size() + 1, 0);
            if (result == - 1) {
               std::cout << "[ERROR] Message cannot be sent!\n";
            }
         }
    }
}

void delete_client_name(const std::vector<int>& client_array, std::map<std::string, int>& client_name)
{
    int i = 0;
    for (i; i < client_array.size(); ++i) {
        if (client_array[i] != - 1) {
            continue;
        } else {
            auto it = client_name.begin();
            while (it != client_name.end()) {
                if (it->second != client_array[i]) {
                    ++it;
                } else {
                    client_name.erase(it->first);
                    break;
                }
            }
        }
    }
}

void client_listen(std::vector<FuncPtr>& funcs, std::vector<int>& client_array, std::map<std::string, int>& client_name, std::vector<std::thread>& threads, int server_socket)
{
        static int i = 0;
        struct sockaddr_in client_address;
        socklen_t client_address_len = sizeof(client_address);
        int client_socket = 0;

        while (true) {
            client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);
            if (client_socket == -1)
            {
                std::cerr << "Error accepting connection" << std::endl;
            }

            if (i == 0) {
                std::cout << "enter message clienr:\n";
            }

            char name_client[30];
            int valread = read(client_socket, name_client, 10);
            std::string username(name_client, valread);
            client_name[username] = client_socket;

            funcs.push_back(&server);
            client_array.push_back(client_socket);
            delete_client_name(client_array, client_name);

            std::cout << "A client named " <<  username <<  " has connected to the server" << ".IP address [" << client_socket  << "]" <<'\n';

            threads.emplace_back(funcs[i], client_array[i], std::ref(client_array), std::ref(client_name));
            message_client(client_array, client_name);
            ++i;
        }
}
