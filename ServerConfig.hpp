/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 15:29:40 by glions            #+#    #+#             */
/*   Updated: 2025/03/12 14:33:09 by glions           ###   ########.fr       */
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
        void setServerName(std::string name);
        void setPort(int port);
        std::string getServerName(void) const;
        int getPort(void) const;
        ServerConfig &operator=(const ServerConfig &copy);
    private:
        std::string _serverName;
        int _port;
        std::vector<Route> _routes;
};

#endif