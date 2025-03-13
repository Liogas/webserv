/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 14:45:03 by glions            #+#    #+#             */
/*   Updated: 2025/03/13 10:28:01 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTE_HPP
#define ROUTE_HPP

#include "utils.hpp"
#include <map>

typedef struct s_redirection
{
	bool exist;
	int code;
	char path;
} t_redirection;

class Route
{
	public:
		Route(void);
		Route(std::string path);
		Route(const Route &copy);
		~Route();
		Route &operator=(const Route &copy);
		// SETTERS
		void setPath(std::string path);
		void setRoot(std::string root);
		void setIndex(std::string index);
		void setBodySize(int size);
		void setAutoIndex(bool value);
		void addMethod(Method m);
		void addErrorPage(int key, std::string path);
		void setRedir(t_redirection redir);
		// GETTERS
		std::string getPath(void) const;
		std::string getRoot(void) const;
		std::string getIndex(void) const;
		int getBodySize(void) const;
		bool getAutoIndex(void) const;
		std::vector<Method> getMethods(void) const;
		std::map<int, std::string> getErrorPages(void) const;
		t_redirection getRedir(void) const;

	private:
		std::string _path;
		std::string _root;
		std::string _index;
		int _bodysize;
		bool _autoindex;
		std::vector<Method> _methods;
		std::map<int, std::string> _error_pages;
		t_redirection _redir;
};

#endif