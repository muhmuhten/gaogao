#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/param.h>
#include <sys/jail.h>
#include <sys/uio.h>

int main(int const argc, char **argv) {
	int niov = 0, mode = JAIL_CREATE|JAIL_ATTACH;
	struct iovec *const jiov = alloca(argc * sizeof(struct iovec));

	if (!*argv) return 2;
	
	while (*++argv && **argv) {
		if (**argv == ' ') ++*argv;
		switch (**argv) {
			case '-':
				while (*++*argv) {
					switch (**argv) {
						case 'c': mode |= JAIL_CREATE;  break;
						case 'u': mode |= JAIL_UPDATE;  break;
						case 'a': mode |= JAIL_ATTACH;  break;
						case 'd': mode |= JAIL_DYING;   break;
						case 'C': mode &= ~JAIL_CREATE; break;
						case 'U': mode &= ~JAIL_UPDATE; break;
						case 'A': mode &= ~JAIL_ATTACH; break;
						case 'D': mode &= ~JAIL_DYING;  break;
						default:
							fprintf(stderr, "Illegible mode '%c'.\n", **argv);
							return 2;
					}
				}
				continue;

			case 'S':
				jiov[niov].iov_base = *argv+1;
				jiov[niov].iov_len  = strlen(*argv);
				break;

			case '0':
				jiov[niov].iov_base = NULL;
				jiov[niov].iov_len  = 0;
				break;

			case 'J':
				jiov[niov].iov_len  = sizeof(int);
				jiov[niov].iov_base = alloca(jiov[niov].iov_len);
				*(int *)jiov[niov].iov_base = (int)strtol(*argv+1, NULL, 0);
				break;

			default:
				fprintf(stderr, "Illegible parameter \"%s\".\n", *argv);
				return 2;
		}
		niov++;
	}

	if (jail_set(jiov, niov, mode) == -1) {
		perror("jail_set");
		return 1;
	}

	if (!*argv || !*++argv) return 0;

	execvp(*argv, argv);
	perror(*argv);
	return 1;
}
/* ex: se ts=4 sw=4 noet : */
