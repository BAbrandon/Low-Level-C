#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include "logprt.h"


int main(int argc, char *argv[]){
   int fd = 0;
	struct my_pkthdr pkthdr;
	struct pcap_file_header filehdr;
	char* buf;
	int n,pnum=0,firsttime=1;
	int b_sec,b_usec,c_usec,c_sec;

	if(argc > 2 ){
	fprintf(stderr, "syntax: \nlogprt <file.log>\n");
	}
	fd = open(argv[1],O_RDONLY);
	if(fd == -1){
	perror(argv[1]);
	exit(1);
	}
	n = read(fd, &filehdr,sizeof(struct pcap_file_header));

	switch(filehdr.magic){
	case PCAP_MAGIC: printf("PCAP_MAGIC\n"); break;	
	case PCAP_SWAPPED_MAGIC: printf("PCAP_SWAPPED_MAGIC\n"); break;
	case PCAP_MODIFIED_MAGIC: printf("PCAP_MODIFIED_MAGIC\n"); break;
	case PCAP_SWAPPED_MODIFIED_MAGIC: printf("PCAP_SWAPPED_MODIFIED_MAGIC\n");break;
	}

	printf("Version major number = %d\n",filehdr.version_major);
	printf("Version minor number = %d\n",filehdr.version_minor);
	printf("GMT to local correction = %d\n",filehdr.thiszone);
	printf("Timestamp accuracy = %d\n",filehdr.sigfigs);
	printf("Snaplen =  %d\n",filehdr.snaplen);
	printf("Linktype = %d\n\n",filehdr.linktype);

	while((n = read(fd, &pkthdr, sizeof(struct my_pkthdr)) > 0)){
			if(firsttime ){
			firsttime = 0;
			b_sec = pkthdr.ts.tv_sec;
			b_usec = pkthdr.ts.tv_usec;
			}

			c_sec =  (unsigned)pkthdr.ts.tv_sec - b_sec;
			c_usec = (unsigned)pkthdr.ts.tv_usec - b_usec;
			while(c_usec < 0){
			c_usec += 1000000;
			c_sec--;
			}

			printf("Packet %d\n",pnum++);
			printf("%05u.%06u\n",(unsigned)c_sec, (unsigned)c_usec);
			printf("Captured Packet Length = %d\n",pkthdr.caplen);
			printf("Actual Packet Length = %d\n\n", pkthdr.len);
		buf =  malloc(pkthdr.caplen);
		n = read(fd, buf, pkthdr.caplen);

	}
}

