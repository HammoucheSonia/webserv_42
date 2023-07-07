#include "webserv.hpp"
#include "request.hpp"

static void displayMap(std::map<std::string, std::string> toDisplay) {
    for (std::map<std::string, std::string>::iterator it = toDisplay.begin(); it != toDisplay.end(); it++) {
        std::cout << '\t' << it->first << " = " << it->second << std::endl;
    }
    std::cout << std::endl;
}

Request::Request() {
    _chunked = false;
    _chunked_error = false;
}

Request::Request(const char* request_array) {
    _chunked = false;
    _chunked_error = false;
    _body = parse_body(request_array);
    std::string request_string = std::string(request_array);
    std::string start_line = std::string(request_string, 0, request_string.find('\n'));
    parse_start_line(request_string);
    _translated_path = std::string();
    _headers = parse_headers(request_string);

    _host_port = _headers["Host"];
    size_t pos;
    if ((pos = _host_port.find(":")) != std::string::npos) {
        _host = std::string(_host_port, 0, pos);
        _port = std::string(_host_port, pos + 1);
        while (!isdigit(_port[_port.size() - 1])) {
            _port = std::string(_port, 0, _port.size() - 1);
        }
    } else {
        _host = _host_port;
        _port = std::string();
    }

    std::string encoding;
    encoding = _headers["Transfer-Encoding"];
    if (encoding == "chunked\r") {
        _chunked = true;
    }
}

Request::Request(const Request& src) {
    _chunked = src._chunked;
    _chunked_error = src._chunked_error;
    _method = src._method;
    _headers = src._headers;
    _host_port = src._host_port;
    _host = src._host;
    _port = src._port;
    _body = src._body;
    _path = src._path;
    _translated_path = src._translated_path;
}

Request::Request(Request const& src, std::string body) {
    _method = src._method;
    _headers = src._headers;
    _host_port = src._host_port;
    _host = src._host;
    _port = src._port;
    _path = src._path;
    _translated_path = src._translated_path;
    _body = body;
}

Request::~Request() {}

Request& Request::operator=(Request const& rhs) {
    if (this != &rhs) {
        _chunked = rhs._chunked;
        _chunked_error = rhs._chunked_error;
        _method = rhs._method;
        _headers = rhs._headers;
        _body = rhs._body;
        _host_port = rhs._host_port;
        _host = rhs._host;
        _port = rhs._port;
        _path = rhs._path;
        _translated_path = rhs._translated_path;
    }
    return *this;
}

std::ostream& operator<<(std::ostream& o, Request const& i) {
    o << "REQUEST-" << std::endl;
    o << "Method: " << i.get_method() << std::endl
      << "URI: " << i.get_URI() << std::endl
      << "Path: " << i.get_path() << std::endl
      << "Query string: " << i.get_query_string() << std::endl
      << "Protocol: " << i.get_protocol() << std::endl
      << "Host: " << i.get_host() << std::endl
      << "Port: " << i.get_port() << std::endl
      << "Headers:" << std::endl;
    displayMap(i.get_headers());
    o << std::endl
      << "Body:" << std::endl
      << i.get_body() << std::endl;
    o << "end" << std::endl;

    return o;
}

int get_header_value(std::map<std::string, std::string>& headers, std::string request, std::string header) {
    size_t pos = 0;
    int len = 0;
    if ((pos = request.find(header + ": ")) != std::string::npos) {
        pos += header.size() + 2;
        while (request[pos + len] && request[pos + len] != '\n') {
            len++;
        }
        headers[header] = std::string(request, pos, len);
    }
    return 0;
}

void init_headers(std::map<std::string, std::string>& headers) {
    headers["Accept-Charset"] = std::string();
    headers["Accept-Language"] = std::string();
    headers["Authorization"] = std::string();
    headers["Content-Length"] = std::string();
    headers["Content-Type"] = std::string();
    headers["Date"] = std::string();
    headers["Host"] = std::string();
    headers["Referer"] = std::string();
    headers["Transfer-Encoding"] = std::string();
    headers["User-Agent"] = std::string();
}

std::map<std::string, std::string> Request::parse_headers(const std::string request_str) {
    std::map<std::string, std::string> headers;

    init_headers(headers);
    get_header_value(headers, request_str, "Accept-Charset");
    get_header_value(headers, request_str, "Accept-Language");
    get_header_value(headers, request_str, "Authorization");
    get_header_value(headers, request_str, "Content-Length");
    get_header_value(headers, request_str, "Content-Type");
    get_header_value(headers, request_str, "Date");
    get_header_value(headers, request_str, "Host");
    get_header_value(headers, request_str, "Referer");
    get_header_value(headers, request_str, "Transfer-Encoding");
    get_header_value(headers, request_str, "User-Agent");

    return headers;
}

string Request::parse_body(const char* request) {
    string requestStr = string(request);
    size_t pos;
    char separator[5] = { 13, 10, 13, 10, 0 };
    pos = requestStr.find(separator);

    if (pos == (requestStr.size() + 4) || pos == string::npos) {
        return string();
    } else {
        return string(requestStr, pos + 4);
    }
}

   

int Request::parse_start_line(string start_line)
{
	string::iterator it_begin = start_line.begin();
	string::iterator it_end = it_begin + start_line.find(" ");
	if (*it_begin && *it_end)
		_method = string(it_begin, it_end++);
	it_begin = it_end;
	while (*it_end && *it_end != ' ')
		it_end++;
	if (*it_begin && *it_end)
		_URI = string(it_begin, it_end++);
	size_t pos;
	size_t len = 0;
	if ((pos = _URI.find('?')) != string::npos)
	{
		_path = string(_URI, 1, pos - 1);
		pos += 1;
		while (start_line[pos + len] && start_line[pos + len] != ' ')
			len++;
		_query_string = string(_URI, pos, len);
	}
	else if (_URI != string())
	{
		_path = string(_URI, 1, string::npos);
		_query_string = string();
	}
	it_begin = it_end;
	while (*it_end && *it_end != '\n')
		it_end++;
	if (*it_begin && *it_end)
		_protocol = string(it_begin, it_end);
	return 0;
}


bool Request::is_bad_request() const
{
	if (_chunked_error == true)
		return true;
	if (get_headers()["Host"] == string())
		return true;
	else
		return false;
}

void Request::append_root_to_path(string root)
{

	if (root == string())
	{
		_translated_path = "./" + _path;
		return;
	}
	while (root.size() >= 1 && root[0] == ('.' | '/'))
	{
		if (root[0] == '.')
			root.erase(0, 1);
		if (root.size() >= 1 && root[0] == '/')
			root.erase(0, 1);
	}
	_translated_path = root + '/' + _path;
}

void Request::set_translated_path(string new_path)
{
	_translated_path = new_path;
}

void Request::set_unchunked_body(string new_body)
{
	_body = new_body;
}

void Request::set_chunked_error(bool error)
{
	_chunked_error = error;
}


bool Request::is_chunked() const
{
	return _chunked;
}

bool Request::is_chunked_false() const
{
	return _chunked_error;
}

string Request::get_method() const
{
	return (_method);
}

string Request::get_URI() const
{
	return (_URI);
}

string Request::get_path() const
{
	return (_path);
}

string Request::get_translated_path() const
{
	return (_translated_path);
}

string Request::get_query_string() const
{
	return (_query_string);
}

string Request::get_protocol() const
{
	return (_protocol);
}

map Request::get_headers() const
{
	return (_headers);
}

string Request::get_body() const
{
	return (_body);
}

string Request::get_host_port() const
{
	return (_host_port);
}

string Request::get_host() const
{
	return (_host);
}

string Request::get_port() const
{
	return (_port);
}

