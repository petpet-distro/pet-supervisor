#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

unsigned int restart = 1;
unsigned int quit = 0;

void
sigterm(int signo)
{
	quit++;
	return;
}

void
sigchld(int signo)
{
	restart++;
	return;
}

void
applyHandlers(void)
{
	struct sigaction sigh = { 0 };

	sigh.sa_handler = &sigterm;
	sigaction(SIGTERM, &sigh, NULL);
	sigh.sa_handler = &sigchld;
	sigaction(SIGCHLD, &sigh, NULL);

	return;
}

int
spawn(char *const argv[])
{
	int ret = 0;

	ret = fork();
	if (ret == 0) {
		setsid();
		execvp(argv[0], argv);

		perror("execvp(2)");
		exit(1);
	}

	if (ret == -1) {
		perror("fork(2)");
		return -1;
	}

	return ret;
}

int
main(int argc, char **argv)
{
	int procpid = 0;

	applyHandlers();

	if (!(argc > 1)) {
		dprintf(2, "Too few arguments! Please hand over the argv of the program!\n");
		return 2;
	}

	while (1) {
		if (quit) {
			kill(procpid, SIGTERM);
			sleep(2);

			if (restart) {
				return 0;
			}

			kill(procpid, SIGKILL);
			return 0;
		}

		if (restart) {
			procpid = spawn(&argv[1]);
			restart = 0;
		}

		pause();
	}

	return 0;
}
