/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 16:55:36 by anpayot           #+#    #+#             */
/*   Updated: 2025/06/07 22:40:50 by anpayot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

void	s_char(int pid, char c)
{
	int	i;
	int	bit;

	i = 7;
	while (i >= 0)
	{
		bit = (c >> i) & 1;
		if (bit == 0)
			kill(pid, SIGUSR1);
		else
			kill(pid, SIGUSR2);
		usleep(BIT_DELAY);
		i--;
	}
}

int	main(int argc, char **argv)
{
	int		pid;
	char	*str;
	int		i;

	if (argc != 3)
	{
		ft_putstr("Usage: ./client <server_pid> <message>\n");
		return (1);
	}
	pid = ft_atoi(argv[1]);
	str = argv[2];
	i = 0;
	while (str[i])
	{
		s_char(pid, str[i]);
		i++;
	}
	s_char(pid, '\0');
	return (0);
}
