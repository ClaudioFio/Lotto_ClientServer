CC = gcc
CFLAGS = -Wall
SRCDIR = src
OBJS = $(SRCDIR)/lotto_client.o $(SRCDIR)/lotto_client_util.o $(SRCDIR)/requestSignup.o $(SRCDIR)/requestLogin.o $(SRCDIR)/requestInvia_giocata.o $(SRCDIR)/requestVedi_giocate.o $(SRCDIR)/requestVedi_estrazione.o $(SRCDIR)/requestVedi_vincite.o $(SRCDIR)/requestEsci.o

client: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

clean:
	rm client $(SRCDIR)/*.o
