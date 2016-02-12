#ifndef DRIVEH
#define DRIVEH		0xCACA
#define TIMER_TICKS	1
#define HDA_CMDREG	0x3F6
#define HDA_DATAREG	0x110
#define TIMER_PARAM	0xF4
#define CMD_SEEK	0x02
#define CMD_READ	0x04
#define CMD_WRITE	0x06
#define CMD_FORMAT	0x08
#define CMD_DSKNFO	0x016
#define HDA_FILENAME    "vdiskA.bin"
#define SECTORSIZE  	256
#define MAX_SECTOR	16
#define MAX_CYLINDER	16
#define HDA_IRQ         14
#define MASTERBUFFER (*HDA_masterbufferaddress)



//void read_sector(unsigned int cylinder, unsigned int sector,unsigned char* buffer);
//void read_sector_n(unsigned int cylinder, unsigned int sector, unsigned char* buffer, int size);
//void write_sector(unsigned int cylinder, unsigned int sector, const unsigned char* buffer);
//void write_sector_n(unsigned int cylinder, unsigned int sector, const unsigned char* buffer, int size);
void format_sector();
void init_disk();

#endif
