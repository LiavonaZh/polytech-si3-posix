#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "station.h"

#define MAX_STATIONS 10
#define INIT_STATION 0

char *int_to_char(int i) {
    char *buf = malloc(MSG_LEN);
    sprintf(buf, "%d", i);
    return buf;
}

/*void station(int i) {
	printf("dans station %d\n", i);
	int j;
    char ctab[5]; 
    char *arg[] = {"./station", NULL, NULL}; // arg pour exec a la suite

    dup2(fd_pipes[i][0], 0);
    dup2(fd_pipes[(i + 1) % n_stations][1], 1);

    // on ferme tous les pipes (voir dans programms.c tst_pipes())
    for (j = 0; j < n_stations; j++) {
		close(fd_pipes[j][0]);
		close(fd_pipes[j][1]);
    }

    // exec la station i
    sprintf(ctab, "%d", i); 
    arg[1] = ctab; // arg[] = {"./station.exe", ctab, null}
    // execvp("./station", arg); //normalement
    printf("qq chose dans station\n");
    execvp("./station", NULL); // pour test
    perror("exécution de la station");
    exit(1);
}*/

void close_pipes(int pipes[][2], int number) {
    for (int i = 0; i < number; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}

/**
* initialize a token ring with %number% station via pipes
*/
int ring(int number) {
    if (number > MAX_STATIONS) {
        printf("Too much stations: abort.\n");
        exit(-1);
    }

    printf("Starting token ring with %d stations...\n", number);
    int pipes[number][2];
    for (int i = 0; i < number; i++) {
        pipe(pipes[i]);
    }

    for (int j = 0; j < number; j++) {
        if (fork() == 0) {
            dup2(pipes[j][0], 0);
            dup2(pipes[(j + 1) % number][1], 1);
            close_pipes(pipes, number);

            execl("./station", "./station", int_to_char(j), NULL);
            fprintf(stderr, "[%d] This station couldn't be spawned\n", j);
            exit(-1);
        }
    }

    //injecting the first frame
    if (fork() == 0) {
        dup2(pipes[INIT_STATION][1], 1);
        close_pipes(pipes, number);
        // flag, station_origin, station_dest, msg
        Packet p = {FREE, -1, 0, ""};
        write(1, &p, sizeof(Packet));
        exit(0);
    }

    //nbStations + 1 'cause we want to wait for the injecting process to end too
    for (int k = 0; k < number + 1; k++) {
        wait(0);
    }

    close_pipes(pipes, number);
    printf("Bye\n");

    return 0;
}

int main(int argc, char const *argv[]) {
    int number_stations;
    printf("Enter the number of stations (0 < n < 11): \n");
    scanf("%d", &number_stations);
	ring(number_stations);
	return 0;
}