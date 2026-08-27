// file    : oled.c
// author  : rb
// purpose : SSD1351 WaveShare OLED driver
// date    : 210904
// last    : 230126
//

#include "includes.h"
#include "./fonts/fixedsys_8x14.h"
//#include "./fonts/ubuntumono_8x14.h"

// globals
int oled_active = 1;                   // OLED display on;off flag

// set up OLED display
void init_oled (void) 
{
  oled_wr_cmd (OLED_CMD_COMMANDLOCK);   // command lock
  oled_wr_dat (0x12);
  oled_wr_cmd (OLED_CMD_COMMANDLOCK);   // command lock
  oled_wr_dat (0xb1);

  oled_wr_cmd (OLED_CMD_DISPLAYOFF);    // display off
  oled_wr_cmd (OLED_CMD_DISPLAYALLOFF); // normal display mode

  oled_wr_cmd (OLED_CMD_SETCOLUMN);
  oled_wr_dat (0);   
  oled_wr_dat (OLED_WIDTH-1);

  oled_wr_cmd (OLED_CMD_SETROW);	
  oled_wr_dat (0);  
  oled_wr_dat (OLED_HEIGHT-1);    

  oled_wr_cmd (OLED_CMD_CLOCKDIV);
  oled_wr_dat (0xf1);

  oled_wr_cmd (OLED_CMD_MUXRATIO);	
  oled_wr_dat (0x7f);

  // set re-map & data format, bits:
  //  7,6 color depth        (0b01 = 64K)
  //  5   odd/even split COM (0: disable / 1: enable)
  //  4   scan direction     (0: top-down / 1: bottom-up)
  //  3   reserved
  //  2   color remap        (0: A->B->C / 1: C->B->A)
  //  1   column remap       (0: 0..127 / 1: 127..0)
  //  0   address increment  (0: horizontal / 1: vertical)
  oled_wr_cmd (OLED_CMD_SETREMAP);
  oled_wr_dat (0b01110100); 

  // set display start line
  oled_wr_cmd (OLED_CMD_STARTLINE);
  oled_wr_dat (0x00);

  // set display offset
  oled_wr_cmd (OLED_CMD_DISPLAYOFFSET);
  oled_wr_dat (0x00);

  // enable internal regulator & select SPI interface
  oled_wr_cmd (OLED_CMD_FUNCTIONSELECT);	
  oled_wr_dat (0x01);	

  oled_wr_cmd (OLED_CMD_SETVSL);	
  oled_wr_dat (0xa0);	  
  oled_wr_dat (0xb5);  
  oled_wr_dat (0x55);    

  oled_wr_cmd (OLED_CMD_CONTRASTABC);	
  oled_wr_dat (0xc8);	
  oled_wr_dat (0x80);
  oled_wr_dat (0xc0);

  oled_wr_cmd (OLED_CMD_CONTRASTMASTER);	
  oled_wr_dat (0x0f);

  oled_wr_cmd (OLED_CMD_PRECHARGE);	
  oled_wr_dat (0x32);

  oled_wr_cmd (OLED_CMD_DISPLAYENHANCE);	
  oled_wr_dat (0xa4);
  oled_wr_dat (0x00);
  oled_wr_dat (0x00);

  oled_wr_cmd (OLED_CMD_PRECHARGELEVEL);	
  oled_wr_dat (0x17);

  oled_wr_cmd (OLED_CMD_PRECHARGE2);
  oled_wr_dat (0x01);

  oled_wr_cmd (OLED_CMD_VCOMH);
  oled_wr_dat (0x05);

  oled_wr_cmd (OLED_CMD_NORMALDISPLAY);

  // set brightness
  oled_brightness (0x0f);
 
  // wipe screen
  oled_clrscr (OLED_COL_BLACK);

  // turn on display
  oled_wr_cmd (OLED_CMD_DISPLAYON);

  // draw status screen background
  oled_display_background ();
}

// write command to OLED display
void oled_wr_cmd (uint8_t dat)
{
  // assert DATA/CMD line
  oled_cmd ();

  // write command byte
  oled_select ();
  spi1_write_byte (dat);
  oled_deselect ();

  // relax a bit
  usleep (1);
}

// write data  to OLED display
void oled_wr_dat (uint8_t dat)
{
  // assert DATA/CMD line
  oled_dat ();

  // write data byte
  oled_select ();
  spi1_write_byte (dat);
  oled_deselect ();
  
  // relax a bit
  usleep (1);
}

// write 16-bit color pixel to SRAM
void oled_wr_col (uint16_t col)
{
  // assert DATA/DATA line
  oled_dat ();

  // write data 
  oled_select ();
  spi1_write_byte ((uint8_t)(col >> 8));
  spi1_write_byte ((uint8_t)(col & 0x00ff));
  oled_deselect ();
  
  // relax a bit
  usleep (1);
}

// control OLED on timing 
#define OLED_CONTROL_START  0
#define OLED_CONTROL_ACTIVE 1
#define OLED_CONTROL_END    2

#define OLED_TIME_ON       10  // maximum on time OLED [s]

void oled_update (void)
{
  static int state = OLED_CONTROL_START;
  static int tcnt = 0;

  switch (state)
  {
    case OLED_CONTROL_START:
    {
      // OLED is turned on, start timer
      if (oled_active)
        state = OLED_CONTROL_ACTIVE;    

      break;
    }

    case OLED_CONTROL_ACTIVE:
    {
      // track on time
      if (tcnt++ >= OLED_TIME_ON)
        state = OLED_CONTROL_END;    

      break;
    }

    case OLED_CONTROL_END:
    {
      // turn of OLED & update state 
      oled_power_down ();

      oled_active = 0;
      tcnt = 0;

      state = OLED_CONTROL_START;    

      break;
    }

    default:
      break;
 }
}

// power down display module
void oled_power_down (void)
{
  // enter sleep mode
  oled_wr_cmd (OLED_CMD_DISPLAYOFF);

  // disable internal regulator
  oled_wr_cmd (OLED_CMD_FUNCTIONSELECT);	
  oled_wr_dat (0x00);	

  // reset flag
  oled_active = 0;
}

// power up display module
void oled_power_up (void)
{
  // enable internal regulator & select SPI interface
  oled_wr_cmd (OLED_CMD_FUNCTIONSELECT);	
  oled_wr_dat (0x01);	

  // leave sleep mode
  oled_wr_cmd (OLED_CMD_DISPLAYON);

  // set flag
  oled_active = 1;
}

// set display brightness
void oled_brightness (uint8_t dat) 
{
  // sanity check
  if (dat > 0x0f) 
    dat = 0x0f;

  oled_wr_cmd (OLED_CMD_CONTRASTMASTER);
  oled_wr_dat (dat);
}

// clear screen with color
void oled_clrscr (uint16_t col)  
{
  // set start column & width
  oled_wr_cmd (OLED_CMD_SETCOLUMN);
  oled_wr_dat (0);
  oled_wr_dat (OLED_WIDTH-1);

  // set start row & height
  oled_wr_cmd (OLED_CMD_SETROW);
  oled_wr_dat (0);
  oled_wr_dat (OLED_HEIGHT-1);

  // write to SRAM
  oled_wr_cmd (OLED_CMD_WRITERAM);

  // fill SRAM
  for (int i = 0; i < OLED_WIDTH; i++)  
    for (int j = 0; j < OLED_HEIGHT; j++)  
      oled_wr_col (col);
}

// draw color filled rectangle 
void oled_rect (uint8_t x, uint8_t w, uint8_t y, uint8_t h, uint16_t col)  
{
  // set start column & width
  oled_wr_cmd (OLED_CMD_SETCOLUMN);
  oled_wr_dat (x);
  oled_wr_dat (x+w-1);

  // set start row & height
  oled_wr_cmd (OLED_CMD_SETROW);
  oled_wr_dat (y);
  oled_wr_dat (y+h-1);

  // write to SRAM
  oled_wr_cmd (OLED_CMD_WRITERAM);

  // fill SRAM
  for (int i = 0; i < w*h; i++)  
  {
    oled_wr_col (col);
  }
}

// draw ASCII character - only works with <= 8 pixel wide characters
void oled_putc (uint8_t x, uint8_t w, uint8_t y, uint8_t h, uint16_t fg, uint16_t bg, uint8_t *font)  
{
  // sanity check
  if (w > 8)
    return;

  // set start column & character width
  oled_wr_cmd (OLED_CMD_SETCOLUMN);
  oled_wr_dat (x);
  oled_wr_dat (x+w-1);

  // set start row & character height
  oled_wr_cmd (OLED_CMD_SETROW);
  oled_wr_dat (y);
  oled_wr_dat (y+h-1);

  // write to SRAM
  oled_wr_cmd (OLED_CMD_WRITERAM);

  for (int i = 0; i < h; i++)
  {
    for (int bit = 0; bit < w; bit++)
    {
      if (font[i] & (1 << bit))
        oled_wr_col (fg);  // pixel in foreground color
      else
        oled_wr_col (bg);  // & rest in background color
    }
  }
}

// draw '\0' terminated string
void oled_puts (uint8_t x, uint8_t w, uint8_t y, uint8_t h, uint16_t fg, uint16_t bg, char *str)
{
  char ch;
  uint8_t cnt = 0;

  while ((ch = str[cnt++]) != '\0')
    oled_putc (x + cnt*w, w, y, h, fg, bg, &fixedsys_8x14[ch-32][0]);
}  

// dump font - dev only
void oled_dump_font (void)
{
  for (int y = 0; y < 6; y++)
    for (int x = 0; x < 16; x++)
      oled_putc (x*8, 8, y*16, 14, OLED_COL_OLDLACE, OLED_COL_BLACK, &fixedsys_8x14[x + (y*16)][0]);
    //oled_putc (x*8, 8, y*16, 14, OLED_COL_OLDLACE, &ubuntu_mono_8x14[x + (y*16)][0]);
}


// -- BATMON data display part --

#define X_ORG            0
#define X_OFS            0
#define X_WDT            8
#define X_LEN          127
#define X_DEC    (13*X_WDT) // location u/m sign
#define X_VAL           30  // offset for values

#define Y_ORG            0
#define Y_OFFS          10
#define Y_HDT           14
#define Y_BAR     (Y_HDT+2)
#define Y_INC     (Y_HDT+2)

#define STR_LEN          9 

char str[20];

// display background
void oled_display_background (void)
{
  sprintf (str, "Vbat         V");
  oled_rect (X_ORG, X_LEN, Y_ORG + Y_OFFS-1, Y_BAR, OLED_COL_OLDLACE);
  oled_puts (X_ORG, X_WDT, Y_ORG + Y_OFFS,   Y_HDT, OLED_COL_BLACK, OLED_COL_OLDLACE, str);

  sprintf (str, "Ibat        mA");
  oled_rect (X_ORG, X_LEN, Y_ORG + Y_INC + Y_OFFS-1, Y_BAR, OLED_COL_OLDLACE);
  oled_puts (X_ORG, X_WDT, Y_ORG + Y_INC + Y_OFFS,   Y_HDT, OLED_COL_BLACK, OLED_COL_OLDLACE, str);

  sprintf (str, "Vsol         V");
  oled_rect (X_ORG, X_LEN, Y_ORG + 2.5*Y_INC + Y_OFFS-1, Y_BAR, OLED_COL_GREEN);
  oled_puts (X_ORG, X_WDT, Y_ORG + 2.5*Y_INC + Y_OFFS,   Y_HDT, OLED_COL_BLACK, OLED_COL_GREEN, str);

  sprintf (str, "PAR          ");
  oled_rect (X_ORG, X_LEN, Y_ORG + 4*Y_INC + Y_OFFS-1, Y_BAR, OLED_COL_YELLOW);
  oled_puts (X_ORG, X_WDT, Y_ORG + 4*Y_INC + Y_OFFS,   Y_HDT, OLED_COL_BLACK, OLED_COL_YELLOW, str);

  sprintf (str, "    oC     %%RH");
  oled_rect (X_ORG, X_LEN, Y_ORG + 5.5*Y_INC + Y_OFFS-1, Y_BAR, OLED_COL_RED);
  oled_puts (X_ORG, X_WDT, Y_ORG + 5.5*Y_INC + Y_OFFS,   Y_HDT, OLED_COL_BLACK, OLED_COL_RED, str);
}

// display data
void oled_display_data (float vbat, float ibat, float vsol, float par, float temp, float rh)
{
  static int display_state = 1;
  
  static float vbat_old;
  static float ibat_old;
  static float vsol_old;
  static float par_old;
  static float temp_old;
  static float rh_old;

  char res[STR_LEN] = {0};

  // handle OLED on/off control touch button
  if (touch_oled_pressed ())
  {
     if (display_state == 1)
     {
       oled_power_down ();
       display_state = 0;
     }
     else
     {
       oled_power_up ();
       display_state = 1;
     }
  }  

  // display battery voltage
  if (vbat_old != vbat)
  {
    sprintf (str, "%2.2f", vbat);
    oled_align (str, res);    
  
    if (bat_empty ())
      oled_puts (X_VAL, X_WDT, Y_ORG + Y_OFFS, Y_HDT, OLED_COL_RED, OLED_COL_OLDLACE, res);
    else
      oled_puts (X_VAL, X_WDT, Y_ORG + Y_OFFS, Y_HDT, OLED_COL_BLACK, OLED_COL_OLDLACE, res);
  }

  // display battery current
  if (ibat_old != ibat)
  {    
    sprintf (str, "%2.2f", ibat);
    oled_align (str, res);    
  
    oled_puts (X_VAL, X_WDT, Y_ORG + Y_INC + Y_OFFS, Y_HDT, OLED_COL_BLACK, OLED_COL_OLDLACE, res);
  }

  // display solar panel voltage
  if (vsol_old != vsol)
  {
    if (vsol < 6.0)
    {
      // solar voltage too low
      strcpy (str, "* LOW *");
      oled_align (str, res);
      oled_puts (X_VAL, X_WDT, Y_ORG + 2.5*Y_INC + Y_OFFS, Y_HDT, OLED_COL_BLACK, OLED_COL_GREEN, res);
    }
    else
    {
      // solar volage in normal range
      sprintf (str, "%2.2f", vsol);
      oled_align (str, res);
      oled_puts (X_VAL, X_WDT, Y_ORG + 2.5*Y_INC + Y_OFFS, Y_HDT, OLED_COL_BLACK, OLED_COL_GREEN, res);
    }

  }

  // display PAR
  if (par_old != par)
  {
  //sprintf (str, "%1.0f", par);
    sprintf (str, "%d", (uint16_t)par);
    oled_align (str, res);    

    oled_puts (X_VAL, X_WDT, Y_ORG + 4.0*Y_INC + Y_OFFS, Y_HDT, OLED_COL_BLACK, OLED_COL_YELLOW, res);
  }

  // display temperature
  if (temp_old != temp)
  {  
    // sanity check
    if (temp > 99.9)
      temp = 99.9;

    sprintf (str, "%2.1f", temp);
    oled_puts (0, X_WDT, Y_ORG + 5.5*Y_INC + Y_OFFS, Y_HDT, OLED_COL_BLACK, OLED_COL_RED, str);
  }
  
  // display humidity
  if (rh_old != rh)
  {  
    // sanity check
    if (rh > 99.0)
      rh = 99.0;

    sprintf (str, "%d", (uint16_t)rh);
    oled_puts (68, X_WDT, Y_ORG + 5.5*Y_INC + Y_OFFS, Y_HDT, OLED_COL_BLACK, OLED_COL_RED, str);
  }

  // safe state
  vbat_old = vbat;
  ibat_old = ibat;
  vsol_old = vsol;
  par_old  = par;
  temp_old = temp;
  rh_old   = rh;
}

// right align number string
void oled_align (char *str, char *res)
{
  uint8_t pos = 0;

  // find end of string location
  for (int i = 0; i < STR_LEN; i++)
  {
    if (str[i] == '\0')
    {
      pos = i;  
      break;
    }
  } 
  
  // fill buffer with spaces & add '\0' at last postion
  memset (res, ' ', STR_LEN-1);
  res[STR_LEN-1] = '\0';

  // shift digits
  for (int i = 0; i < STR_LEN-1; i++)
    res[i + (STR_LEN-1 - pos)] = str[i];
}

// OLED control lines
void oled_select (void)
{
  gpio_clr (GPIOA, OLED_CS_L);
}

void oled_deselect (void)
{
  gpio_set (GPIOA, OLED_CS_L);
}

void oled_cmd (void)
{
  gpio_clr (GPIOA, OLED_CD);
}

void oled_dat (void)
{
  gpio_set (GPIOA, OLED_CD);
}


//  // display Idut bargraph
//  int bar_len = ((int)i_adc * X_LEN) / 4096;  // scale ADC range back to x-size OLED screen
//  
//  oled_rect (X_ORG, X_LEN,   Y_ORG + 3.4*Y_INC + Y_OFFS, 1, OLED_COL_BLACK);    // undraw old bargraph
//  oled_rect (X_ORG, bar_len, Y_ORG + 3.4*Y_INC + Y_OFFS, 1, OLED_COL_OLDLACE);  // draw bargraph

