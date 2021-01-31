/*
 * utils.c
 */
#include "tl_common.h"
#include "drivers.h"
#include "common/utility.h"


inline void bbcopy(register unsigned char * src, register unsigned char * dest, unsigned int len) {
	if (dest < src)
		while (len--)
			*dest++ = *src++;
	else {
		unsigned char *lasts = src + (len - 1);
		unsigned char *lastd = dest + (len - 1);
		while (len--)
			*(unsigned char *) lastd-- = *(char *) lasts--;
	}
}

inline void bcopy(register unsigned char * src, register unsigned char * dest, unsigned int len) {
	bbcopy(src, dest, len);
}

_attribute_ram_code_
void * memset(void * dest, int val, unsigned int len) {
	register unsigned char *ptr = (unsigned char*) dest;
	while (len-- > 0)
		*ptr++ = (unsigned char)val;
	return dest;
}

_attribute_ram_code_
void * memcpy(void * out, const void * in, unsigned int length) {
	bcopy((unsigned char *) in, (unsigned char *) out, (int) length);
	return out;
}

_attribute_ram_code_
int memcmp(const void * m1, const void *m2, unsigned int len) {
	unsigned char *st1 = (unsigned char *) m1;
	unsigned char *st2 = (unsigned char *) m2;

	while(len--){
		if(*st1 != *st2){
			return (*st1 - *st2);
		}
		st1++;
		st2++;
	}
	return 0;
}

/****************************************************************************
 * @fn          addrExtCmp
 *
 * @brief       Compare two extended addresses.
 *
 * input parameters
 *
 * @param       pAddr1        - Pointer to first address.
 * @param       pAddr2        - Pointer to second address.
 *
 * output parameters
 *
 * @return      TRUE if addresses are equal, FALSE otherwise
 */
u8 addrExtCmp(const u8 * pAddr1, const u8 * pAddr2)
{
  u8 i;

  for (i = 8; i != 0; i--)
  {
    if (*pAddr1++ != *pAddr2++)
    {
      return FALSE;
    }
  }
  return TRUE;
}


// general swap/endianess utils

void swapN(unsigned char *p, int n)
{
	int i, c;
	for (i=0; i<n/2; i++)
	{
		c = p[i];
		p[i] = p[n - 1 - i];
		p[n - 1 - i] = c;
	}
}

void swapX(const u8 *src, u8 *dst, int len)
{
    int i;
    for (i = 0; i < len; i++)
        dst[len - 1 - i] = src[i];
}

void swap24(u8 dst[3], const u8 src[3])
{
    swapX(src, dst, 3);
}

void swap32(u8 dst[4], const u8 src[4])
{
    swapX(src, dst, 4);
}

void swap48(u8 dst[7], const u8 src[7])
{
    swapX(src, dst, 6);
}

void swap56(u8 dst[7], const u8 src[7])
{
    swapX(src, dst, 7);
}

void swap64(u8 dst[8], const u8 src[8])
{
    swapX(src, dst, 8);
}

void swap128(u8 dst[16], const u8 src[16])
{
    swapX(src, dst, 16);
}

void net_store_16(u8 *buffer, u16 pos, u16 value)
{
    buffer[pos++] = value >> 8;
    buffer[pos++] = value;
}


void flip_addr(u8 *dest, u8 *src){
    dest[0] = src[5];
    dest[1] = src[4];
    dest[2] = src[3];
    dest[3] = src[2];
    dest[4] = src[1];
    dest[5] = src[0];
}

void store_16(u8 *buffer, u16 pos, u16 value){
    buffer[pos++] = value;
    buffer[pos++] = value >> 8;
}



void my_fifo_init (my_fifo_t *f, int s, u8 n, u8 *p)
{
	f->size = s;
	f->num = n;
	f->wptr = 0;
	f->rptr = 0;
	f->p = p;
}

u8* my_fifo_wptr (my_fifo_t *f)
{
	if (((f->wptr - f->rptr) & 255) < f->num)
	{
		return f->p + (f->wptr & (f->num-1)) * f->size;
	}
	return 0;
}

void my_fifo_next (my_fifo_t *f)
{
	f->wptr++;
}

int my_fifo_push (my_fifo_t *f, u8 *p, int n)
{
	if (((f->wptr - f->rptr) & 255) >= f->num)
	{
		return -1;
	}

	if (n >= f->size)
	{
		return -1;
	}
	u8 *pd = f->p + (f->wptr++ & (f->num-1)) * f->size;
	*pd++ = n & 0xff;
	*pd++ = (n >> 8) & 0xff;
	memcpy (pd, p, n);
	return 0;
}

void my_fifo_pop (my_fifo_t *f)
{
	f->rptr++;
}

u8 * my_fifo_get (my_fifo_t *f)
{
	if (f->rptr != f->wptr)
	{
		u8 *p = f->p + (f->rptr & (f->num-1)) * f->size;
		return p;
	}
	return 0;
}
