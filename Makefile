honeypot: honeypot.c
	gcc honeypot.c -o honeypot -lpthread -Wall

clean:
	rm -rf honeypot
