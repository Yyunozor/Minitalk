/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 16:54:52 by anpayot           #+#    #+#             */
/*   Updated: 2025/05/21 03:31:24 by anpayot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

#define BUF_SIZE 4096

void	x_signal(int signum)
{
	static int				bit_count = 0;
	static unsigned char	c = 0;
	static char				buf[BUF_SIZE];
	static int				idx = 0;

	c <<= 1;
	if (signum == SIGUSR2)
		c |= 1;
	bit_count++;
	if (bit_count == 8)
	{
		buf[idx++] = c;
		if (c == '\0' || idx >= BUF_SIZE - 1)
		{
			buf[idx - 1] = '\0';
			ft_putstr(buf);
			ft_putchar('\n');
			idx = 0;
		}
		bit_count = 0;
		c = 0;
	}
}

int	main(void)
{
	print_pid();
	signal(SIGUSR1, x_signal);
	signal(SIGUSR2, x_signal);
	while (1)
		pause();
	return (0);
}
