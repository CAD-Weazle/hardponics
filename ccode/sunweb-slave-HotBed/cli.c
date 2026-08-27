// file    : cli.c
// author  : rb
// purpose : command line interface 
// date    : 081104
// last    : 191217
//
// note    : updated version, doesn't need 'type.h' & 'types.h' anymore
//         : added 'rb_atoh'

#include "includes.h"
#include <ctype.h>
#include <strings.h>

// globals
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
  if( line != NULL )
  {
    while( *line == ' ' )
      line++;
  }

  return( line );
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
  if ((c == '\r') || (c == '\n'))   // check for enter to end string
  {
    printf2 ("\n");                    // print newline

    buf[buf_len] = '\0';               // /0 terminate buffer
    buf_len      = 0;                  // reset pointer
    
    argc = split_line (buf, argv);     // parse command
    parse_line (argc, argv);          

    prompt ();                         // display new prompt
  }
  else if ((c == '\b') || (c == 0x7f)) // check for backspace and delete
  {
    if (buf_len > 0)
      buf_len--;
  }
  else if (isprint(c) && (buf_len < INBUF_LEN))
    buf[buf_len++] = c;
}

// parse command line
void parse_line (int argc, char **argv)
{
  if (argc)
  {
    // *** SunWeb commands
    // get SunWeb Serial Bus ID
    if (!strcmp (argv[0], "get_id"))
    {
      printf2 ("SunWeb ID: 0x%04x\n", flash_id_read ());
    }

    // set SunWeb Serial Bus ID
    if (!strcmp (argv[0], "set_id"))
    {
      uint16_t dat = (uint16_t)(rb_atoh (argv[1]));
      flash_id_write (dat);

      printf2 ("SunWeb ID: 0x%04x\n", flash_id_read ());
    }

    // *** PID temperature control commands
    // get HotBed temperature
    if (!strcmp (argv[0], "pid_get"))
    {
      pid_dump_temp ();
    }

    // set HotBed temperature ([100*oC])
    if (!strcmp (argv[0], "pid_set"))
    {
      if (argc == 2)
      {
        uint16_t target = rb_atoi (argv[1]);
        pid_set_target (target);
      }
    }

    // start HotBed PID temperature control
    if (!strcmp (argv[0], "pid_start"))
    {
      pid_start ();
    }

    // stop HotBed PID temperature control
    if (!strcmp (argv[0], "pid_stop"))
    {
      pid_stop ();
    }

    // start logging temperature
    if (!strcmp (argv[0], "pid_log"))
    {
      pid_start_log ();
    }

    // *** Pt100 parameter commands
    // dump Pt100 error parameters
    if (!strcmp (argv[0], "pt_dump"))
    {
      printf2 ("from FLASH:\n");
      flash_dump_rtderr ();
      printf2 ("from core :\n");
      pt_dump_error ();
    }

    // set Pt100 error parameter
    if (!strcmp (argv[0], "pt_err"))
    {
      if (argc == 3)
      {
        uint8_t  num = rb_atoi (argv[1]);
        uint16_t err = rb_atoi (argv[2]);
        pt_set_error (num, err);
      }

      flash_dump_rtderr ();
    }

    // log Pt100 temperatures
    if (!strcmp (argv[0], "pt_log"))
    {
      rtd_start_log ();
    }

    // *** PWM commands   
    // set green bottom LEDs light output
    if (!strcmp (argv[0], "pwm_leds"))
    {
      if (argc == 2)
      {
        uint8_t pwm = rb_atoi (argv[1]);
        pwm_set_leds (pwm);
      }
      else
        pwm_dump_leds (1);
    }

    // set heater output
    if (!strcmp (argv[0], "pwm_heater"))
    {
      if (argc == 2)
      {
        uint8_t pwm = rb_atoi (argv[1]);
        pwm_set_heater (pwm);
      }
      else
        pwm_dump_heater (1);
    }

    // set fan speed
    if (!strcmp (argv[0], "pwm_fan"))
    {
      if (argc == 2)
      {
        uint8_t pwm = rb_atoi (argv[1]);
        pwm_set_fan (pwm);
      }
      else
        pwm_dump_fan (1);
    }

    // dump PWM state
    if (!strcmp (argv[0], "pwm_state"))
    {
      pwm_state ();
    }

    // *** RTC commands
    // start RTC wallclock
    if (!strcmp (argv[0], "clock"))
    {
      rtc_wallclock_start ();
    }

    // start RTC wallclock from 00:00:00 -- for RTC calibration
    if (!strcmp (argv[0], "rtc"))
    {
      rtc_set_time (0, 0, 0);
      rtc_wallclock_start ();
    }

    // set RTC wall clock time
    if (!strcmp (argv[0], "set_time"))
    {
       uint8_t hrs = (uint8_t)(rb_atoi (argv[1]));
       uint8_t min = (uint8_t)(rb_atoi (argv[2]));
       uint8_t sec = (uint8_t)(rb_atoi (argv[3]));

       rtc_set_time (hrs, min, sec);
       usleep (500);                   // relax a bit
       rtc_update_time ();
       rtc_dump_time ();
    }

    // get RTC wallclock time
    if (!strcmp (argv[0], "get_time"))
    {
       rtc_update_time ();
       rtc_dump_time ();
    }

    // *** others <> todo
    if (!strcmp (argv[0], "show"))
    {
      if (!strcmp (argv[1], "on"))
        pwm_show_start ();

      if (!strcmp (argv[1], "off"))
        pwm_show_stop ();
    }

    // stop all
    if (!strcmp (argv[0], "q"))
    {
      rtc_wallclock_stop ();
      rtd_stop_log ();
      pid_stop_log ();
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
  printf2 ("\n- SunWeb Slave - HoTBed - " VERSION " -\n");
}

// dump commands
void help (void)
{
/*
  printf2 ("-- SunWeb commands:\n");
  printf2 ("get_id\n");
  printf2 ("set_id <num>\n");
  printf2 ("\n");

  printf2 ("-- HotBed PID commands:\n");
  printf2 ("pid_get\n");
  printf2 ("pid_set <temp>\n");
  printf2 ("pid_start\n");
  printf2 ("pid_stop\n");
  printf2 ("pid_log\n");
  printf2 ("\n");

  printf2 ("-- RTD commands:\n");
  printf2 ("rtd_dump <num> <err>\n");
  printf2 ("rtd_err <num> <err>\n");
  printf2 ("rtd_log\n");
  printf2 ("\n");

  printf2 ("-- PWM commands:\n");
  printf2 ("pwm_leds <num>\n");
  printf2 ("pwm_heater <num>\n");
  printf2 ("pwm_fan <num>\n");
  printf2 ("pwm_state\n");
  printf2 ("\n");

  printf2 ("-- RTC commands:\n");
  printf2 ("clock\n");
  printf2 ("rtc\n");
  printf2 ("set_time <hrs min sec>\n");
  printf2 ("get_time\n");
  printf2 ("get_temp\n");
  printf2 ("\n");

  printf2 ("-- others\n");
  printf2 ("show <on|off>\n");
  printf2 ("q\n");
  printf2 ("version\n");
  printf2 ("help\n");
*/
}

