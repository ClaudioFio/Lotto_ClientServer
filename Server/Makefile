CC = gcc
CFLAGS = -Wall
SRCDIR = src
OBJS = $(SRCDIR)/lotto_server.o $(SRCDIR)/lotto_server_util.o $(SRCDIR)/signup.o $(SRCDIR)/login.o $(SRCDIR)/invia_giocata.o $(SRCDIR)/vedi_giocate.o $(SRCDIR)/vedi_estrazione.o $(SRCDIR)/vedi_vincite.o $(SRCDIR)/esci.o $(SRCDIR)/extraction_util.o

server: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

clean:
	rm server $(SRCDIR)/*.o