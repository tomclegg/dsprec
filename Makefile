INSTALL?=install
all: dsprec
clean:
	rm dsprec
dsprec: *.c
	$(CC) -o dsprec *.c
install: dsprec
	$(INSTALL) -o 0 -g 0 -m 0755 dsprec /usr/local/bin/dsprec
