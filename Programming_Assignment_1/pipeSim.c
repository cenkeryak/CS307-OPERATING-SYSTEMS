#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>

int main(int argc, char *argv[]){

    int fd[2];
    pipe(fd);

    
	

    printf("I`m SHELL process, with PID: %d - Main command is: man grep | grep \"-c,\" -A 2 > output.txt\n",(int) getpid());

    FILE * outputfile = fopen("output.txt","w");
    int outputfileno = fileno(outputfile);

    
    int rc0 = fork();
    if (rc0 < 0) {
		fprintf(stderr, "fork failed\n");
		exit(1);
	}else if (rc0 == 0)
    {
        printf("I`m MAN process, with PID: %d - My command is: man grep\n", (int) getpid());

        int rc1 = fork();
        if (rc1 < 0) {
		fprintf(stderr, "fork failed\n");
		exit(1);
	    }else if (rc1 == 0){
            printf("I` GREP process, with PID: %d - My command is: grep \"-c,\" -A 2\n",(int) getpid());

            close(fd[1]);
            dup2(fd[0],0);
            dup2(outputfileno,1);

            char *Arguments2[5];
            
            Arguments2[0] = strdup("grep");
            Arguments2[1] = strdup("\\-c,");
            Arguments2[2] = strdup("-A 2");
            Arguments2[3] = NULL;
            
            execvp(Arguments2[0],Arguments2);

        }else{
            close(fd[0]);
            dup2(fd[1],1);

            char *Arguments1[5];

            Arguments1[0] = strdup("man");
            Arguments1[1] = strdup("grep");
            Arguments1[2] = NULL;

            execvp(Arguments1[0],Arguments1);

        }
        
    }else{
        wait(NULL);
        printf("I`m SHELL process, with PID: %d - execution is completed, you can find the results in output.txt\n", (int) getpid());
        close(fd[0]);
        close(fd[1]);
        

    }
    




    return 0;
}
