CC     = gcc
CFLAGS = -Wall -Wmissing-prototypes -Wstrict-prototypes \
		 -O2 -fomit-frame-pointer -std=c99 #-lpthread
SOURCE = brutehttp.c

brutehttp: $(OBJECT)
	$(CC) $(CFLAGS) -o $@ $(OBJECT) $(SOURCE)

.PHONY: clean

clean:
	rm brutehttp
