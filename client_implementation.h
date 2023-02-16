void print(int client_socket, std::string name)
{
    while (true) {
        char arr[1024];
        size_t res2 = recv(client_socket, arr, sizeof(arr), 0);
        if (res2 == -1) {
            std::cout << "Error reciving message cliner \n";
            return;
        }

        std::string tmp;
        if (res2 > 1) {
            std::cout << arr << '\n';
        } else if (res2 == 0) {
            return;
        }
    }
}

void fix_line(std::string& line)
{
    int i = 0;
    while (line[i] == ' ') {
        ++i;
    }

    if (i > 0) {
        line.erase(0, i);
        i = 0;
    }

    while (line[i] && i < 10) {
        if (line[0] != '_') {
            return;
        }
        if (line[i] != ':') {
            ++i;
        } else {
            if (line[++i] == ' ') {
                return;
            } else {
                line.insert(i, " ");
                return;
            }
        }
    }
}
