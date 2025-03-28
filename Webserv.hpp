/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 09:24:10 by glions            #+#    #+#             */
/*   Updated: 2025/03/25 11:17:19 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "ParseConfig.hpp"
# include "Server.hpp"
# include "Request.hpp"


class Webserv
{
    public:
        Webserv(void);
        ~Webserv();
        // Webserv &operator=(Webserv const &copy);
        void printServers(void);
        bool parsing(std::string file_path);
        bool initServers(void);
        bool ready(void);
        bool start(void);
        bool handleClient(int clientFd);
        Server *whereIsClient(int fdClient);
        std::vector<Server *> getServers() const;
    private:
        std::vector<Server *> _servers;
        int _epollFd;
};

#endif