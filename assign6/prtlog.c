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
#include "dnet.h"
//#include "pcap.h"


int main(int argc, char *argv[]){
   int fd = 0;
	struct my_pkthdr pkthdr;
	struct pcap_file_header filehdr;
	struct eth_hdr ethhdr;
	struct arp_hdr arp;
	struct ip_hdr  ip;

	char buf[4096];
	int pktlen; 
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
	if(n == -1){
	perror("read error");
	}


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
		pktlen = pkthdr.len;
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
			printf("Actual Packet Length = %d\n", pkthdr.len);
			printf("Ethernet Header\n");
			n = read(fd,&ethhdr, sizeof(struct eth_hdr));
			if(n == -1){
			perror("read error");
			exit(1);
			}
			pktlen -= sizeof(struct eth_hdr);
			//todo error traping	
		  	if((ntohs(ethhdr.eth_type)) == ETH_TYPE_IP){
			
				printf("   IP\n");
				n = read(fd,&ip, sizeof(struct ip_hdr));
				if(n == -1){
				perror("read error");
				exit(1);
				}
				pktlen -= sizeof(struct ip_hdr);
				switch(ip.ip_p){
					case IP_PROTO_ICMP: printf("      ICMP\n\n"); break;
					case IP_PROTO_IGMP: printf("      IGMP\n\n"); break;
					case IP_PROTO_TCP : printf("      TCP\n\n"); break;
					case IP_PROTO_UDP : printf("      UDP\n\n"); break;
						default: printf("UNRECOGNIZED\n\n");
				}
				
			} else if((ntohs(ethhdr.eth_type))== ETH_TYPE_ARP){
				
			n = read(fd,&arp,sizeof(struct arp_hdr));
			if(n == -1){
			perror("read error");
			exit(1);
			}
				pktlen -= sizeof(struct arp_hdr);
				printf("   ARP");
				switch(arp.ar_op){
					case ARP_OP_REQUEST		:printf("      arp operation = Arp Request"); break;
					case ARP_OP_REPLY		:printf("      arp operation = Arp Reply"); break;
					case ARP_OP_REVREQUEST	:printf("      arp operation = Arp Revrequest"); break;
					case ARP_OP_REVREPLY	:printf("      arp operation = Arp Revreply"); break;
						default: printf("UNRECOGNIZED\n\n");	
				}
			}else{
				printf("\tUNRECOGNIZED\n\n");
			
			}
			n = read(fd, buf, pktlen);
			if(n == -1 ){
			perror("read error ");
			exit(1);
			}	  
		}
			
	}


