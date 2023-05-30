#include <iostream>
#include <string>

struct HTTPHeader;
{
    std :: string name;
    std :: string value;
};

class HTTPRequest
{
    private:
        std :: string method;
        std :: string uri;
        std :: string protocol;
        std :: string body;
        HTTPHeader *headers;
    public:
        
}