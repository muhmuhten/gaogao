#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <err.h>

#include <sys/param.h>
#include <sys/jail.h>
#include <sys/uio.h>

int main(int const argc, char **argv) {
	int niov = 0, mode = JAIL_CREATE|JAIL_ATTACH;
	struct iovec jiov[argc];

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
				default: errx(2, "Invalid mode '%c'.", **argv);
				}
			}
			continue; /* don't increment niov */

		case 'S':
			jiov[niov].iov_base = *argv+1;
			jiov[niov].iov_len  = strlen(*argv);
			break;

		case '0':
			jiov[niov].iov_base = NULL;
			jiov[niov].iov_len  = 0;
			break;

		case 'B':
			/* NOTE: it's safe to write this directly back into argv because a
			 * char takes up only one byte, space for which is provided by the
			 * char skipped before each strtol(). */
			jiov[niov].iov_base = *argv;
			jiov[niov].iov_len  = 0;

			/* We must check **argv, as otherwise we'd skip over the '\0' at
			 * the end of an argument and try parsing whatever comes next. */
			while (**argv && *++*argv) {
				*((char *)jiov[niov].iov_base + jiov[niov].iov_len++) =
					strtol(*argv, argv, 0);
			}
			break;

		case 'J':
			/* NOTE: it is NOT safe to write back into argv! The argument may
			 * not be long enough to store an int to the same space. */
			jiov[niov].iov_len  = sizeof(int);
			jiov[niov].iov_base = alloca(jiov[niov].iov_len);
			*(int *)jiov[niov].iov_base = (int)strtol(*argv+1, NULL, 0);
			break;

		default: errx(2, "Illegible parameter \"%s\".", *argv);
		}
		niov++;
	}

	if (jail_set(jiov, niov, mode) == -1)
		err(1, "jail_set");
	if (!*argv || !*++argv) return 0;

	execvp(*argv, argv);
	err(1, "%s", *argv);
}
