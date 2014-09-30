CFLAGS = `pkg-config --cflags libcurl libxml-2.0` -g -Wall --std=c99
LDLIBS = `pkg-config --libs libcurl libxml-2.0`
CC = gcc
OBJ = soapreq.o callcurl.o getdevices.o parse.o

all: ancle
	
ancle: $(OBJ)
	$(CC) $(CFLAGS) $@.c $^ -o $@ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LDLIBS)

soaptest: soapreq.o callcurl.o parse.o
	$(CC) $(CFLAGS) $@.c $^ -o $@ $(LDLIBS)

soapreqtest: soapreq.o 
	$(CC) $(CFLAGS) $@.c $^ -o $@ $(LDLIBS)

config:
	$(CC) -g -Wall $@.c -o $@

clean:
	-rm config soapreqtest soaptest ancle *.o

