#include <stdio.h>
#include <stdlib.h>

#define CHUNKSIZE 192

int main(int argc, char** argv) {
	FILE* tty;
	int c;
	char val;
	char buf[CHUNKSIZE];

	if(argc < 2) {
		fprintf(stderr, "you need to give the tty device as first argument.\n");
		exit(1);
	}
	
	tty = fopen(argv[1], "r+");
	if(tty == NULL) {
		perror("opening TTY");
		exit(2);
	}

	while(1) {
		c = fread(&val, 1, 1, tty);
		if(c != 1) {
			fprintf(stderr, "error reading from TTY\n");
			exit(3);
		}
		c = fread(buf, CHUNKSIZE, 1, stdin);
		if(c != 1) {
			fprintf(stderr, "error reading from stdin\n");
			exit(4);
		}
		fwrite(buf, CHUNKSIZE, 1, tty);
	}
}
