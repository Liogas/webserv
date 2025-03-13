/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 15:29:40 by glions            #+#    #+#             */
/*   Updated: 2025/03/13 11:04:45 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include "utils.hpp"
# include "Route.hpp"

class ServerConfig
{
    public:
        ServerConfig(void);
        ServerConfig(ServerConfig const &copy);
        ~ServerConfig();
        ServerConfig &operator=(const ServerConfig &copy);
        // SETTERS
        void setServerName(std::string name);
        void setPort(int port);
        void addRoute(Route *route);
        // GETTERS
        int getPort(void) const;
        std::string getServerName(void) const;
        std::map<std::string, Route *> getRoutes(void) const;
    private:
        std::string _serverName;
        int _port;
        std::map<std::string, Route *> _routes;
};

#endif