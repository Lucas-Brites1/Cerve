TARGET = app

SRCDIR = .
SERVERDIR = server
HTTPDIR = server/http
JSONDIR = server/json
UTILSDIR = server/utils

CC = gcc
CFLAGS = -Wall -Wextra -I. -std=c11

SRC = main.c \
	$(SERVERDIR)/server.c \
	$(SERVERDIR)/router.c \
	$(SERVERDIR)/middleware.c \
	$(SERVERDIR)/server_utils.c \
	$(UTILSDIR)/utils.c \
	$(JSONDIR)/json.c 

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

.PHONY: all clean