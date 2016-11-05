#include <stdio.h>
#include <stdint.h>  // for uint32_t, to get 32bit-wide rotates, regardless of the size of int.
#include <limits.h>  // for CHAR_BIT
#include <errno.h>
#include <stdlib.h>

#define ARGCOUNT 1000

extern int errno;

uint32_t schedule_core(uint32_t, uint8_t, uint8_t*, uint8_t*);
uint32_t rotl32 (uint32_t, unsigned int);
 
int main( int argc, char *argv[] ) {
	uint8_t s[256] = {
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
    0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
    0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
    0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
    0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
    0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
    0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
    0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
    0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16 };
	
	uint8_t rcon[8] = { 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40 };
	uint8_t i, tempsa, tempsb, tempsc;
	uint8_t n = 8;
	uint32_t temp;
	static uint32_t expanded[60];
	static uint32_t key[8];
	
	FILE *raw_key;
	FILE *expanded_key;
	
	if(argc != 3) {
		errno = ARGCOUNT;
		perror("Command line argument error;");
		fprintf(stderr, "Error 'argcount': %d\n\n", errno);
		exit(EXIT_FAILURE);
	}
	
	
	raw_key = fopen(argv[1],"rb");
	if (raw_key == NULL) {
		fprintf(stderr, "Can't open output file %s!\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	
	fread(&key, sizeof(temp), 8, raw_key);
		
	for(i=0; i < 8; i++) {
		printf("\nkey[%d] = 0x%08X\n", i, key[i]);
	}
	
	fclose(raw_key);
	
	expanded[0] = key[0];
	expanded[1] = key[1];
	expanded[2] = key[2];
	expanded[3] = key[3];
	expanded[4] = key[4];
	expanded[5] = key[5];
	expanded[6] = key[6];
	expanded[7] = key[7];
	
	for(i = 1; i < 7; i++) {
		expanded[8*i] = schedule_core(expanded[8*i-1], i, s, rcon) ^ expanded[8*i - n]; // Create the first 4-byte block
		
		expanded[8*i + 1] = expanded[8*i] ^ expanded[(8*i - n) + 1]; // Create blocks 2-4
		expanded[8*i + 2] = expanded[8*i+1] ^ expanded[(8*i - n) + 2];
		expanded[8*i + 3] = expanded[8*i+2] ^ expanded[(8*i - n) + 3];
		
		temp = expanded[8*i + 3]; // Data needed for block 5
		tempsa = temp >> 24;
		tempsb = temp >> 16;
		tempsc = temp >> 8;
		
		//Block 5 below
		temp = ((((uint32_t)s[tempsa])<<24) ^ (((uint32_t)s[tempsb])<<16) ^ (((uint32_t)s[tempsc])<<8) ^ (uint32_t)s[((uint8_t)temp)]) ^ expanded[(8*i - n) + 4];
		
		expanded[8*i + 4] = temp; // Assign block 5
		expanded[8*i + 5] = expanded[8*i+4] ^ expanded[(8*i - n) + 5]; // Block 6
		expanded[8*i + 6] = expanded[8*i+5] ^ expanded[(8*i - n) + 6]; // Block 7
		expanded[8*i + 7] = expanded[8*i+6] ^ expanded[(8*i - n) + 7]; // Block 8
	}
	expanded[8*i] = schedule_core(expanded[8*i-1], i, s, rcon) ^ expanded[8*i - n]; // Create the first 4-byte block
		
	expanded[8*i + 1] = expanded[8*i] ^ expanded[(8*i - n) + 1]; // Create blocks 2-4
	expanded[8*i + 2] = expanded[8*i+1] ^ expanded[(8*i - n) + 2];
	expanded[8*i + 3] = expanded[8*i+2] ^ expanded[(8*i - n) + 3];
	
	// 'i' becomes a writing incrementer to a binary file
	
	exit(EXIT_SUCCESS);
}
 
uint32_t schedule_core(uint32_t input, uint8_t i, uint8_t* s, uint8_t* rcon) {
	uint32_t output;
	output = rotl32(input, 8);
	output = (uint32_t)(s[(uint8_t)(output >> 24)] << 24) ^ (uint32_t)(s[(uint8_t)(output >> 16)] << 16) ^ (uint32_t)(s[(uint8_t)(output >> 8)] << 8) ^ (uint32_t)s[(uint8_t) output];
	output = output ^ (((uint32_t)rcon[i]) << 24);
	
	return(output);
};

uint32_t rotl32 (uint32_t value, unsigned int count) {
    const unsigned int mask = (CHAR_BIT*sizeof(value)-1);
    count &= mask;
    return (value<<count) | (value>>( (-count) & mask ));
}

/*
	#include<stdio.h>
	struct rec
	{
		int x,y,z;
	};
	int main()
	{
		int counter;
		FILE *ptr_myfile;
		struct rec my_record;
		ptr_myfile=fopen("test.bin","wb");
		if (!ptr_myfile)
		{
			printf("Unable to open file!");
			return 1;
		}
		for ( counter=1; counter <= 10; counter++)
		{
			my_record.x= counter;
			fwrite(&my_record, sizeof(struct rec), 1, ptr_myfile);
		}
		fclose(ptr_myfile);
		return 0;
	}
	
*/
