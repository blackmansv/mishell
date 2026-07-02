PROG=	shell
DIR =	src
SRCS=	$(DIR)/shell.c 
CC	=	clang
DEPS=	$(DIR)/fshell.h $(DIR)/headers.h

shell: $(SRCS) $(DEPS)
	@echo 'Compilando: $@'
	@echo 'Invocando: compilador CC de C'
	$(CC) $(SRCS) -o $(PROG)
	@echo 'Finalizando la construccion del objetivo: $@'
	@echo ' '
	
.PHONY: clean

clean:
	rm shell