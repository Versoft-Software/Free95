#ifndef IO_H
#define IO_H

unsigned char insb(unsigned short port);
unsigned short insw(unsigned short port);

void outb(unsigned short port, unsigned char val);
void outw(unsigned short port, unsigned short val);

#endif
