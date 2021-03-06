INC=-Iinclude
CFLAGS=$(INC) -w

PARSERDIR=input_parse

SRCS=utils.c mshell.c builtins.c ioLib.c redirections.c parserCommons.c children.c
OBJS:=$(SRCS:.c=.o)

all: mshell

mshell: $(OBJS) siparse.a
	cc $(CFLAGS) $(OBJS) siparse.a -o $@

%.o: %.c
	cc $(CFLAGS) -c $<

siparse.a:
	$(MAKE) -C $(PARSERDIR)

clean:
	rm -f mshell *.o
