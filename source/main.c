#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
/* #include <3ds.h> */
#include </opt/devkitpro/libctru/include/3ds.h>
#include "extdata.h"
#include "persistantData.h"

bool fixCoins(u16 change, u16 amt);

int main()
{
	gfxInitDefault();
	consoleInit(GFX_BOTTOM, NULL);
	extdataInit();

	typedef struct {
		u8 day;
		u8 month;
		u16 year;
	}date;
	date today = {getDay(), getMonth(), getYear()};

	u32 todaysteps = getDaySteps();
	u16 todaycoins = getDayCoins();
	/* hold select on startup for debug info */
	hidScanInput();
	u32 kHeld = hidKeysHeld();
	if (kHeld & KEY_SELECT) {
		printf("\n\n\n");
		printf("    Step count: 0x%08x\n\n    Day's steps: 0x%08x\n\n\n", (unsigned int)getSteps(), (unsigned int)todaysteps);
		printf("    Coin count: %d\n\n    Day's Coins: %d\n\n", (unsigned int)getCoins(), (unsigned int)todaycoins);
		printf("    D M Y: 0x%02x 0x%02x 0x%04x\n\n", today.day, today.month, today.year);
		/* printf("\n\n    Press Start to exit.\n"); */
	}


	/* TEST DATA */
	today.day = 14; today.month=8; today.year=2015;
	todaysteps = 10;
	todaycoins = 0;
	/* REMOVE FOR ACTUAL 3DS */

	Result res = mydataInit(today.day, today.month, today.year);
	printf("mydataInit() returned %d\n", (int)res);
	if (res == 1) {
		u16 storedcoins = getStoredCoins();
		printf("today coins: %d\nstored coins: %d\n", (unsigned int)todaycoins, (unsigned int)storedcoins);
		todaycoins = (storedcoins > todaycoins) ? storedcoins : todaycoins;
	}
	consoleInit(GFX_TOP, GFX_LEFT);
	unsigned int correctcoins = (todaysteps / 100);
	bool slighted;
	if (correctcoins > todaycoins) {
		slighted = true;
		printf("\x1b[0m\n\n You've taken \x1b[2m%d\x1b[0m steps but only earned\n \x1b[31m%d\x1b[0m coins for today! How would you like to\n have another \x1b[32m%d\x1b[0m coins to make up for it?\n\n [A] Sure!\n\n [START] It's okay, just take me back home.\n", (unsigned int)todaysteps, todaycoins, correctcoins - todaycoins);
	} else {
		slighted = false;
		printf("\x1b[0m\n\n You've taken \x1b[2m%d\x1b[0m steps and earned\n \x1b[32m%d\x1b[0m coins for today.\n\n Come back later.\n\n Press [START] to exit.\n", (unsigned int)todaysteps, todaycoins);
	}

	// Main loop
	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();

		u32 kDown = hidKeysDown();

		if (kDown & KEY_START) break; // break in order to return to hbmenu

		if (slighted && kDown & KEY_A) {
			printf("\n Applying...");
			if (fixCoins(correctcoins - todaycoins, correctcoins)) {
				printf("...Applied!\n\n Press [START] to exit.");
				slighted = false;
			} else {
				printf("...Error!\n\n Press [START] to exit.");
			}
		}
	}

	gfxExit();
	extdataExit();
	mydataExit();
	return 0;
}

/*
 * Increase play coins by amt
 * Set data file's play coin count to amt
 */
bool fixCoins(u16 change, u16 amt) {
	Result res;
	res = changeCoins((s16)change);
	if (res != 0) return false;

	setStoredCoins(amt);
	/* if (res != 0) return false; */

	return true;
}
