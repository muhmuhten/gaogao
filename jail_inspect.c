#include <stdio.h>
#include <sys/param.h>
#include <sys/jail.h>
#include <errno.h>
#include <sys/uio.h>
#include <dlfcn.h>
#include <string.h>
#include <stdlib.h>

int jail(struct jail *jail) {
	int (*old)(struct jail *) = (int (*)(struct jail *))dlsym(RTLD_NEXT, "jail");
	int res = old(jail);
	printf("jail(%p) = %d\n", (void *)jail, res);
	if (res == -1) printf("%d %s\n", errno, strerror(errno));
	return res;
}

int jail_get(struct iovec *iov, u_int niov, int flags) {
	int (*old)(struct iovec *, u_int, int) =
		(int (*)(struct iovec *, u_int, int))dlsym(RTLD_NEXT, "jail_get");
	struct iovec *oiov = iov;
	u_int oniov = niov;
	printf("iovec * %p\n", (void *)iov);
	while (niov--) {
		char **base = iov->iov_base;
		int len = (int)iov->iov_len;
		printf("\t%d %d %*s\n", len, base ? (int)*base : 0, -len, (char *)base);
		iov++;
	}
	int res = old(oiov, oniov, flags);
	printf("jail_get(%p, %u, %d) = %d\n", (void *)iov, niov, flags, res);
	if (res == -1) printf("%d %s\n", errno, strerror(errno));
	return res;
}

int jail_set(struct iovec *iov, u_int niov, int flags) {
	unsetenv("LD_PRELOAD");
	int (*old)(struct iovec *, u_int, int) =
		(int (*)(struct iovec *, u_int, int))dlsym(RTLD_NEXT, "jail_set");
	struct iovec *oiov = iov;
	u_int oniov = niov;
	int res = old(oiov, oniov, flags);
	printf("iovec * %p\n", (void *)iov);
	while (niov--) {
		char **base = iov->iov_base;
		int len = (int)iov->iov_len;
		printf("\t%d %d %*s\n", len, base ? (int)*base : 0, -len, (char *)base);
		iov++;
	}
	printf("jail_set(%p, %u, %d) = %d\n", (void *)iov, niov, flags, res);
	if (res == -1) printf("%d %s\n", errno, strerror(errno));
	return res;
}

int jail_attach(int jid) {
	int (*old)(int) = (int (*)(int))dlsym(RTLD_NEXT, "jail_attach");
	int res = old(jid);
	printf("jail_attach(%d) = %d\n", jid, res);
	if (res == -1) printf("%d %s\n", errno, strerror(errno));
	return res;
}

int jail_remove(int jid) {
	int (*old)(int) = (int (*)(int))dlsym(RTLD_NEXT, "jail_remove");
	int res = old(jid);
	printf("jail_remove(%d) = %d\n", jid, res);
	if (res == -1) printf("%d %s\n", errno, strerror(errno));
	return res;
}
