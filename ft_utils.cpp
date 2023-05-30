#include <iostream>
#include <cstring>

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
    request = request.substr(0, request.find('\r'));
    int i = request.find(' ');
    int j = request.find(' ', i + 1);
    std::string uri = request.substr(i + 1, j - i - 1);
    if (i != std::string::npos) {
        uri = request.substr(0, i+1);
    }
    return (uri.c_str());
}

int main()
{
    std :: cout << find_uri("GET /search?platform=Windows&category=office") << std :: endl;
    if (isValidHTTPMethod(find_methode("GET /search?platform=Windows&category=office")))
        std :: cout << "coucou\n";
    else 
        std :: cout << "salut\n";
}