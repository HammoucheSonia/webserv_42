# include <iostream>
# include <string>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <stdlib.h>
# include <sstream>
# include <stdio.h>
# include <map>
# include <arpa/inet.h>
# include <limits.h>
# include <sys/time.h>
# include <time.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <errno.h>
# include <fstream>
# include <vector>
# include <poll.h>
# include <dirent.h>
# include <signal.h>
# include <sys/wait.h>
#include "request.hpp"
#include "location.hpp"


#define PORT 8080
# define PENDING_MAX 512
# define BUFFER_SIZE 1024

// STATUS CODES
# define OK					"200"
# define CREATED			"201"
# define NO_CONTENT			"204"
# define TEMPORARY_REDIRECT "307"
# define BAD_REQUEST		"400"
# define FORBIDDEN			"403"
# define NOT_FOUND			"404"
# define NOT_ALLOWED		"405"
# define TOO_LARGE			"413"
# define NOT_IMPLEMENTED	"501"