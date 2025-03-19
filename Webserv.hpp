/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 09:24:10 by glions            #+#    #+#             */
/*   Updated: 2025/03/19 15:33:37 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "ParseConfig.hpp"
# include "Server.hpp"

class Webserv
{
    public:
        Webserv(void);
        Webserv(Webserv const &copy);
        ~Webserv();
        Webserv &operator=(Webserv const &copy);
        void printServers(void);
        bool parsing(std::string file_path);
        bool bindServers(void);
        bool ready(void);
        bool start(void);
        Server *whereIsClient(int fdClient);
        std::vector<Server *> getServers() const;
        int getEpollfd(void) const;
    private:
        std::vector<Server *> _servers;
        int _epollFd;
};

#endif