PROJ	= ../client
OBJS	= $(patsubst %.c, %.o, $(wildcard *.c))
CC	= gcc
CFLAGS = -g
LINK	= gcc
RM	= rm -rf

all: $(PROJ)

$(PROJ): $(OBJS)
	$(LINK) $^ $(LIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(PROJ) $(OBJS)
