it:	all

progs :=	gaogao
libs  :=	jail_inspect.so

all:	$(progs) $(libs)
clean:
	rm -f $(progs) $(libs)

.SUFFIXES:	.c .so
.c:
	cc -pipe -Wall -pedantic -Os -o $@ $<
.c.so:
	cc -pipe -Wall -pedantic -Os -shared -fPIC -o $@ $<
