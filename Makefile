it:	all

progs :=	gaogao
libs  :=	jail_inspect.so

all:	$(progs) $(libs)
clean:
	rm -f $(progs) $(libs)

.SUFFIXES:	.c .so
.c:
	cc -pipe -Wall -pedantic -Os -s -o $@ $<
.c.so:
	cc -pipe -Wall -pedantic -Os -s -shared -fPIC -o $@ $<
