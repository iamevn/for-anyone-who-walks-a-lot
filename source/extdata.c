/* #include <3ds.h> */
#include </opt/devkitpro/libctru/include/3ds.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "archive.h"
#include "extdata.h"

/* gamecoin.dat [http://3dbrew.org/wiki/Extdata#Shared_Extdata_0xf000000b_gamecoin.dat]
 * size: 0x14
 *
 * offset| size | description
 * ------+------+-------------------------------------------------------------
 *  0x00 | 0x04 | Magic number: 0x4F00
 *  0x04 | 0x02 | Total Play Coins
 *  0x06 | 0x02 | Total Play Coins obtained on the date stored below.
 *       |      | When the below date does not match the current date,
 *  	 |  	| this field is reset to zero, then the date (and other
 *  	 |  	| fields) are updated. Once this value is >= 10, no more
 *  	 |  	| Play Coins can be obtained until the current date changes.
 *  	 |  	|
 *  0x08 | 0x04 | Total step count at the time a new Play Coin was obtained
 *  0x0C | 0x04 | Step count for the day the last Play Coin was obtained,
 *       |      | for that day's step count (same as the step count displayed
 *  	 |  	| by home-menu when this file was updated).
 *  	 |  	|
 *  0x10 | 0x02 | Year
 *  0x12 | 0x01 | Month
 *  0x13 | 0x01 | Day
 */

u8 *filebuffer;
u32 filebuffer_maxsize = 0x400000;


/* Initialization */
Result extdataInit()
{
	filebuffer = malloc(0x400000);
	if (filebuffer == NULL) {
		return -1;
	}
	memset(filebuffer, 0, filebuffer_maxsize);
	return open_extdata();
}

void extdataExit()
{
	close_extdata();
	return free(filebuffer);
}

/* Total Play Coins - 0x04 0x02 */
Result setcoins(u8 highByte, u8 lowByte)
{	
	Result ret=0;

	/* printf("Reading gamecoin.dat...\n"); */
	/* gfxFlushBuffers(); */
	/* gfxSwapBuffers(); */

	ret = archive_readfile(GameCoin_Extdata, "/gamecoin.dat", filebuffer, 0x14);
	if(ret!=0)
	{
		printf("Failed to read file: 0x%08x\n", (unsigned int)ret);
		gfxFlushBuffers();
		gfxSwapBuffers();
		return 0;
	}

	filebuffer[0x4]=lowByte;
	filebuffer[0x5]=highByte;

	/* printf("Writing updated gamecoin.dat...\n"); */
	/* gfxFlushBuffers(); */
	/* gfxSwapBuffers(); */

	ret = archive_writefile(GameCoin_Extdata, "/gamecoin.dat", filebuffer, 0x14);
	if(ret!=0)
	{
		printf("Failed to write file: 0x%08x\n", (unsigned int)ret);
		gfxFlushBuffers();
		gfxSwapBuffers();
	}

	return 0;
}

Result setCoins(u16 value)
{
	if (value > 300) {
		printf("can't set coins to %d\n", (unsigned int)value);
		gfxFlushBuffers();
		gfxSwapBuffers();
		return -1;
	}
	u8 highByte, lowByte;
	highByte = value >> 8 & 0xFF;
	lowByte = value & 0xFF;
	return setcoins(highByte, lowByte);
}

u16 getCoins()
{
	Result ret=0;

	ret = archive_readfile(GameCoin_Extdata, "/gamecoin.dat", filebuffer, 0x14);
	if(ret!=0) {
		printf("failed to read file: 0x%08x\n", (unsigned int)ret);
		gfxFlushBuffers();
		gfxSwapBuffers();
		return 0;
	}

	u8 lowByte = filebuffer[0x4];
	u8 highByte = filebuffer[0x5];

	u16 combined = highByte;
	combined <<= 8;
	combined = combined | lowByte;
	return combined;
}

Result changeCoins(s16 amt)
{
	s16 cnt = getCoins();
	s16 final = cnt + amt;

	if (final < 0 || final > 300) {
		if (amt > 0) {
			final = 300;
		}
		else {
			final = 0;
		}
	}
	return setCoins(final);
}

/* Daily Play Coins - 0x06 0x02 */
Result setdaycoins(u8 highByte, u8 lowByte)
{
	Result ret = 0;

	printf("Reading gamecoin.dat...\n");
	gfxFlushBuffers();
	gfxSwapBuffers();

	ret = archive_readfile(GameCoin_Extdata, "/gamecoin.dat", filebuffer, 0x14);
	if (ret != 0)
	{
		printf("Failed to read file: 0x%08x\n", (unsigned int)ret);
		gfxFlushBuffers();
		gfxSwapBuffers();
		return 0;
	}

	filebuffer[0x6] = lowByte;
	filebuffer[0x7] = highByte;

	printf("Writing updated gamecoin.dat...\n");
	gfxFlushBuffers();
	gfxSwapBuffers();

	ret = archive_writefile(GameCoin_Extdata, "/gamecoin.dat", filebuffer, 0x14);
	if (ret != 0)
	{
		printf("Failed to write file: 0x%08x\n", (unsigned int)ret);
		gfxFlushBuffers();
		gfxSwapBuffers();
	}

	return 0;
}

Result setDayCoins(u16 value)
{
	u8 highByte, lowByte;
	highByte = value >> 8 & 0xFF;
	lowByte = value & 0xFF;
	return setdaycoins(highByte, lowByte);
}

u16 getDayCoins()
{
	Result ret=0;

	ret = archive_readfile(GameCoin_Extdata, "/gamecoin.dat", filebuffer, 0x14);
	if(ret!=0) {
		printf("failed to read file: 0x%08x\n", (unsigned int)ret);
		gfxFlushBuffers();
		gfxSwapBuffers();
		return 0;
	}

	u8 lowByte = filebuffer[0x6];
	u8 highByte = filebuffer[0x7];

	u16 combined = highByte;
	combined <<= 8;
	combined = combined | lowByte;
	return combined;
}

Result changeDayCoins(s16 amt)
{
	s16 cnt = getDayCoins();
	s16 final = cnt + amt;

	if (final < 0 || final > 300) {
		if (amt > 0) {
			final = 300;
		}
		else {
			final = 0;
		}
	}
	return setDayCoins(final);
}

/* Total Step Count - 0x08 0x04 */
// Result setSteps(u16 value);

// Result changeSteps(int amt);

u32 getSteps()
{
	Result ret = 0;

	ret = archive_readfile(GameCoin_Extdata, "/gamecoin.dat", filebuffer, 0x14);
	if (ret != 0) {
		printf("failed to read file: 0x%08x\n", (unsigned int)ret);
		gfxFlushBuffers();
		gfxSwapBuffers();
		return 0;
	}

	u32 steps = 0;
	steps |= filebuffer[0xb];
	steps <<= 8;
	steps |= filebuffer[0xa];
	steps <<= 8;
	steps |= filebuffer[0x9];
	steps <<= 8;
	steps |= filebuffer[0x8];
	return steps;
}

/* Daily Step Count - 0x0C 0x04 */

// Result setDaySteps(u16 value);

// Result changeDaySteps(int amt);

u32 getDaySteps()
{
	Result ret = 0;

	ret = archive_readfile(GameCoin_Extdata, "/gamecoin.dat", filebuffer, 0x14);
	if (ret != 0) {
		printf("failed to read file: 0x%08x\n", (unsigned int)ret);
		gfxFlushBuffers();
		gfxSwapBuffers();
		return 0;
	}

	u32 steps = 0;
	steps |= filebuffer[0xf];
	steps <<= 8;
	steps |= filebuffer[0xe];
	steps <<= 8;
	steps |= filebuffer[0xd];
	steps <<= 8;
	steps |= filebuffer[0xc];
	return steps;
}

/* Date - 0x10 0x04 */
u8 getDay()
{
	Result ret = 0;

	ret = archive_readfile(GameCoin_Extdata, "/gamecoin.dat", filebuffer, 0x14);
	if (ret != 0) {
		printf("failed to read file: 0x%08x\n", (unsigned int)ret);
		gfxFlushBuffers();
		gfxSwapBuffers();
		return 0;
	}

	u8 day = 0;
	day = filebuffer[0x13];
	return day;
}

u8 getMonth()
{
	Result ret = 0;

	ret = archive_readfile(GameCoin_Extdata, "/gamecoin.dat", filebuffer, 0x14);
	if (ret != 0) {
		printf("failed to read file: 0x%08x\n", (unsigned int)ret);
		gfxFlushBuffers();
		gfxSwapBuffers();
		return 0;
	}

	u8 month = 0;
	month = filebuffer[0x12];
	return month;
}

u16 getYear()
{
	Result ret = 0;

	ret = archive_readfile(GameCoin_Extdata, "/gamecoin.dat", filebuffer, 0x14);
	if (ret != 0) {
		printf("failed to read file: 0x%08x\n", (unsigned int)ret);
		gfxFlushBuffers();
		gfxSwapBuffers();
		return 0;
	}

	u16 year = 0;
	year |= filebuffer[0x11];
	year <<=8;
	year |= filebuffer[0x10];
	return year;
}
