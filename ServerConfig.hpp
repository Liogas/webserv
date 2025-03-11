/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 15:29:40 by glions            #+#    #+#             */
/*   Updated: 2025/03/11 15:33:57 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include "utils.hpp"

class ServerConfig
{
    public:
        ServerConfig(void);
        ServerConfig(ServerConfig const &copy);
        ~ServerConfig();
        void addServerName(std::string name);
        void addPort(int port);
        std::vector<std::string> getServerNames(void) const;
        std::vector<int> getPorts(void) const;
        ServerConfig &operator=(const ServerConfig &copy);
    private:
        std::vector<std::string> _server_names;
        std::vector<int> _ports;
};

#endif