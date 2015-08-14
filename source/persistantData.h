/* usage:
 *
 * Init with dataInit() and the current date from gamecoin.dat
 * - checks data file to see if it's current/valid
 * - returns 0 if file was out of date 
 *     (coins field will be 0 and date set to today)
 * - returns 1 if file was current (file left alone)
 * - returns 2 if file was messed with externally/is missing
 *     (coins field will be 0 and date set to today)
 * - returns -1 on error
 *
 * get and set coins with getter/setter
 *
 * close files with dataExit()
 */
Result mydataInit(u8 day, u8 month, u16 year);
void mydataExit();

u16 getStoredCoins();
void setStoredCoins(u16 value);

// u32 getStoredSteps();
// Result setStoredSteps(u32 value);
