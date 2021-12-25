CC     = gcc
LDFLAGS = `pkg-config --libs libuv`
CFLAGS = -Wall -Wmissing-prototypes -Wstrict-prototypes \
		 -O2 -fomit-frame-pointer -std=gnu99 #-lpthread
SOURCE = brutehttp.c


.PHONY: brutehttp clean

brutehttp: $(OBJECT)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECT) $(SOURCE)

clean:
	rm brutehttp
