/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tissad <tissad@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 17:58:03 by tissad            #+#    #+#             */
/*   Updated: 2025/04/01 11:18:13 by tissad           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include "sendAnswer.hpp"

std::string read_cgi_res(int fd)
{
	std::string res;
	char buffer[1024];
	ssize_t bytesRead;

	while ((bytesRead = read(fd, buffer, sizeof(buffer) - 1)) > 0)
	{
		buffer[bytesRead] = '\0';
		res += buffer;
	}
	return res;
}

std::string cgi(char* const *args, char* const *envp)
{
	int pipefd[2];  // 0: lecture, 1: écriture
	if (pipe(pipefd) == -1)
		return ("Error: Cannot create pipe");
	pid_t pid = fork();
	if (pid == -1)
		return "Error: Cannot fork process";
	if (pid == 0) {
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		execve(args[0], args, envp);
		return ("Error: Cannot execute CGI script");
	}
	else
	{
		close(pipefd[1]);
		int status;
		std::string res = read_cgi_res(pipefd[0]);
		waitpid(pid, &status, 0);
		close(pipefd[0]);
		return res;
	}
}