TARGET=clientTCP serveurTCP 
CC=gcc
CFLAGS= -Wall -Wextra -g
LDFLAGS = -lncurses
normal: $(TARGET)
clientTCP: clientTCP.c
	$(CC) $(CFLAGS) clientTCP.c client_template.c -o clientTCP -lncurses
serveurTCP: serveurTCP.c
	$(CC) $(CFLAGS) serveurTCP.c -o serveurTCP
clean:
	$(RM) $(TARGET)