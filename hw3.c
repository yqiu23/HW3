#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>


#define MAXARGS 128
#define MAXLINE 500

/* Function prototypes */
void Eval(char *cmd);
int parse(char *buf, char **argv);
int builtin_command(char **argv);
void sigint_handler(int sig);
void sigtstp_handler(int sig);

 int main()
 {
    char cmd[MAXLINE]; 

     while (1) {

	signal(SIGINT, sigint_handler);
        signal(SIGTSTP, sigtstp_handler);
        printf("CS361 >");
        fgets(cmd, MAXLINE, stdin);

        if (feof(stdin))
           exit(0);
           
	int pid = fork();
	if (pid == 0) {
            Eval(cmd);
   	}
	else { 
	    int status;
            wait(&status);
            printf("pid:%d status:%d\n", pid, WEXITSTATUS(status));
	}
     }

 }

 void Eval(char *cmd)
 {
	 char *argv[MAXARGS]; 
	 char buf[MAXLINE];
	 int bg; 
	 pid_t pid; 

	 strcpy(buf, cmd);
	 bg = parse(buf, argv);
	 if (argv[0] == NULL)
	 return; 

	 if (strcmp(argv[0], "exit") == 0)
		 exit(0);
	 if (!builtin_command(argv)) {
	 if ((pid = fork()) == 0) { 
	 if (execve(argv[0], argv, __environ) < 0) {
	 	printf("%s: Command not found.\n", argv[0]);
	 	exit(0);
	 	}
	 }


	 if (!bg) {
	    int status;
	 if (waitpid(pid, &status, 0) < 0)
	    printf("waitfg: waitpid error");
	 }
	 else
	    printf("%d %s", pid, cmd);
	 }
	 return;
 }


 int builtin_command(char **argv)
 {
    if (!strcmp(argv[0], "quit")) 
       exit(0);
    if (!strcmp(argv[0], "&")) 
       return 1;
    return 0; 
 }


 int parse(char *buf, char **argv)
 {
    char *delim; 
    int argc;
    int bg; 

    buf[strlen(buf)-1] = ' '; 
    while (*buf && (*buf == ' ')) 
    buf++;


    argc = 0;
	while ((delim = strchr(buf, ' '))) {
	   argv[argc++] = buf;
	   *delim = '\0';
	    buf = delim + 1;
	    while (*buf && (*buf == ' ')) 
	       buf++;
}
    argv[argc] = NULL;

    if (argc == 0) 
       return 1;

    if ((bg = (*argv[argc-1] == '&')) != 0)
       argv[--argc] = NULL;

    return bg;
}

void sigint_handler(int sig){
  char msg[] = "\ncaught sigint\n";
  char msg2[] = "CS361 >";
  write(1, msg, sizeof(msg));
  write(1, msg2, sizeof(msg2));
}


void sigtstp_handler(int sig) {
  char msg[] = "\ncaught sigtstp\n";
  char msg2[] = "CS361 >";
  write(1, msg, sizeof(msg));
  write(1, msg2, sizeof(msg2));
}
