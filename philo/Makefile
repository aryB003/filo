NAME	= philo

CC		= cc
CFLAGS	= -Wall -Wextra -Werror -pthread

SRCS	= main.c parsing.c utils.c init.c log.c routine.c monitor.c
OBJS	= $(SRCS:.c=.o)
HEADER	= philo.h

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
