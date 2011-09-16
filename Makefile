NAME=fft

# C source names
CSRCS = main.c  fft_cpp.c

COBJS = $(CSRCS:%.c=%.o)
ASOBJS = $(ASRCS:%.S=%.o)

OBJS= $(COBJS) $(ASOBJS)

LIBS =

CC = g++

CFLAGS = 

all: clean note $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME).elf $(OBJS) $(LIBS)
	rm $(OBJS)
	@echo "******************** SUCCESS **********************"


dis: note $(OBJS)
	$(CC) $(CFLAGS)  -o $(NAME).elf $(OBJS) $(LIBS)
	$(OBJDUMP) -D $(NAME).elf > $(NAME).dis
	$(PREFIX)/$(TOOLPREFIX)-strip -s -R .comment $(NAME).elf
	@echo "******************** SUCCESS DIS **********************"

note:
	@echo "********************************************************"
	@echo "********************************************************"

clean:
	rm -f *.o $(OBJS) $(NAME).dis $(NAME).bin



%.o : %.c *.h
	$(CC) $(CFLAGS) -c $< -o $@

%.o : %.S #*.h
	$(CC) $(CFLAGS) -c $< -o $@


