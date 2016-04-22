#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/param.h>
#include <sys/jail.h>
#include <sys/uio.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int const argc, char **argv) {
	int niov = 0, jid;
	struct iovec *jiov = alloca(argc * sizeof(struct iovec));

	if (!*argv) return 22;
	
	while (*++argv && **argv) {
		if (**argv == ' ') ++*argv;
		switch (**argv) {
			case 'A':
				jiov[niov].iov_len  = sizeof(struct in_addr);
				jiov[niov].iov_base = alloca(jiov[niov].iov_len);
				if (inet_pton(AF_INET, *argv+1, jiov[niov].iov_base) != 1) {
					fprintf(stderr, "Illegible address \"%s\".\n", *argv+1);
					return 22;
				}
				break;

			case 'G':
				jiov[niov].iov_len  = sizeof(struct in6_addr);
				jiov[niov].iov_base = alloca(jiov[niov].iov_len);
				if (inet_pton(AF_INET6, *argv+1, jiov[niov].iov_base) != 1) {
					fprintf(stderr, "Illegible address \"%s\".\n", *argv+1);
					return 22;
				}
				break;


			case 'J':
				jiov[niov].iov_len  = sizeof(int);
				jiov[niov].iov_base = alloca(jiov[niov].iov_len);
				*(int *)jiov[niov].iov_base = (int)strtol(*argv+1, NULL, 0);
				break;

			case 'S':
				jiov[niov].iov_len  = strlen(*argv);
				jiov[niov].iov_base = *argv+1;
				break;

			case 'Z':
				jiov[niov].iov_base = NULL;
				jiov[niov].iov_len  = 0;
				break;

			default:
				fprintf(stderr, "Illegible parameter \"%s\".\n", *argv);
				return 22;
		}
		niov++;
	}
	
	jid = jail_set(jiov, niov, JAIL_CREATE|JAIL_UPDATE|JAIL_ATTACH);
	if (jid == -1) {
		perror("jail_set");
		return errno;
	}

	if (*argv && *++argv) {
		execvp(*argv, argv);
		perror(*argv);
		return errno;
	}
	return 0;
}
/* ex: se ts=4 sw=4 noet : */
