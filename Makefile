NAME=	pingpong
SRC=	main.c	\
	gest_contexte.c
OBJ=	$(SRC:.c=.o)
CC=	gcc
CFLAGS=	-W -Wall -g -m32
RM=	@rm -fv


$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

clean:
	$(RM) $(OBJ)

fclean:
	$(RM) $(NAME)

re:	clean fclean $(NAME)
