#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <err.h>

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
						default: errx(2, "Invalid mode '%c'.", **argv);
					}
				}
				continue; // don't increment niov

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

			case 'T':
				jiov[niov].iov_base = *argv+2;
				jiov[niov].iov_len  = 0;

				if (*++*argv) {
					char nul = **argv;
					while (*++*argv) {
						if (**argv == nul) {
							**argv = 0;
						}
						jiov[niov].iov_len++;
					}
				}
				break;

			case 'H':
				jiov[niov].iov_base = *argv+1;
				jiov[niov].iov_len  = 0;

				while (*++*argv) {
					char nib = **argv;
					if      ('0' <= nib && nib <= '9') nib -= '0';
					else if ('A' <= nib && nib <= 'F') nib -= 'A'-10;
					else if ('a' <= nib && nib <= 'f') nib -= 'a'-10;
					else errx(2, "Invalid hex digit '%c'.", nib);

					char *pos = *argv - ++jiov[niov].iov_len/2;
					if (jiov[niov].iov_len & 1) {
						*pos = nib << 4;
					}
					else {
						*pos |= nib;
					}
				}

				if (jiov[niov].iov_len & 1)
					errx(2, "Hex not byte-aligned.");
				jiov[niov].iov_len /= 2;
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
/* ex: se ts=4 sw=4 noet : */
