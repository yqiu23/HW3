#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main()
{
    char input[500];
    char *argsarray[20];
    while(1){
        printf("CS361 >");
        fgets(input, 300, stdin);
        
        char *word = strtok(input, " ");
        int i = 0;
        int j = 0;
        int ret;
        
        while(word){
            for(j = 0; j <= sizeof(word); j++){
                if(word[j] == "\n"){
                    word[j] = "\0";
                }
            }
            
            argsarray[i] = word;
            word = strtok(NULL, " ");
            ret = strcmp(argsarray[i], "exit");
            
            if(ret == 0){
                exit(0);
            }
            else{
                i = i + 1;
            }
        }
        
        argsarray[i] = (char *)0;
        
        int pid = fork();
        int status;
        
        if(pid == 0){
            execv(argsarray[0], argsarray);
        }
        else{
            wait(&status);
            printf("pid:%d status:%d\n", pid, WEXITSTATUS(status));
        }
    }
    return 0;
}
