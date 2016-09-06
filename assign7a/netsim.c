#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const int MAX_PROC = 10;

int main(void){
	int i, j,rng,n,num;
	int procNum = 0;
	int parent;
	pid_t pid;
	
	int fd[MAX_PROC][2];

	for(i = 0; i<MAX_PROC; i++){
	 if( (pipe(fd[i]) == -1) ){
	 perror("pipe_error");
	 exit(1);
	 }
	}

	for(i = 1; i<MAX_PROC; i++){
		switch(parent = fork()){
			case -1: perror("fork_error");
				exit(1);
		
			case  0: procNum = i;
				break; 
			}
		if(parent){
			break;
		}
	}

	for(i = 0; i< MAX_PROC; i++){
		if(procNum != i){
			close(fd[i][0]);
		}
	}
	close(fd[procNum][1]);

		srand(1921739*procNum);

		rng = RAND_MAX/10;
		rng *= 10;
		
		for(n=0; n< 9;n++){
			do{
				num = rand();
				j = num%10;
				//printf("this is a random number %d\n",j);
			} while(num >= rng || procNum==j);

			char msg[1024];
			sprintf(msg,"process%d",procNum);
			if(write(fd[j][1],msg,10) != 10)
			{
				printf("J: %d\n", j);
				perror("write_error");
			}
		}
		for(i=0;i<MAX_PROC; i++){
		if(i != procNum)
			close(fd[i][1]);
		}
		char buf[1024];
		while((n = read(fd[procNum][0],&buf,10)) != 0){
		if(n == -1){
		perror("read error");
		}
		printf("process%d has received a message from %s \n", procNum,buf);
		}
		close(fd[procNum][0]);
		exit(0);
		
	}
