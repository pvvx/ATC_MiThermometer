#ifndef __tiny_Flash__
#define __tiny_Flash__

bool  tinyFlash_Init(unsigned long start_addr, unsigned long len);
		
int tinyFlash_Read(unsigned char KEY, unsigned char * buf, unsigned char * len);

int tinyFlash_Write(unsigned char KEY, unsigned char * buf, unsigned char len);

void tinyFlash_Swap(void);

void tinyFlash_Format(void);

void tinyFlash_Debug(unsigned long addr);
#endif