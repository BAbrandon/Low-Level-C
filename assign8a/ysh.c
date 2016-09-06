#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

struct cmd{
	int redirect_in;     /* Any stdin redirection?         */
	int redirect_out;    /* Any stdout redirection?        */
	int redirect_append; /* Append stdout redirection?     */
	int background;      /* Put process in background?     */
	int piping;          /* Pipe prog1 into prog2?         */
	char *infile;        /* Name of stdin redirect file    */
	char *outfile;       /* Name of stdout redirect file   */
	char *argv1[10];     /* First program to execute       */
	char *argv2[10];     /* Second program in pipe         */
 };

int cmdscan(char *cmdbuf, struct cmd *com);

int main(void){
	char buf[1024];
	struct cmd command;
	int i, fd[2],fdin,fdout;

	printf(">>");
	while((strcmp((fgets(buf,sizeof(buf),stdin)),"exit\n")) != 0){
		if(cmdscan(buf,&command)){
		printf("Illegal format \n>>");
		continue;
		}	
		if(command.piping){
			if(pipe(fd)){
				perror("pipe_error");
				exit(-1);
			}
		}
		if(command.redirect_in){

			if((fdin = open(command.infile,O_RDONLY)) ==-1){
				perror("infile open_error");
				exit(1);
			}
		
		}

		if(command.redirect_out){
			if((fdout = open(command.outfile,O_WRONLY|O_CREAT|O_TRUNC,0600))== -1){
				perror("outfile open_error");
				exit(1);
			}
		}

		if(command.redirect_append){
			if((fdout = open(command.outfile,O_WRONLY|O_CREAT|O_APPEND,0600))== -1){
				perror("outfile open_error");
				exit(1);
			}
		}
		if(command.piping == 0){
			switch(fork()){
				case -1: perror("fork_error");
						 exit(-1);
				case  0: 
						if(command.background){
							switch(fork()){
								case -1: perror("fork_error");
										 exit(-1);
								case  0:
										 if(command.background){
											if(command.redirect_in){
												dup2(fdin,STDIN_FILENO);
												close(fdin);
											}
											if(command.redirect_out || command.redirect_append){
												dup2(fdout,STDOUT_FILENO);
												close(fdout);
											}								 
											if(command.argv1[0]!= NULL){
												execvp(command.argv1[0],command.argv1);
												perror("exec_error");
												exit(1);
											}else{
												printf("illegal format");
												exit(1);
											}
										}

								default:
									exit(1);
							}
					
					
					}

							if(command.redirect_in){
								dup2(fdin,STDIN_FILENO);
								close(fdin);
							}
							if(command.redirect_out || command.redirect_append){
								dup2(fdout,STDOUT_FILENO);
								close(fdout);
							}
						execvp(command.argv1[0],command.argv1);
						perror("exec_error");
						exit(1);
					
				default:	
					wait(NULL);	
					
			}
		}
	if(command.piping){
		switch(fork()){
			case -1: perror("fork_error");
					 exit(1);
		    case  0: 
					 switch(fork()){
						 case -1: perror("fork_error");
								  exit(1);
						 case  0:
								if(command.background != 1){
								
									dup2(fd[0], STDIN_FILENO);
									close(fd[1]);
									close(fd[0]);
								
									if(command.redirect_out || command.redirect_append){
										dup2(fdout, STDOUT_FILENO);
										close(fdout);
									}
									execvp(command.argv2[0],command.argv2);
									perror("exec_error");
									exit(1);
								}else{
								 switch(fork()){
									case -1: perror("fork_error");
											 exit(1);
									case  0:
											 dup2(fd[0],STDIN_FILENO);
											 close(fd[1]);
											 close(fd[0]);
											if(command.redirect_out || command.redirect_append){
												dup2(fdout, STDOUT_FILENO);
												close(fdout);
											}
											 execvp(command.argv2[0],command.argv2);
											 perror("exec_error");
											 exit(1);
									default:
											dup2(fd[1], STDOUT_FILENO);
											close(fd[1]);
											close(fd[0]);
											if(command.redirect_in){
												dup2(fdin,STDIN_FILENO);
												close(fdin);
											}
											execvp(command.argv1[0],command.argv1);
											perror("exec_error");
											exit(1);
								 }
								//exit(1);	
								}
						default:
								if(command.background != 1){
									dup2(fd[1], STDOUT_FILENO);
									close(fd[1]);
									close(fd[0]);
									if(command.redirect_in){
										dup2(fdin,STDIN_FILENO);
										close(fdin);
									}
									execvp(command.argv1[0],command.argv1);
									perror("exec_error");
									exit(1);
								}else{
									exit(1);
								}
					 
					 }
		
			default:
				 wait(NULL);
		
		
		
		}
	}
		

	printf(">>");
	}
}
