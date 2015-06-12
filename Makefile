CC=gcc

CFLAGS=-O2 -o

LDFLAGS=-lpthread -lfcgi

OUT=test.cgi

DEBUG=-g -o

src=	cgi_task.c \
	CThread.c \
	main.c

all:
	$(CC) $(CFLAGS) $(OUT) cgi_task.c CThread.c main.c $(LDFLAGS)
debug:
	$(CC) $(DEBUG) $(OUT) cgi_task.c CThread.c main.c $(LDFLAGS)
clean:
	rm $(OUT)