/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tissad <tissad@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 17:58:03 by tissad            #+#    #+#             */
/*   Updated: 2025/04/01 11:02:28 by tissad           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include "sendAnswer.hpp"

std::string executePHP(const char* scriptPath) {


 else {  // Processus parent (lit le résultat du PHP)
		close(pipefd[1]); // Ferme le côté écriture

		char buffer[1024];
		std::string output;
		ssize_t bytesRead;

		// Lire la sortie de PHP dans le pipe
		while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
			buffer[bytesRead] = '\0';  // Ajoute un terminateur
			output += buffer;
			std :: cout << "output : " << output << "size " << bytesRead << std :: endl;
		}
		close(pipefd[0]); // Fermer le pipe après lecture
		
		// Attendre la fin du processus enfant
		int status;
		waitpid(pid, &status, 0);

		return output;
	}
}


std::string cgi(char* const *argv, char* const *envp)
{
	int pipefd[2];  // 0: lecture, 1: écriture
	if (pipe(pipefd) == -1) {

		return "Error: Cannot create pipe";
	}
	pid_t pid = fork();
	if (pid == -1) {

		return "Error: Cannot fork process";
	}
	if (pid == 0) {
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		
	
		
		
		std::string cmd = "/usr/bin/php-cgi";
		
		std::cout << "fils cmd : " << cmd << std::endl;
		// Exécuter le script PHP avec execve
		char* const args[] = {(char* const)(cmd.c_str()), (char* const)scriptPath, NULL};
		execve(args[0], args, NULL);

		// Si execve échoue
		exit(1);
	}
}