/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minitalk.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 16:29:07 by anpayot           #+#    #+#             */
/*   Updated: 2025/05/17 16:56:51 by anpayot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINITALK_H
# define MINITALK_H

# include <signal.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>

# define BIT_DELAY 100

// Server Functions Prototype
void	print_pid(void);
void	init_signal(void);
void	x_signal(int signum);

// Client Fucntions Prototype (s for send)
void	s_bit(int pid, int bit);
void	s_char(int pid, char c);
void	s_string(int pid, char *str);

#endif