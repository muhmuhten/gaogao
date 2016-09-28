all:	gaogao jail_inspect.so
clean:
	rm -f gaogao jail_inspect.so

gaogao:	gaogao.c
	cc -pipe -Wall -pedantic -Os -o $@ $>

jail_inspect.so:	jail_inspect.c
	cc -pipe -Wall -pedantic -Os -shared -fPIC -o $@ $>
