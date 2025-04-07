/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseRequest.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glions <glions@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 10:43:02 by glions            #+#    #+#             */
/*   Updated: 2025/04/07 14:18:19 by glions           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSEREQUEST_HPP
# define PARSEREQUEST_HPP

# include "Server.hpp"
# include "Request.hpp"
# include "utils.hpp"

class Server;
class Client;
class Route;

class ParseRequest
{
    public:
        ParseRequest(std::string buffer, ssize_t bytes, Server *serv, Client *client);
        ~ParseRequest();
        int                         parseHost(std::string host);
        int                         parseRequest(std::string request);
        int                         parseHeader(void);
        void                        addBuffer(std::string buffer, ssize_t bytes);
        Route                      *findLocation(void);
        std::string                 getBuffer(void) const;
        int                         getContentLength(void) const;
        int                         getBytes(void) const;
        void                        setBytes(int value);
        void                        setContentLength(int value);
        void                        setHeader(std::string header);
        void                        setBody(std::string body);
        t_header                    getHeaderInfo(void) const;

        std::string                 getBody(void) const;
        Route                       *getRoute(void) const;
        Server                      *getServer(void) const;
        Client                      *getClient(void) const;
        std::string                 getSaveRequest(void) const;

    private:
        t_header    _headerInfo;
        std::string _header;
        std::string _body;
        std::string _buffer;
        int         _bytesRead;
        std::string _copyRequest;
        std::string _saveRequest;
        Route       *_route;
        Server      *_server;
        Client      *_client;
};

#endif