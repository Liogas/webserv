/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 14:45:03 by glions            #+#    #+#             */
/*   Updated: 2025/03/18 15:32:57 by glions           ###   ########.fr       */
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
	std::string path;
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
		void setRoot(std::vector<std::string> args);
		void setIndex(std::string index);
		void setIndex(std::vector<std::string> args);
		void setBodySize(int size);
		void setAutoIndex(bool value);
		void setAutoIndex(std::vector<std::string> args);
		void addMethod(Method m);
		void addMethods(std::vector<std::string> args);
		void addErrorPage(int key, std::string path);
		void setRedir(t_redirection redir);
		void setRedir(std::vector<std::string> args);
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
	class ErrorToManyArgs : public std::exception
	{
		public:
			virtual const char *what() const throw();
	};
	class ErrorNotEnoughArgs : public std::exception
	{
		public:
			virtual const char *what() const throw();
	};
	class ErrorNotValidArgs : public std::exception
	{
		public:
			virtual const char *what() const throw();
	};
	class ErrorDuplicate : public std::exception
    {
        public:
            virtual const char *what() const throw();
    };
};

#endif