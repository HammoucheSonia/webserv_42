#include "webserv.hpp"
#include "location.hpp"
#include <iostream>
#include <sstream>

Location::Location() {
    this->_id = 0;
}

Location::Location(const std::vector<unsigned char>& location_config, int id) 
{
    size_t start_block = 0;
    for (size_t i = 0; i < location_config.size(); i++) 
    {
        if (location_config[i] == '{') 
        {
            start_block = i;
            break;
        }
    }

    size_t end_block = 0;
    for (size_t i = 0; i < location_config.size(); i++) 
    {
        if (location_config[i] == '}') 
        {
            end_block = i;
            break;
        }
    }
    if (end_block != location_config.size() - 1) {
        throw std::string("Un bloc interdit est présent à l'intérieur du bloc location.");
    }
    
    this->_id = id;
    this->_path = std::string(location_config.begin(), location_config.begin() + start_block);
    set_autoindex(location_config);
    set_redirection(location_config);
    set_cgi_path(location_config);
    set_root(location_config);
    set_indexes(location_config);
    set_methods(location_config);
}

Location::~Location() {}

int Location::get_id() const {
    return (this->_id);
}

std::string Location::get_path() const {
    return (this->_path);
}

bool Location::method_is_allowed(const std::string& method) const {
    if (this->is_empty()) {
        return true;
    }
    if (this->_allowed_methods.find(method) == this->_allowed_methods.end()) {
        return false;
    } else {
        return this->_allowed_methods.at(method);
    }
}

std::string Location::get_root() const {
    return (this->_root);
}

std::vector<std::string> Location::get_index() const {
    return (this->_index);
}

bool Location::auto_index_is_on() const {
    return (this->_auto_index);
}

std::string Location::get_cgi_path() const {
    return (this->_cgi_path);
}

std::string Location::get_cgi_extension() const {
    return (this->_cgi_extension);
}

std::string Location::get_upload_path() const {
    return (this->_upload_path);
}

std::pair<std::string, std::string> Location::get_redirect() const {
    return (this->_redirect);
}

bool Location::is_empty() const {
    if (this->_id == -1) {
        return true;
    }
    return false;
}

bool Location::is_cgi() const {
    if (this->_cgi_path == "") {
        return false;
    }
    return true;
}

bool Location::root_is_set() const {
    return this->_root_set;
}

std::string Location::get_configuration(const std::vector<unsigned char>& server_config, const std::string& label, bool optional) {
    std::string server_config_str(server_config.begin(), server_config.end());
    size_t label_pos = server_config_str.find(label);
    
    if (label_pos == std::string::npos && optional == false) {
        throw std::string("Le label " + label + " est manquant.");
    } else if (label_pos == std::string::npos && optional == true) {
        return ("");
    }
    
    size_t end_line_pos = server_config_str.find(";", label_pos);
    if (end_line_pos == std::string::npos) {
        throw std::string("Il manque un point-virgule à la fin de la ligne.");
    }
    
    std::string line = server_config_str.substr(label_pos, end_line_pos - label_pos);
    if (server_config_str.find(label, end_line_pos + 1) != std::string::npos) {
        throw std::string("Double " + label + ".");
    }
    
    return (line);
}

void Location::set_root(const std::vector<unsigned char>& location_config) {
    std::string location_config_str(location_config.begin(), location_config.end());
    std::string root_line = get_configuration(location_config, "root", true);
    if (root_line != "") {
        _root_set = true;
        size_t split_pos = root_line.find(' ');
        if (split_pos == std::string::npos) {
            throw std::string("Instruction invalide pour 'root' dans la configuration de la location.");
        }
        
        this->_root = root_line.substr(split_pos + 1);
        if (this->_root == "") {
            throw std::string("Instruction invalide pour 'root' dans la configuration de la location.");
        }
    } else {
        _root_set = false;
    }
}

void Location::set_indexes(const std::vector<unsigned char>& location_config) {
    std::string location_config_str(location_config.begin(), location_config.end());
    std::string index_line = get_configuration(location_config, " index ", true);
    if (index_line != "") {
        size_t split_pos = 0;
        while ((split_pos = index_line.find(' ', split_pos + 1)) != std::string::npos) {
            if (index_line.find(' ', split_pos + 1) == std::string::npos) {
                this->_index.push_back(index_line.substr(split_pos + 1));
            } else {
                this->_index.push_back(index_line.substr(split_pos + 1, index_line.find(' ', split_pos + 1) - split_pos - 1));
            }
        }
        if (this->_index.empty() == true || this->_index.at(0) == "") {
            throw std::string("Instruction invalide pour 'index' dans la configuration de la location.");
        }
    }
}

void Location::set_methods(const std::vector<unsigned char>& location_config) {
    std::string location_config_str(location_config.begin(), location_config.end());
    std::string methods_line = get_configuration(location_config, "methods", true);
    this->_allowed_methods["GET"] = false;
    this->_allowed_methods["POST"] = false;
    this->_allowed_methods["DELETE"] = false;
    
    if (methods_line != "") {
        size_t split_pos = 0;
        while ((split_pos = methods_line.find(' ', split_pos + 1)) != std::string::npos) {
            std::string method;
            if (methods_line.find(' ', split_pos + 1) == std::string::npos) {
                method = methods_line.substr(split_pos + 1);
            } else {
                method = methods_line.substr(split_pos + 1, methods_line.find(' ', split_pos + 1) - split_pos - 1);
            }
            if (method == "GET") {
                this->_allowed_methods["GET"] =true;
            } else if (method == "POST") {
                this->_allowed_methods["POST"] = true;
            } else if (method == "DELETE") {
                this->_allowed_methods["DELETE"] = true;
            }
        }
    } else {
        this->_allowed_methods["GET"] = true;
        this->_allowed_methods["POST"] = true;
        this->_allowed_methods["DELETE"] = true;
    }
}

void Location::set_autoindex(const std::vector<unsigned char>& location_config) {
    std::string location_config_str(location_config.begin(), location_config.end());
    std::string auto_index_line = get_configuration(location_config, "autoindex", true);
    if (auto_index_line != "") {
        size_t split_pos = auto_index_line.find(' ');
        if (split_pos == std::string::npos) {
            throw std::string("Instruction invalide pour 'autoindex' dans la configuration de la location.");
        }
        std::string boolean = auto_index_line.substr(split_pos + 1);
        if (boolean == "on") {
            this->_auto_index = true;
        } else {
            this->_auto_index = false;
        }
    } else {
        this->_auto_index = false;
    }
}

void Location::set_redirection(const std::vector<unsigned char>& location_config) {
    std::string location_config_str(location_config.begin(), location_config.end());
    std::string redirect_line = get_configuration(location_config, "redirect", true);
    if (redirect_line != "") {
        size_t split_pos = redirect_line.find(' ');
        if (split_pos == std::string::npos) {
            throw std::string("Instruction invalide pour 'redirect' dans la configuration de la location.");
        }
        size_t second_split_pos = redirect_line.find(' ', split_pos + 1);
        if (second_split_pos == std::string::npos) {
            throw std::string("Instruction invalide pour 'redirect' dans la configuration de la location.");
        }
        this->_redirect.first = redirect_line.substr(split_pos + 1, second_split_pos - split_pos - 1);
        this->_redirect.second = redirect_line.substr(second_split_pos + 1);
        if (this->_redirect.first == "" || this->_redirect.second == "") {
            throw std::string("Instruction invalide pour 'redirect' dans la configuration de la location.");
        }
    }
}

void Location::set_cgi_path(const std::vector<unsigned char>& location_config) {
    std::string location_config_str(location_config.begin(), location_config.end());
    std::string cgi_line = get_configuration(location_config, "cgi_path", true);
    if (cgi_line != "") {
        size_t split_pos = cgi_line.find(' ');
        if (split_pos == std::string::npos) {
            throw std::string("Instruction invalide pour 'cgi_path' dans la configuration de la location.");
        }
        this->_cgi_path = cgi_line.substr(split_pos + 1);
        if (this->_cgi_path == "") {
            throw std::string("Instruction invalide pour 'cgi_path' dans la configuration de la location.");
        }
    } else {
        this->_cgi_path = "";
    }
    
    cgi_line = get_configuration(location_config, "cgi_extension", true);
    if (cgi_line != "") {
        size_t split_pos = cgi_line.find(' ');
        if (split_pos == std::string::npos) {
            throw std::string("Instruction invalide pour 'cgi_extension' dans la configuration de la location.");
        }
        this->_cgi_extension = cgi_line.substr(split_pos + 1);
        if (this->_cgi_extension == "") {
            throw std::string("Instruction invalide pour 'cgi_extension' dans la configuration de la location.");
        }
    } else {
        this->_cgi_extension = "";
    }
    
    cgi_line = get_configuration(location_config, " upload ", true);
    if (cgi_line != "") {
        size_t split_pos = cgi_line.find(' ');
        if (split_pos == std::string::npos) {
            throw std::string("Instruction invalide pour 'upload' dans la configuration de la location.");
        }
        this->_upload_path = cgi_line.substr(split_pos + 1);
        if (this->_upload_path == "") {
            throw std::string("Instruction invalide pour 'upload' dans la configuration de la location.");
        }
    } else {
        this->_upload_path = "";
    }
}
