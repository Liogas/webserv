/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 14:45:03 by glions            #+#    #+#             */
/*   Updated: 2025/03/12 16:12:47 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTE_HPP
# define ROUTE_HPP

# include "utils.hpp"

typedef struct s_redirection
{
    int code;
    char path;  
} t_redirection;

class Route
{
    public:
        
    private:
        std::string _domain;
        std::string _root;
        std::string _index;
        int _bodysize;
        bool autoindex;
        std::vector<Method> _methods;
        std::map<int, std::string> _error_pages;
        bool _redir_in;
        t_redirection redir;

};

#endif