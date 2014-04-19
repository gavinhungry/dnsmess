build: clean dnsmess

dnsmess: dnsmess.c dnsmess.h
	gcc dnsmess.c -o dnsmess

clean:
	@rm -f dnsmess
