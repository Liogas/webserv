/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 15:29:40 by glions            #+#    #+#             */
/*   Updated: 2025/03/10 15:32:39 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include "utils.hpp"

class ServerConfig
{
    public:
        ServerConfig(void);
        ~ServerConfig();
        void addServerName(std::string name);
        void addPort(int port);
    private:
        std::vector<std::string> server_name;
        std::vector<int> ports;
};

#endif