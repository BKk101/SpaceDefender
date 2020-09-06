
CC=gcc
CFLAGS= -Wall -Werror -Wextra
LINK= -lmlx -framework OpenGL -framework AppKit -L./minilibx
NAME=a.out
SRCS=$(wildcard *.c)
LIB=$(wildcard *.a)

all : $(NAME)

$(NAME) :
	$(CC) -g $(SRCS) $(LINK) -o $@
clean :
	rm -f $(OBJS)
fclean : clean
	rm -f $(NAME)
re : fclean all

bonus :