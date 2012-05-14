OBJS := app.o print.o
SRCS := $(OBJS:.=.c)

Tar: .depend $(OBJS)
	gcc -o $@ $(OBJS)

clean:
	rm -f $(OBJS)

include rules.mk
sinclude .depend

