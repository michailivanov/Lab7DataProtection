OBJS_SERVER    = icrypto.o isock.o server.o
OBJS_CLIENT    = icrypto.o isock.o client.o
SOURCE_SERVER  = icrypto.c isock.c server.c
SOURCE_CLIENT  = icrypto.c isock.c client.c
HEADER         = icrypto.h isock.h
OUT_SERVER     = server
OUT_CLIENT     = client
CC             = gcc
FLAGS          = -Wall
LFLAGS         = -lcrypto

all: $(OBJS_SERVER) $(OBJS_CLIENT)
	$(CC) -g $(OBJS_SERVER) -o $(OUT_SERVER) $(LFLAGS)
	$(CC) -g $(OBJS_CLIENT) -o $(OUT_CLIENT) $(LFLAGS)

$(OBJS_SERVER): $(SOURCE_SERVER) $(LFLAGS)
$(OBJS_CLIENT): $(SOURCE_CLIENT) $(LFLAGS)

clean:
	rm -f $(OBJS_SERVER) $(OBJS_CLIENT) $(OUT_SERVER) $(OUT_CLIENT)
