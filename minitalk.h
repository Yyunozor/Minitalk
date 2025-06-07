/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minitalk.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 16:29:07 by anpayot           #+#    #+#             */
/*   Updated: 2025/06/07 22:40:58 by anpayot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINITALK_H
# define MINITALK_H

# include <signal.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include "minitalk_comments.h"

# define BIT_DELAY 100

// Utils Functions

void	ft_putchar(char c);
void	ft_putstr(char *str);
void	ft_putnbr(int n);
void	print_pid(void);
int		ft_atoi(const char *str);

// Server Functions

void	x_signal(int signum);

// Client Functions (s for send)

void	s_char(int pid, char c);

#endif