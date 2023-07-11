#include "webserv.hpp"
#include <cstring>

typedef std::string string;
typedef std::map<std::string, std::string> map;


template <typename T>
string NumberToString(T Number)
{
	std::ostringstream ss;
	ss << Number;
	return ss.str();
}

void add_status_line(string &status_line, string code)
{
	status_line += code;
	status_line += " ";
	status_line += "\n";
}

void add_header(string &headers, string header, string value)
{
	headers += header;
	headers += value;
	headers += "\n";
}

string get_current_date()
{

	struct timeval tv;
	time_t nowtime;
	struct tm *nowtm;
	char tmbuf[64];

	gettimeofday(&tv, NULL);
	nowtime = tv.tv_sec;
	nowtm = localtime(&nowtime);
	strftime(tmbuf, sizeof tmbuf, "%a, %d %b %Y %H:%M:%S GMT", nowtm);
	return string(tmbuf);
}

string get_last_modified(const char *path)
{
	struct stat stat_struct;
	time_t last_modification;
	struct tm *mtime;
	char tmbuf[64];

	if (stat(path, &stat_struct) != 0)
		std::cout << strerror(errno) << " for path " << path << std::endl;
	last_modification = stat_struct.st_mtime;
	mtime = localtime(&last_modification);
	strftime(tmbuf, sizeof tmbuf, "%a, %d %b %Y %H:%M:%S GMT", mtime);
	return string(tmbuf);
}

off_t get_file_size(const char *path)
{
	struct stat stat_struct;

	if (stat(path, &stat_struct) != 0) 
	{
		std::cerr << strerror(errno) << " for path " << path << std::endl;
		return -1;
	}
	else
		return stat_struct.st_size;
}

std::vector<string> convert_CGI_string_to_vector(string CGI_string)
{
	std::vector<string> pairs;

	char *to_split = (char *)CGI_string.c_str();
	char *ret;
	ret = strtok(to_split, "&");
	while (ret != NULL)
	{
		pairs.push_back(string(ret));
		ret = strtok(NULL, "&");
	}
	return pairs;
}

char *clean_CGI_env_token(string token)
{
	char *shiny_token;

	if ((shiny_token = (char *)malloc(sizeof(char) * (token.size() + 1))) == NULL)
	{
		std::cerr << "Failed allocating memory for a brand new shiny CGI token\n";
		return NULL;
	}
	shiny_token[token.size()] = '\0';
	strcpy(shiny_token, token.c_str());
	for (size_t i = 0; i < strlen(shiny_token); i++)
	{
		if (shiny_token[i] == '+')
			shiny_token[i] = ' ';
	}
	return shiny_token;
}

char **convert_CGI_vector_to_CGI_env(std::vector<string> CGI_vector)
{
	char **CGI_env = NULL;

	if ((CGI_env = (char **)malloc(sizeof(char *) * (CGI_vector.size() + 1))) == NULL)
	{
		std::cerr << "Failed allocating memory for CGI env\n";
		return NULL;
	}
	CGI_env[CGI_vector.size()] = NULL;
	for (size_t i = 0; i < CGI_vector.size(); i++)
		CGI_env[i] = clean_CGI_env_token(CGI_vector[i]); 
	return CGI_env;
}

void free_CGI_env(char **CGI_env)
{
	size_t i = 0;
	while (CGI_env && CGI_env[i])
	{
		free(CGI_env[i]);
		CGI_env[i] = NULL;
		i++;
	}
	if (CGI_env)
		free(CGI_env);
	CGI_env = NULL;
}

void add_body_from_path(string &response, string path, off_t file_size)
{
	char *size_itoa;
	size_itoa = (char *)NumberToString(file_size).c_str();
	char *buffer = new char[file_size + 1];
	buffer[file_size] = '\0';
	std::ifstream stream;
	stream.open(path.c_str(), std::ifstream::binary);
	stream.read(buffer, sizeof(char) * file_size);
	add_header(response, "Content-Length: ", string(size_itoa));
	response += "\n";
	response += string(buffer);
	delete[] buffer;
	stream.close();
}

void CGI_response(Request &request, string &response, Location &location)
{
	std::cout << "CGI response\n";
	string request_body = request.getBody();
	std::vector<string> CGI_vector = convert_CGI_string_to_vector(request_body);
	char **CGI_env = convert_CGI_vector_to_CGI_env(CGI_vector);

	int link[2];
	char cgi_response[8000];
	memset(cgi_response, 0, 8000);
	pipe(link);
	if (fork() == 0)
	{
		dup2(link[1], STDOUT_FILENO);
		close(link[0]);
		close(link[1]);
		char **input = (char **)malloc(sizeof(char *) * 3);

		input[0] = strdup(location.get_cgi_path().c_str());
		input[1] = strdup(request.getTranslatedPath().c_str());
		input[2] = NULL;
		execve(location.get_cgi_path().c_str(), input, CGI_env);
		free(input[0]);
		free(input[1]);
		free(input);
		free_CGI_env(CGI_env);
		exit(EXIT_FAILURE);
	}
	else
	{
		int status;
		wait(&status);
		close(link[1]);
		read(link[0], cgi_response, sizeof(cgi_response));
		string cgi_str(cgi_response);
		add_status_line(response, OK);
		char separator[5] = {13, 10, 13, 10, 0};
		size_t pos = cgi_str.find(separator) + 4;
		string body = string(cgi_str, pos);
		add_header(response, "Date: ", get_current_date());
		char *size_itoa;
		size_itoa = (char *)NumberToString(body.size()).c_str();
		add_header(response, "Content-Length: ", string(size_itoa));
		size_t begin = cgi_str.find("Content-type: ");
		size_t end = string(cgi_str, begin + 14).find('\n' | ';');
		if (end != string::npos && begin != string::npos)
			add_header(response, "Content-Type: ", string(cgi_str, begin + 14, end));
		response += '\n';
		response += body;
	}
}

bool request_is_cgi(Request &request, Location &location)
{
	string path = request.getTranslatedPath();
	if (location.is_empty())
		return false;
	string cgi_ext = location.get_cgi_extension();
	if (location.get_cgi_path() != "" && path.size() >= 4 && path.compare(path.size() - cgi_ext.size(), cgi_ext.size(), cgi_ext) == 0)
		return true;
	else
		return false;
}





bool path_is_a_directory(string path, bool slash_needed)
{
	if (slash_needed && path[path.size() - 1] != '/') 
		return false;
	DIR *dir;
	bool ret = false;
	if ((dir = opendir(path.c_str())) != NULL)
		ret = true;
	closedir(dir);
	return ret;
}

string path_where_to_upload_file(Request &request, Location &location)
{
	if (location.is_empty())
		return string();
	string new_file_path = request.getTranslatedPath() ;
	return new_file_path;
}

bool request_is_to_upload_a_file_and_valid(Request &request, Location &location)
{
	string new_file_path = path_where_to_upload_file(request, location);
	if (location.is_empty() || location.get_upload_path() == string() || path_is_a_directory(location.get_upload_path(), false) || request.getMethod() != "POST" || new_file_path == string())
		return false;
	else
		return true;
}

void chunked_response(Request &request, string &response)
{
		add_status_line(response, CREATED); 
		string body = request.getBody();
		char *size_itoa;
		size_itoa = (char *)NumberToString(body.size()).c_str();
		add_header(response, "Date: ", get_current_date());
		add_header(response, "Content-Length: ", string(size_itoa));
		response += '\n';
		response += body;
}