CFLAGS = `pkg-config --cflags libcurl libxml-2.0` -g -Wall --std=c99
LDLIBS = `pkg-config --libs libcurl libxml-2.0`
CC = gcc
OBJ = soapenv.o callcurl.o getdevices.o 

all: ancle
	
ancle: $(OBJ)
	$(CC) $(CFLAGS) $@.c $^ -o $@ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LDLIBS)

soaptest: soapenv.o callcurl.o
	$(CC) $(CFLAGS) $@.c $^ -o $@ $(LDLIBS)

clean:
	-rm soaptest ancle *.o

