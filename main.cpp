#include "webserv.hpp"

int main()
{
    Request re("GET /api/endpoint HTTP/1.1 \\
                Host: example.com \\
                User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36 \\
                Accept: application/json");
   ///std :: cout << re._method;
    return (0);
}