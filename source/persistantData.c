#include <3ds.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "persistantData.h"

FILE *file;
const char *filename = "cache.bin";
const int size = 0x8;
u8 *buffer;
/* file structure:
 * offset|size|desc
 * ------+----+----
 *   0x00|0x02|magic number (0xE5A7)
 *   0x02|0x02|play coins earned for below date
 *   0x04|0x02|year
 *   0x06|0x01|month
 *   0x07|0x01|day
 */

void buffReset(u8 day, u8 month, u16 year);
void buffSetDate(u8 day, u8 month, u16 year);
bool buffValid();
bool dateCurrent(u8 day, u8 month, u16 year);
void readFileToBuff();
void writeBuffToFile();

/* sets up various buffers and makes sure file is there and is valid
 * if file is not there/invalid, return 2 to indicate something was wrong with file and it's been reset
 * if file is fine but outdated, return 0 to indicate file was reset
 * otherwise return 1 to indicate file is current
 */
Result mydataInit(u8 day, u8 month, u16 year)
{
	Result ret;

	buffer=malloc(size);
	if(!buffer) return -1;

	/* check for file's existence */
	file = fopen(filename, "rb");
	if (file == NULL) {
		ret = 2; //file doesn't exist
REINIT:     /* initialize it */
		buffReset(day, month, year);
		writeBuffToFile();
	} else {
		// seek to end of file
		fseek(file,0,SEEK_END);
		// file pointer tells us the size
		off_t fsize = ftell(file);
		if (fsize != size) {
			ret = 2; //file is wrong size
			goto REINIT;
		}
		fclose(file);

		readFileToBuff();

		if (!buffValid()) {
			ret = 2; //file had invalid magic number
			goto REINIT;
		} else {
			if (dateCurrent(day, month, year)) {
				ret = 1; //file is current
			} else {
				ret = 0; //file is from another day
				goto REINIT;
			}
		}
	}
	return ret;
}

//free buffers
void mydataExit()
{
	free(buffer);
}

u16 getStoredCoins()
{
	u8 lowByte = buffer[2];
	u8 highByte = buffer[3];

	return (u16)lowByte;
	u16 combined;
	u8 *bytes = (void*)&combined;
	bytes[0] = highByte;
	bytes[1] = lowByte;

	return combined;
}
void setStoredCoins(u16 value)
{
	u8 highByte, lowByte;
	highByte = value >> 8 & 0xFF;
	lowByte = value & 0xFF;

	buffer[2] = lowByte;
	buffer[3] = highByte;

	writeBuffToFile();
}

void buffReset(u8 day, u8 month, u16 year)
{
	memset(buffer, 0x0, size);
	buffer[1] = 0xE5;
	buffer[0] = 0xA7;
	buffSetDate(day, month, year);
}

void buffSetDate(u8 day, u8 month, u16 year)
{
	u8 yearHigh = (year >> 8) & 0xFF;
	u8 yearLow = year & 0xFF;
	buffer[4] = yearLow;
	buffer[5] = yearHigh;
	buffer[6] = month;
	buffer[7] = day;
}

bool buffValid()
{
	return buffer[1] == 0xE5 && buffer[0] == 0xA7;
}

bool dateCurrent(u8 day, u8 month, u16 year)
{
	u8 yearHigh = (year >> 8) & 0xFF;
	u8 yearLow = year & 0xFF;
		return buffer[4] == yearLow && buffer[5] == yearHigh && buffer[6] == month && buffer[7] == day;
}

void writeBuffToFile()
{
	file = fopen(filename, "wb+");
	fseek(file, 0, SEEK_SET);
	fwrite(buffer, 1, size, file);
	fflush(file);
	fclose(file);
}

void readFileToBuff()
{
	file = fopen(filename, "rb");
	fseek(file, 0, SEEK_SET);
	fread(buffer,1,size,file);
	fclose(file);
}
