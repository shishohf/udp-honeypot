#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include <arpa/inet.h>

#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/ether.h>

int main() {
	FILE *log;
	log = fopen("/var/www/html/index.html", "a+");
        int listener_socket = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
        if(listener_socket < 0) {
                printf("Could not open raw socket\n");
                exit(0);
        }
        unsigned char *receive_buffer = (unsigned char *)malloc(65536);
        while(1) {
        socklen_t data_size;
        data_size = recvfrom(listener_socket, receive_buffer, 65536, 0, NULL, NULL);
                if(data_size < 0) {
                        printf("Receive error\n");
                        exit(0);
                }
        struct iphdr *iph = (struct iphdr*)(receive_buffer + 14);
                if(iph->protocol == 17) {
                        u_int16_t iphdrlen = iph->ihl*4;
                        struct udphdr *udph = (struct udphdr*)(receive_buffer + 14 + iphdrlen);
			struct in_addr addr;
			addr.s_addr = iph->saddr;
			int hours, minutes, seconds, day, month, year;
			time_t now;
			time(&now);
			struct tm *local = localtime(&now);
			hours = local->tm_hour;
			minutes = local->tm_min;
    			seconds = local->tm_sec;
    			day = local->tm_mday;
    			month = local->tm_mon + 1;
    			year = local->tm_year + 1900;
			fprintf(log, "UDP Probe Received: %02d/%02d/%d", month, day, year);
			fprintf(log, " %02d:%02d:%02d | ", hours, minutes, seconds);
                        fprintf(log, "Source IP: %s | Destination Port: %u | Payload Length: %i | UDP Payload: ", inet_ntoa(addr), ntohs(udph->dest), (data_size - 14) - iphdrlen - 8);
                        	for(int i = 14 + iphdrlen + 8; i < data_size; i++) {
                                        fprintf(log, "\\x%02hhX", receive_buffer[i]);
                        	}
                        fprintf(log, "\n");
			fflush(log);
                }
        }
	return 0;
}
