Result extdataInit();
void extdataExit();

Result setCoins(u16 value);
Result changeCoins(s16 amt);
u16 getCoins();

Result setDayCoins(u16 value);
Result changeDayCoins(s16 amt);
u16 getDayCoins();

/* not providing setters for these for now. */
// Result setSteps(u16 value);
// Result changeSteps(int amt);
u32 getSteps();

// Result setDaySteps(u16 value);
// Result changeDaySteps(int amt);
u32 getDaySteps();

// Result getDate();
u8 getDay();
u8 getMonth();
u16 getYear();
