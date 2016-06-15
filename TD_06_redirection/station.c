#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "station.h"

#define STOP_SIG "stop\n"

FILE *fp_in;

char * create_station(int n) {
	char *buf = malloc(MSG_LEN);
	sprintf(buf, "[%d]", n);
	return buf;
}

void enter_message(FILE *in, Packet *p, char *station, char *buf) {
	// destination
	fprintf(stderr, "%s To which station? (No >= 0)\n", station);
	fgets(buf, MSG_LEN, in);
	p->station_dest = atoi(buf);

	// message
	fprintf(stderr, "%s What is the message?\n", station);
	fgets(buf, MSG_LEN, in);
	strcpy(p->msg, buf);
}

int main(int argc, char **argv) {
	int id = atoi(argv[1]);
	char *station = create_station(id);
	char *buffer = malloc(MSG_LEN);

	fp_in = fopen("/dev/tty", "r");

	Packet *p = (Packet *) malloc(sizeof(Packet));

	while(1) {
		if (read(0, p, sizeof(Packet)) == -1) {
			//read failed
			fprintf(stderr, "%s couldn't read the last frame. The station will propagate a STOP signal\n"
					, station);
			//clean frame
			p = (Packet *) malloc(sizeof(Packet));
			p->m_token = STOP;
		}

		if (p->m_token != FREE) {
			// this station isn't origin nor destination
			if (p->station_dest != id && p->station_origin != id) {
				fprintf(stderr, "%s Passing to next\n", station);
			}

			else {
				// this station is destination: show msg and acknowledge packet
                if (p->station_dest == id) {
				    fprintf(stderr, "%s Got a new message from [%d] :\n", station, p->station_origin);
					fprintf(stderr, "%s", p->msg);
					p->m_token = ACK;
				}
				// this station is orgin: free packet
				if (p->station_origin == id) {
				    p->m_token = FREE;
				}
			}
		}
		// packet is free
		else {
			fprintf(stderr, "%s The frame is FREE, do you want to send something? (y/N/stop)\n", station);
			fgets(buffer, MSG_LEN, fp_in);

			if (buffer[0] == 'y') {
				enter_message(fp_in, p, station, buffer);
				p->station_origin = id;
				p->m_token = BUSY;
			} 
		}
		write(1, p, sizeof(Packet));
	}

	fprintf(stderr, "%s Closing station\n", station);

	return 0;
}