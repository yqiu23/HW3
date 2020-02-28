#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>


#define MAXARGS 128
#define MAXLINE 500

/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);
void sigint_handler(int sig);
void sigtstp_handler(int sig);

 int main()
 {
    //signal(SIGINT, sigint_handler);
    //signal(SIGTSTP, sigtstp_handler);
    char cmdline[MAXLINE]; /* Command line */

     while (1) {
        /* Read */
	signal(SIGINT, sigint_handler);
        signal(SIGTSTP, sigtstp_handler);
        printf("CS361 >");
        fgets(cmdline, MAXLINE, stdin);
	//signal(SIGINT, sigint_handler);
    	//signal(SIGTSTP, sigtstp_handler);
        if (feof(stdin))
           exit(0);
        /* Evaluate & Forking */
	int pid = fork();
	if (pid == 0) {
            eval(cmdline);
   	}
	else { 
	    int status;
            wait(&status);
            printf("pid:%d status:%d\n", pid, WEXITSTATUS(status));
	}
     }

 }
/* eval - Evaluate a command line */
 void eval(char *cmdline)
 {
	 char *argv[MAXARGS]; /* Argument list execve() */
	 char buf[MAXLINE]; /* Holds modified command line */
	 int bg; /* Should the job run in bg or fg? */
	 pid_t pid; /* Process id */

	 strcpy(buf, cmdline);
	 bg = parseline(buf, argv);
	 if (argv[0] == NULL)
	 return; /* Ignore empty lines */
	 /* EXIT */
	 if (strcmp(argv[0], "exit") == 0)
		 exit(0);
	 if (!builtin_command(argv)) {
	 if ((pid = fork()) == 0) { /* Child runs user job */
	 if (execve(argv[0], argv, __environ) < 0) {
	 	printf("%s: Command not found.\n", argv[0]);
	 	exit(0);
	 	}
	 }

 /* Parent waits for foreground job to terminate */
	 if (!bg) {
	    int status;
	 if (waitpid(pid, &status, 0) < 0)
	    printf("waitfg: waitpid error");
	 }
	 else
	    printf("%d %s", pid, cmdline);
	 }
	 return;
 }

 /* If first arg is a builtin command, run it and return true */
 int builtin_command(char **argv)
 {
    if (!strcmp(argv[0], "quit")) /* quit command */
       exit(0);
    if (!strcmp(argv[0], "&")) /* Ignore singleton & */
       return 1;
    return 0; /* Not a builtin command */
 }

 /* parseline - Parse the command line and build the argv array */
 int parseline(char *buf, char **argv)
 {
    char *delim; /* Points to first space delimiter */
    int argc; /* Number of args */
    int bg; /* Background job? */

    buf[strlen(buf)-1] = ' '; /* Replace trailing â€™\nâ€™ with space */
    while (*buf && (*buf == ' ')) /* Ignore leading spaces */
    buf++;

    /* Build the argv list */
    argc = 0;
	while ((delim = strchr(buf, ' '))) {
	   argv[argc++] = buf;
	   *delim = '\0';
	    buf = delim + 1;
	    while (*buf && (*buf == ' ')) /* Ignore spaces */
	       buf++;
}
    argv[argc] = NULL;

    if (argc == 0) /* Ignore blank line */
       return 1;

    /* Should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0)
       argv[--argc] = NULL;

    return bg;
}
/* ^c */
void sigint_handler(int sig){
  char msg[] = "\ncaught sigint\n";
  char msg2[] = "CS361 >";
  write(1, msg, sizeof(msg));
  write(1, msg2, sizeof(msg2));
}

/* ^z */
void sigtstp_handler(int sig) {
  char msg[] = "\ncaught sigtstp\n";
  char msg2[] = "CS361 >";
  write(1, msg, sizeof(msg));
  write(1, msg2, sizeof(msg2));
}
