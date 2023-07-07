#ifndef REQUEST_HPP
#define REQUEST_HPP
#include<map>
#include<sstream>
typedef std::string string;
typedef std::map<std::string, std::string> map;
class Request
{
    private:
	    bool	_chunked;
	    bool	_chunked_error;
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
	    Request(const char *request_str);
	    Request(Request const &src);
	    Request(Request const &src, string body);
	    ~Request();
	    bool is_method_valid() const;
	    bool is_bad_request() const;
	    bool is_CGI() const;
	    bool is_chunked() const;
	    bool is_chunked_false() const;
	    string get_method() const;
	    string get_URI() const;
	    string get_path() const;
	    string get_translated_path() const;
	    string get_host_port() const;
	    string get_host() const;
	    string get_port() const;
	    string get_query_string() const;
	    string get_protocol() const;
	    map get_headers() const;
	    string get_body() const;
	    void append_root_to_path(string root);
	    void set_translated_path(string new_path);
	    void set_unchunked_body(string new_body);
	    void set_chunked_error(bool error);
	    Request &operator=(Request const &rhs);
	    int parse_start_line(string start_line);
	    map parse_headers(const string request_str);
	    string parse_body(const char *request_str);
};

std::ostream &operator<<(std::ostream &o, Request const &i);

#endif