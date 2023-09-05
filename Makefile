SOURCES=cope.c
EXECUTABLE=cope
PREFIX=/usr/local
CFLAGS += -O2

all:
	$(CC) $(SOURCES) $(CFLAGS) -D_DEFAULT_SOURCE -DINSTALL_PREFIX=\"$(PREFIX)\" -l archive -l curl -o $(EXECUTABLE)

install: all
	mkdir -p /root/.cache/pk
	cp $(EXECUTABLE) $(DESTDIR)$(PREFIX)/bin

