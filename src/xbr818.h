/**
 * @file xbr818.h
 * @author pvvx
 */

#ifndef _XBR818_H
#define _XBR818_H

typedef struct {
	u16 trg_ac; // threshold for ac detection
	u16 time_s;  // 1..524 sec
} xbr818_cfg_t;

extern xbr818_cfg_t xbr818_cfg;

int xbr818_init(void);
int xbr818_set_cfg(void);
void xbr818_activate(void);
int xbr818_write_regs(u8 raddr, u8 * pdata, int size);
int xbr818_read_regs(u8 raddr, u8 *pdata, int size);
void xbr818_go_sleep(void);

#endif // _XBR818_H
