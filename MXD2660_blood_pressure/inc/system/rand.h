#ifndef _RAND_H_
#define _RAND_H_

#define SYS_RAND_MAX 0xffffffff

extern void rand_init(uint32 seed);
extern uint32 get_rand(void);

extern uint8* get_rand_128(uint8* buf);
extern uint8* get_rand_192(uint8* buf);

extern uint8* get_rand_256(uint8* buf);
#endif

