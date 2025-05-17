# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: anpayot <anpayot@student.42lausanne.ch>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/17 16:38:02 by anpayot           #+#    #+#              #
#    Updated: 2025/05/17 16:54:44 by anpayot          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SERVER = server
CLIENT = client

# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror
INCLUDES = -I.

SRC_DIR = srcs/

# Source Files
SRC_SERVER = $(SRC_DIR)server.c
SRC_CLIENT = $(SRC_DIR)client.c

# Rules
all: $(SERVER) $(CLIENT)

$(SERVER): $(SRC_SERVER)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

$(CLIENT): $(SRC_CLIENT)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

clean:
	rm -f *.o

fclean: clean
	rm -f $(SERVER) $(CLIENT)

re: fclean all

.PHONY: all clean fclean re