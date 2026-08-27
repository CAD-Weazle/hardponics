// file    : cli.c
// author  : rb
// purpose : command line interface 
// date    : 081104
// last    : 190906
//
// note    : updated version, doesn't need 'type.h' & 'types.h' anymore
//         : added 'rb_atoh'

#include "includes.h"
#include <ctype.h>
#include <strings.h>

// -- globals
uint8_t do_rtc = 0;    // wallclock flag

uint8_t buf_len;
char    buf[INBUF_LEN];

int   argc;
char *argv[10];

// display terminal prompt
void prompt (void)
{ 
  // normal prompt
  printf2 (">");
}

// display terminal prompt with extra '\n'
void first_prompt (void)
{
  printf2 ("\n>");
}

// convert ASCII string to integer
int rb_atoi (char *s)
{
  int i = 0, n, sign = 1;
  
  // check for negative number
  if (s[0] == '-')
  {
    sign = -1;
    i++;
  }

  // look for numbers & calculate integer
  for (n = 0; s[i] >= '0' && s[i] <= '9'; i++)
    n = 10 * n + s[i] - '0';
  
  return (sign * n);
}

// convert ASCII string to long
long rb_atol (char *s)
{
  int i = 0, sign = 1;
  long n;
  
  // check for negative number
  if (s[0] == '-')
  {
    sign = -1;
    i++;
  }
  
  // look for numbers & calculate long
  for( n = 0; s[i] >= '0' && s[i] <= '9'; i++ )
    n = 10 * n + s[i] - '0';
  
  return (sign * n);
}

// convert ASCII string to hex int
uint32_t rb_atoh (char *s)
{
  int i = 0;
  int digit, lcase;
  uint32_t n;
  
  // look for numbers / lower case letters & calculate integer
  for (n = 0; ((digit = (s[i] >= '0' && s[i] <= '9')) || 
               (lcase = (s[i] >= 'a' && s[i] <= 'f')) ||
                        (s[i] >= 'A' && s[i] <= 'F')); i++)
  {
    n *= 16;

    if (digit)
      n += s[i] - '0';
    else if (lcase)
      n += 10 + (s[i] - 'a');
    else      
      n += 10 + (s[i] - 'A');
  }

  return (n);
}

// skip whitespace
char *skipws (char *line)
{
  if (line != NULL)
  {
    while (*line == ' ')
      line++;
  }

  return (line);
}

// split up command line in words seperated by whitespaces
int split_line (char *line, char **words)
{
  int n = 0;
  char *sep = line;
  
  while ((line = skipws(sep)) != NULL && *line)
  {
    sep = strchr (line, ' ');

    if (sep)
      *sep++ = 0;

    words[n++] = line;
  }

  return (n);
}

// poll command line interface USART2
void poll_cli (void)
{
  int c = uart2_getc ();

  // test for Rx buffer empty
  if (c < 0)
    return;

  // add byte to buffer & parse
  if (( c == '\r' ) | ( c == '\n' ))   // check for enter to end string
  {
    printf2 ("\n");                    // print newline

    buf[buf_len] = '\0';               // /0 terminate buffer
    buf_len      = 0;                  // reset pointer
    
    argc = split_line (buf, argv);     // parse command
    parse_line (argc, argv);          

    prompt ();                         // display new prompt
  }
  else if (c == '\b' || c == 0x7f)     // check for backspace and delete
  {
    if( buf_len > 0 )
      buf_len--;
  }
  else if (isprint(c) && buf_len < INBUF_LEN)
    buf[buf_len++] = c;
}

// parse command line
void parse_line (int argc, char **argv)
{
  if (argc)
  {
    // start Simsun day light cycle
    if (!strcmp (argv[0], "ss_start"))
    {
      simsun_start ();
    }

    // stop Simsun day light cycle
    if (!strcmp (argv[0], "ss_stop"))
    {
      simsun_stop ();
    }

    // get SimSun state
    if (!strcmp (argv[0], "ss_state"))
    {
      simsun_dump_state ();
    }

    // set SimSun dawn (hh mm ss)
    if (!strcmp (argv[0], "set_dawn"))
    {
      if (argc == 4)
      {
        uint8_t hrs = (uint8_t)(rb_atoi (argv[1]));
        uint8_t min = (uint8_t)(rb_atoi (argv[2]));
        uint8_t sec = (uint8_t)(rb_atoi (argv[3]));

        simsun_set_dawn (hrs, min, sec);
        simsun_dump_state ();
      }
    }

    // set SimSun dusk (hh mm ss)
    if (!strcmp (argv[0], "set_dusk"))
    {
      if (argc == 4)
      {
        uint8_t hrs = (uint8_t)(rb_atoi (argv[1]));
        uint8_t min = (uint8_t)(rb_atoi (argv[2]));
        uint8_t sec = (uint8_t)(rb_atoi (argv[3]));

        simsun_set_dusk (hrs, min, sec);
        simsun_dump_state ();
      }
    }

    // set Deep Red LEDs light output
    if (!strcmp (argv[0], "set_dred"))
    {
      if (argc == 2)
      {
        uint8_t dat = (uint8_t)rb_atoi (argv[1]);

        leds_dred (dat);
      }
    }

    // set Royal Blue LEDs light output
    if (!strcmp (argv[0], "set_rblu"))
    {
      if (argc == 2)
      {
        uint8_t dat = (uint8_t)rb_atoi (argv[1]);

        leds_rblu (dat);
      }
    }

    // set Far Red LEDs light output
    if (!strcmp (argv[0], "set_fred"))
    {
      if (argc == 2)
      {
        uint8_t dat = (uint8_t)rb_atoi (argv[1]);

        leds_fred (dat);
      }
    }

    // set all LEDs light output
    if (!strcmp (argv[0], "set_leds"))
    {
      if (argc == 2)
      {
        uint8_t dat = (uint8_t)rb_atoi (argv[1]);

        leds_dred (dat);
        leds_fred (dat);
        leds_rblu (dat);
      }
    }

    // set SimSun Deep Red maximum light intensity
    if (!strcmp (argv[0], "max_dred"))
    {
      if (argc == 2)
      {
        uint8_t dat = (uint8_t)(rb_atoi (argv[1]));

        if (dat <= 100)
          simsun_dred_max (dat);
      }
    }

    // set SimSun Royal Blue maximum light intensity
    if (!strcmp (argv[0], "max_rblu"))
    {
      if (argc == 2)
      {
        uint8_t dat = (uint8_t)(rb_atoi (argv[1]));

        if (dat <= 100)
          simsun_rblu_max (dat);
      }
    }

    // set RTC wall clock time
    if (!strcmp (argv[0], "set_rtc"))
    {
      if (argc == 4)
      {
        uint8_t hrs = (uint8_t)(rb_atoi (argv[1]));
        uint8_t min = (uint8_t)(rb_atoi (argv[2]));
        uint8_t sec = (uint8_t)(rb_atoi (argv[3]));

        rtc_set_time (hrs, min, sec);
        usleep (500);                   // relax a bit
        rtc_update_time ();
        rtc_dump_time ();
      }
    }

    // get RTC wallclock time
    if (!strcmp (argv[0], "get_rtc"))
    {
      rtc_update_time ();
      rtc_dump_time ();
    }

    // start RTC wallclock from 00:00:00 -- for RTC calibration
    if (!strcmp (argv[0], "sync_rtc"))
    {
      rtc_set_time (0, 0, 0);
      do_rtc = 1;
    }

    // start RTC wallclock logging
    if (!strcmp (argv[0], "log_rtc"))
    {
      rtc_log_start ();
    }

    // get temperature
    if (!strcmp (argv[0], "get_ntc"))
    {
      ntc_dump ();
    }

    // get/set NTC temperature offset error
    if (!strcmp (argv[0], "err_ntc"))
    {
      if (argc == 2)
      {
        int16_t err = (int16_t)(rb_atoi (argv[1]));

        ntc_set_error (err);
      }

      ntc_dump_error ();
    }

    // get SunWeb Serial Bus ID
    if (!strcmp (argv[0], "get_id"))
    {
      printf2 ("SunWeb ID: 0x%04x\n", flash_id_read ());
    }

    // set SunWeb Serial Bus ID
    if (!strcmp (argv[0], "set_id"))
    {
      if (argc == 2)
      {
        uint8_t badr = (uint8_t)(rb_atoh (argv[1]));
        flash_id_write (badr);

        printf2 ("SunWeb ID: 0x%04x\n", flash_id_read ());
      }
    }

    // stop logging 
    if (!strcmp (argv[0], "q"))
    {
      rtc_log_stop ();
    }

    // dump version information
    if (!strcmp (argv[0], "version"))
      version ();

    // dump help
    if (!strcmp (argv[0], "help"))
      help ();
  }   
} 
    
// dump version info
void version (void)
{
  printf2 ("\n- SunWeb Slave - SimSun - " VERSION " -\n");
}

// dump commands
void help (void)
{
  printf2 ("-- SimSun commands:\n");
  printf2 ("ss_start\n");
  printf2 ("ss_stop\n");
  printf2 ("ss_state\n");
  printf2 ("set_dawn <hh> <mm> <ss>\n");
  printf2 ("set_dusk <hh> <mm> <ss>\n");
  printf2 ("set_dred <num>\n");
  printf2 ("set_rblu <num>\n");
  printf2 ("set_fred <num>\n");
  printf2 ("set_leds <num>\n");
  printf2 ("max_dred <num>\n");
  printf2 ("max_rblu <num>\n");
  printf2 ("set_rtc  <hh> <mm> <ss>\n");
  printf2 ("get_rtc\n");
  printf2 ("sync_rtc\n");
  printf2 ("log_rtc\n");
  printf2 ("get_ntc\n");
  printf2 ("err_ntc [num]\n");
  printf2 ("get_id\n");
  printf2 ("set_id <badr>\n");
  printf2 ("q\n");
  printf2 ("version\n");
  printf2 ("help\n");
}
