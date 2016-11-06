#include <stdio.h>
#include <stdint.h>
#include <error.h>
#include <stdlib.h>
#include <string.h>

#define ARGCOUNT 1000

extern int errno;

int main( int argc, char *argv[] ) {
	uint32_t key[8] = {
		0x471b6795, 0x2e782a39, 0xbc5cc86b, 0xdb6a2302,
		0xe213b913, 0xeaa13ff9, 0x40762ac2, 0x1eba261f };
	
	FILE *key_ptr;
	char address[80];
	
	if(argc != 2) {
		errno = ARGCOUNT;
		perror("Command line argument error;");
		fprintf(stderr, "Error 'argcount': %d\n\n", errno);
		exit(EXIT_FAILURE);
	}
	
	sprintf(address, "$KEY/%s", argv[1])
	
	key_ptr = fopen(address, "w+b");
	
	fwrite(key, 4, 8, key_ptr);
	
	fclose(key_ptr);
	
	return(0);
}
