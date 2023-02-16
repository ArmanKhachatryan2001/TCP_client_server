#ifndef SERVER_PROTOTYPE_H
#define SERVER_PROTOTYPE_H
using FuncPtr = void(*)(int, std::vector<int>&, std::map<std::string, int>&);
bool syntax_name(const std::string&);
bool find_username(std::string, const std::vector<int>&, const std::map<std::string, int>&, int);
void server(int, std::vector<int>&, std::map<std::string, int>&);
std::string client_name_line(int, std::map<std::string, int>&, const std::vector<int>&);
void message_client(const std::vector<int>&, std::map<std::string, int>&);
void delete_client_name(const std::vector<int>&, std::map<std::string, int>&);
void client_listen(std::vector<FuncPtr>&, std::vector<int>&, std::map<std::string, int>&, std::vector<std::thread>&, int);

#include "server_implementation.h"
#endif //SERVER_PROTOTYPE_H
