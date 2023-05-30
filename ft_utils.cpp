#include <iostream>
#include <cstring>
#include <vector>

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    size_t start = 0;
    size_t end = str.find(delimiter, start);

    while (end != std::string::npos) {
        token = str.substr(start, end - start);
        tokens.push_back(token);
        start = end + 1;
        end = str.find(delimiter, start);
    }

    token = str.substr(start);
    tokens.push_back(token);

    return tokens;
}

bool isValidHTTPMethod(const char* method) {
    if (strcmp(method, "GET") == 0 ||
        strcmp(method, "POST") == 0 ||
        strcmp(method, "PUT") == 0 ||
        strcmp(method, "DELETE") == 0 ||
        strcmp(method, "HEAD") == 0 ||
        strcmp(method, "OPTIONS") == 0 ||
        strcmp(method, "TRACE") == 0 ||
        strcmp(method, "CONNECT") == 0) {
        return true;
    }
    return false;
}

const char *find_methode(std :: string request)
{
    std :: string method; 
    int i = request.find(' ');
    if (i != std::string::npos) {
        method = request.substr(0, i);
    }
    return (method.c_str());
}

const char *find_uri(std :: string request)
{
    std::vector<std::string> lists = split(request, ' ');
    return (lists[1].c_str());
}

int main()
{
    std :: cout << find_uri("GET /search?platform=Windows&category=office") << std :: endl;
    if (isValidHTTPMethod(find_methode("GET /search?platform=Windows&category=office")))
        std :: cout << "coucou\n";
    else 
        std :: cout << "salut\n";
}