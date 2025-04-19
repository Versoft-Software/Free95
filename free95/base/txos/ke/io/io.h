#ifndef IO_H
#define IO_H

extern unsigned char insb(unsigned short port);
extern unsigned short insw(unsigned short port);

extern void outb(unsigned short port, unsigned char val);
extern void outw(unsigned short port, unsigned short val);

#endif
