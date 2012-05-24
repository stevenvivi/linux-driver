target := $1

all: 
	cc -o $(target) $(patsubst %, %.c, $(target))
clean:
	rm a.out $(target)

.PHONY: clean
