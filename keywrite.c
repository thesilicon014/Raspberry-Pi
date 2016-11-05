#include <stdio.h>
#include <stdint.h>

int main() {
	uint32_t key[8] = {
		0x471b6795, 0x2e782a39, 0xbc5cc86b, 0xdb6a2302,
		0xe213b913, 0xeaa13ff9, 0x40762ac2, 0x1eba261f };
	
	uint8_t count; 
	FILE *key_ptr;
	
	key_ptr = fopen("/home/pi/development/Raspberry-Pi/key.bin", "w+b");
	
	fwrite(key, 4, 8, key_ptr);
	
	fclose(key_ptr);
	
	return(0);
}
