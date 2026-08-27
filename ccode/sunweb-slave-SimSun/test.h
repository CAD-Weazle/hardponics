// -- SunWeb Commands
// --- probe commands

// --- LEDs commands

// --- misc. commands




// probe commands
#define SW_CMD_PROBE         0xa0      // probe SunWeb Devices
#define SW_CMD_ATTACH        0xa1      // attach SunWeb Device
#define SW_CMD_RELEASE       0xa2      // detach SunWeb Device

// commands that need broadcast
#define SW_CMD_SETBUSID      0xf0      // set SunWeb Device Bus ID
#define SW_CMD_SYNCCLOCK     0xff      // sync ARM RTC at midnight 00:00:00

// commands that needs addressing
#define SW_CMD_GETTIME       0xf2      // get Device time -- ARM RTC
#define SW_CMD_GETSTATE      0xf3      // get Device state
#define SW_CMD_SETTIME       0xf1      // set Device time -- ARM RTC 
#define SW_CMD_SETDAWN       0x10      // set dawn time (LEDs on)
#define SW_CMD_SETDUSK       0x11      // set dusk time (LEDs off)
#define SW_CMD_SETDRED       0x12      // set light intensity Deep Red LEDs
#define SW_CMD_SETRBLU       0x13      // set light intensity Royal Blue LEDs
#define SW_CMD_SETFRED       0x14      // set light intensity Far Red LEDs
#define SW_CMD_SETDREDMAX    0x15      // set maximum light intensity Deep Red LEDs
#define SW_CMD_SETRBLUMAX    0x16      // set maximum light intensity Royal Blue LEDs


#define SW_CMD_SIMSUNSTART   0x17      // start SimSun program
#define SW_CMD_SIMSUNSTOP    0x18      // stop SimSun program

#define SW_CMD_GETTEMP       0x19      // get temperature

