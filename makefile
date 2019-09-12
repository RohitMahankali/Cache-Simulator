cachesim: cachesim.c
	gcc -g -Wall -Werror -fsanitize=address -o cachesim cachesim.c cachesim.h -lm

clean:
	rm -f cachesim
