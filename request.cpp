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
    _body = parseBody(request_array);
    std::string request_string = std::string(request_array);
    std::string start_line = std::string(request_string, 0, request_string.find('\n'));
    parseStartLine(request_string);
    _translated_path = std::string();
    _headers = parseHeaders(request_string);

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
    o << "Method: " << i.getMethod() << std::endl
      << "URI: " << i.getURI() << std::endl
      << "Path: " << i.getPath() << std::endl
      << "Query string: " << i.getQueryString() << std::endl
      << "Protocol: " << i.getProtocol() << std::endl
      << "Host: " << i.getHost() << std::endl
      << "Port: " << i.getPort() << std::endl
      << "Headers:" << std::endl;
    displayMap(i.getHeaders());
    o << std::endl
      << "Body:" << std::endl
      << i.getBody() << std::endl;
    o << "end" << std::endl;

    return o;
}

int getHeaderValue(std::map<std::string, std::string>& headers, std::string request, std::string header) {
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

void initHeaders(std::map<std::string, std::string>& headers) {
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

std::map<std::string, std::string> Request::parseHeaders(const std::string request_str) {
    std::map<std::string, std::string> headers;

    initHeaders(headers);
    getHeaderValue(headers, request_str, "Accept-Charset");
    getHeaderValue(headers, request_str, "Accept-Language");
    getHeaderValue(headers, request_str, "Authorization");
    getHeaderValue(headers, request_str, "Content-Length");
    getHeaderValue(headers, request_str, "Content-Type");
    getHeaderValue(headers, request_str, "Date");
    getHeaderValue(headers, request_str, "Host");
    getHeaderValue(headers, request_str, "Referer");
    getHeaderValue(headers, request_str, "Transfer-Encoding");
    getHeaderValue(headers, request_str, "User-Agent");

    return headers;
}

string Request::parseBody(const char* request) {
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

   

int Request::parseStartLine(string startLine)
{
	string::iterator it_begin = startLine.begin();
	string::iterator it_end = it_begin + startLine.find(" ");
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
		while (startLine[pos + len] && startLine[pos + len] != ' ')
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


bool Request::isBadRequest() const
{
	if (_chunked_error == true)
		return true;
	if (getHeaders()["Host"] == string())
		return true;
	else
		return false;
}

void Request::appendRootToPath(string root)
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

void Request::setTranslatedPath(string newPath)
{
	_translated_path = newPath;
}

void Request::setUnchunkedBody(string newBody)
{
	_body = newBody;
}

void Request::setChunkedError(bool error)
{
	_chunked_error = error;
}


bool Request::isChunked() const
{
	return _chunked;
}

bool Request::isChunkedFalse() const
{
	return _chunked_error;
}

string Request::getMethod() const
{
	return (_method);
}

string Request::getURI() const
{
	return (_URI);
}

string Request::getPath() const
{
	return (_path);
}

string Request::getTranslatedPath() const
{
	return (_translated_path);
}

string Request::getQueryString() const
{
	return (_query_string);
}

string Request::getProtocol() const
{
	return (_protocol);
}

map Request::getHeaders() const
{
	return (_headers);
}

string Request::getBody() const
{
	return (_body);
}

string Request::getHostPort() const
{
	return (_host_port);
}

string Request::getHost() const
{
	return (_host);
}

string Request::getPort() const
{
	return (_port);
}
