/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minitalk_comments.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 03:00:00 by anpayot           #+#    #+#             */
/*   Updated: 2025/05/21 03:27:58 by anpayot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINITALK_COMMENTS_H
# define MINITALK_COMMENTS_H

/**
 * @brief Sends a bit (0 or 1) to the server via a signal (SIGUSR1 or SIGUSR2).
 * @param pid The server's PID.
 * @param bit 0 for SIGUSR1, 1 for SIGUSR2.
 */
void	s_bit(int pid, int bit);

/**
 * @brief Sends a character (8 bits) to the server, bit by bit.
 * @param pid The server's PID.
 * @param c Character to send.
 */
void	s_char(int pid, char c);

/**
 * @brief Converts a string to an integer (equivalent to atoi).
 * @param str String to convert.
 * @return The corresponding integer value.
 */
int		ft_atoi(const char *str);

/**
 * @brief Writes a single character to standard output.
 * @param c Character to write.
 */
void	ft_putchar(char c);

/**
 * @brief Writes a string to standard output.
 * @param str String to write.
 */
void	ft_putstr(char *str);

/**
 * @brief Writes an integer to standard output.
 * @param n Integer to write.
 */
void	ft_putnbr(int n);

/**
 * @brief Displays the server's PID.
 * Gets and prints the server's process ID to standard output.
 */
void	print_pid(void);

/**
 * @brief Signal handler for the server.
 * 
 * Reconstructs characters received bit by bit, stores them in a buffer,
 * and displays the complete message when the null terminator is received.
 * @param signum Signal received (SIGUSR1 or SIGUSR2).
 */
void	x_signal(int signum);

#endif
