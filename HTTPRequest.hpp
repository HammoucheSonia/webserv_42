#include <iostream>
#include <string>

struct HTTPHeader;
{
    std :: string name;
    std :: string value;
};

struct HTTPRequest
{
    std :: string method;
    std :: string uri;
    std :: string protocol;
    std :: string body;
    int H_lignes; 
    HTTPHeader headers[H_lignes];       
};

class parse
{
    
}