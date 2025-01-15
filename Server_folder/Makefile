PROJ	= ../server
OBJS	= $(patsubst %.c, %.o, $(wildcard *.c))
CC	= gcc
CFLAGS  = -g
LINK	= gcc
RM	= rm -rf
LIBS	= -lpthread

all: $(PROJ)

$(PROJ): $(OBJS)
	$(LINK) $^ $(LIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(PROJ) $(OBJS)
	
