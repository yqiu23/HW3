#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

int builtin_command(char **argv)
 {
    if (!strcmp(argv[0], "quit"))
       exit(0);
    if (!strcmp(argv[0], "&")) 
       return 1;
    return 0; 
 } 

void sigint_handler(int sig){
  char msg[] = "sigint\n";
  char msg2[] = "CS361 >";
  write(1, msg, sizeof(msg));
  write(1, msg2, sizeof(msg2));
}

void sigtstp_handler(int sig) {
  char msg[] = "sigtstp\n";
  char msg2[] = "CS361 >";
  write(1, msg, sizeof(msg));
  write(1, msg2, sizeof(msg2));
}

 int parseline(char *buf, char **argv)
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

void Eval(char *cmdline)
 {
	 char *argv[128]; 
	 char buf[500]; 
	 int bg; 
	 pid_t pid; 

	 strcpy(buf, cmdline);
	 bg = parseline(buf, argv);
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
	    printf("%d %s", pid, cmdline);
	 }
	 return;
 }
 

int main()
{
    char cmdline[500];
    
    while(1){
        signal(SIGINT, sigint_handler);
        signal(SIGINT, sigtstp_handler);
        printf("CS361 >");
        fgets(cmdline, 500, stdin);
        
        if(feof(stdin)){
            exit(0);
        }
        
        int pid = fork();
        if(pid == 0){
            Eval(cmdline);
        }
        else{
            int stat;
            wait(&stat);
            printf("pid:%d status:%d\n", pid, WEXITSTATUS(stat));
        }
        
    }

    return 0;
}
