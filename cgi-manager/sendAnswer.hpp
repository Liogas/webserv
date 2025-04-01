/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sendAnswer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tissad <tissad@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 17:37:56 by tissad            #+#    #+#             */
/*   Updated: 2025/03/21 18:12:01 by tissad           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef SENDANSWER_HPP
#define SENDANSWER_HPP
#include <string>

void sendResponse(int client_socket);
std::string executePHP(const char* scriptPath);

#endif