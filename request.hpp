#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include <sstream>

typedef std::string string;
typedef std::map<std::string, std::string> map;

class Request {
private:
    bool _chunked;
    bool _chunked_error;
    string _body;
    string _method;
    string _URI;
    string _host;
    string _port;
    string _host_port;
    string _path;
    string _translated_path;
    string _query_string;
    string _protocol;
    map _headers;

public:
    Request();
    Request(const char* request_str);
    Request(Request const& src);
    Request(Request const& src, string body);
    ~Request();

    bool isMethodValid() const;
    bool isBadRequest() const;
    bool isCGI() const;
    bool isChunked() const;
    bool isChunkedFalse() const;
    string getMethod() const;
    string getURI() const;
    string getPath() const;
    string getTranslatedPath() const;
    string getHostPort() const;
    string getHost() const;
    string getPort() const;
    string getQueryString() const;
    string getProtocol() const;
    map getHeaders() const;
    string getBody() const;
    void appendRootToPath(string root);
    void setTranslatedPath(string new_path);
    void setUnchunkedBody(string new_body);
    void setChunkedError(bool error);
    Request& operator=(Request const& rhs);
    int parseStartLine(string start_line);
    map parseHeaders(const string request_str);
    string parseBody(const char* request_str);
};

std::ostream& operator<<(std::ostream& o, Request const& i);

#endif
