#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#define BUFSIZE 1024

int main(int argc,char *argv[]){
	int n, fdin1,fdin2,fdout, nofile,dash1,dash2;
	char buf[BUFSIZE];

	if(argc <3){
		fprintf(stderr,"Not enough arguments\n");
		exit(1);
	}
	if(*argv[1] == '-' && *argv[2] == '-'){
		fprintf(stderr,"Both inputs can't be STDIN\n");
		exit(1);
	}
	if(argv[3]){
		fdout = open(argv[3],O_WRONLY|O_CREAT|O_APPEND,0600);
		if(fdout == -1){
			perror(argv[3]);
			exit(1);
		}
	}else{
		nofile = 1;
	}
	if(*argv[1] != '-'){
		fdin1 = open(argv[1], O_RDONLY);
		if(fdin1 == -1){
			perror(argv[1]);
			if(nofile == 0){
				close(fdout);
			}
			exit(1);
		}
	}else{
	dash1 = 1;
	}
	if(*argv[2] != '-'){
		fdin2 = open(argv[2], O_RDONLY);
		if(fdin2 == -1){
			perror(argv[1]);
			if(nofile ==0){
			close(fdout);
			}
			if(dash1 == 0){
			close(fdin1);
			}
			exit(1);
		}
	}else{
	dash2 = 1;
	}

	if(dash1 == 1){
		fdin1 = STDIN_FILENO;
	}
	if(nofile == 1){
		fdout = STDOUT_FILENO;
	}
	while((n = read(fdin1, buf, BUFSIZE))>0){
		write(fdout,buf,n);
	}
	if(n == -1){
		perror("read error");
		if(nofile == 0){
			close(fdout);
		}
		if(dash1 == 0){
			close(fdin1);
		}
		exit(1);
	}
	if(dash2 == 1){
	fdin2 = STDIN_FILENO;
	}
	while((n = read(fdin2, buf,BUFSIZE))>0){
		write(fdout,buf,n);
	}
	if(n == -1){
		perror("read error");
		if(dash1 == 0){
			close(fdin1);
		}
		if(nofile == 0){
			close(fdout);
		}
		if(dash2 == 0){
			close(fdin2);
		}
	}
	if(dash1 ==0){
		close(fdin1);
	}
	if(dash2 == 0){
		close(fdin2);
	}
	if(nofile == 0){
		close(fdout);
	}
exit(0);
}
