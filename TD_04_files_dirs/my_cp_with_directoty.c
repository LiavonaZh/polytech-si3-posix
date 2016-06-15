#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>

// pour compiler :
// gcc -Wall -std=c99 -o my_cp_with_directory my_cp_with_directopy.c

#define MAX 1024

//! Permissions par défaut des fichiers créés : rw pour tous
// (elles seront en plus filtrées par le umask)
// taken from cours
 #define USER_PERM (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

int is_directory(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}

/**
* simple version of cp command equivalent to (for example):
* cp %filename_1% %filename_2%
* if filename_1 does not exist, an error will be displayed
* if filename_2 does not exist, it will be created
* if filename_2 exists, it will be rewritten
*/
int my_cp(char * from, char * to) {
	// if cannot read file to copy - quit program
	if (open(from, O_RDONLY) < 0) {
		perror("cannot read file\n");
		printf("file name: %s\n", from);
		exit(-1);
	}
	int f_from = open(from, O_RDONLY); // open file in READ Only

	int f_to = open(to, O_WRONLY|O_CREAT|O_TRUNC, USER_PERM); 
	// if file doesnt exist, it is created
	// else - nothing happens

	char buffer[MAX];
	int n;

	while((n = read(f_from, buffer, MAX)) != 0)
 		write(f_to, buffer, n);

 	close(f_from);
 	close(f_to);

	return 0;
}

/**
* copy a file to a directory
* directory must exist (it is checked in main function)
* il manque mkdir donc c'est pas fini mais sinon ceci doit marcher
*/
int my_cp_to_directory(char * from, char * to) {

	if (!is_directory(from)) {
		// get path where to copy file
		char *path = malloc(strlen(to) + 1 + strlen(from) + 1);
		strcpy(path, to);
		strcat(path, "/");
		strcat(path, from);
		printf("path: %s\n", path);
		// copy file
		my_cp(from, path);
	}
	
	else {
		printf("%s is directory\n", from);
		struct dirent *dentry;
		DIR *dirp = opendir(from);
		char *path = malloc(strlen(to) + 1 + strlen(from) + 1);
		strcpy(path, to);
		strcat(path, "/");
		strcat(path, from);
		while((dentry = readdir(dirp)) != NULL) {
			if (strcmp(dentry->d_name, ".") == 0 || strcmp(dentry->d_name, "..") == 0)
                continue;
			my_cp_to_directory(dentry->d_name, path);
		}
	}

	return 0;
}


int main(int argc, char * argv[]) {
	int recursif = 0;
	if (strcmp(argv[1], "-r") == 0) {
		recursif = 1;
		printf("recursion is requiered\n");
	}

	char * directory = argv[argc - 1];
	// if last argument is not directory and there are more than 2 arguments
	// do nothing
	if (!is_directory(directory) && argc > 3) {
		perror("directory doesnt exist or not a directory\n");
		exit(-1);
	}
	// if last argument is not directory and there are 2 arguments (files)
	// copy the first to the second (1st version)
	else if (!is_directory(directory) && argc == 3) {
		if (recursif == 0)
			my_cp(argv[1], argv[2]);
		else {
			my_cp(argv[2], argv[3]);
			printf("il ya -r mais rien à recursivier, on fait une simple copie des fichiers\n");
		}
	}
    
    // if last argument is a directory and there are more than 2 arguments
    // 2nd version of cp
    else {
    	if (recursif == 0) {
	    	for (int i = 1; i < argc - 1; i++) {
	    		my_cp_to_directory(argv[i], directory);
	    	}
	    }
	    else {
	    	for (int i = 2; i < argc-1; i++) {
	    		printf("on est dans le cas recursif\n");
	    		my_cp_to_directory(argv[i], directory);	
	    	}
	    }
    }

	return 0;
}