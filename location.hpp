#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <utility>
#include <ostream>
#include <map>
#include <vector>

typedef std::string string;
typedef std::pair<string, string> pair;
typedef std::vector<string> vec_string;
typedef std::map<std::string, bool> map_str_bool;

class Location
{
	private:
		int				_id;
		string			_path;
		string			_root;
		vec_string		_index;
		map_str_bool	_allowed_methods;
		bool			_auto_index;
		pair			_redirect;
		string			_cgi_path;
		bool			_root_set;
		string			_cgi_extension;
		string			_upload_path;

		void	set_root(const std::vector<unsigned char>& location_config);
		void	set_indexes(const std::vector<unsigned char>& location_config);
		void	set_methods(const std::vector<unsigned char>& location_config);
		void	set_autoindex(const std::vector<unsigned char>& location_config);
		void	set_redirection(const std::vector<unsigned char>& location_config);
		void	set_cgi_path(const std::vector<unsigned char>& location_config);

	public:
		Location();
		Location(const std::vector<unsigned char>& location_config, int id);
		~Location();

		int			get_id() const;
		bool		method_is_allowed(const std::string& method) const;
		std::string	get_path() const;
		std::string	get_root() const;
		vec_string	get_index() const;
		bool		auto_index_is_on() const;
		pair		get_redirect() const;
		std::string	get_cgi_path() const;
		std::string	get_cgi_extension() const;
		std::string	get_upload_path() const;
		bool		is_empty() const;
		bool		is_cgi() const;
		bool		root_is_set() const;

		std::string	get_configuration(const std::vector<unsigned char>& server_config, const std::string& label, bool optional);
};

std::ostream &	operator<<(std::ostream & o, Location const & i);

#endif
