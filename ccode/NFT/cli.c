// file    : cli.c
// author  : rb
// purpose : command line interface 
// date    : 081104
// last    : 230221
//

#include "includes.h"
#include <ctype.h>
#include <strings.h>

#define RX1_SIZE           128
#define TX1_SIZE           128
#define INBUF_LEN     RX1_SIZE  // for old 'cli.c' code

// globals
 uint8_t pass_thru = 0;    // <> remove later 

uchar cli_buf_len;
char  cli_buf[INBUF_LEN];

uchar esp_buf_len;
char  esp_buf[INBUF_LEN];

int   cli_argc;
char *cli_argv[10];

int   esp_argc;
char *esp_argv[10];

void prompt (void)
{ 
  // normal prompt
  printf2 (">");
}

void first_prompt (void)
{
  printf2 ("\n>");
}

int rb_atoi (char *s)
{
  int i = 0, n, sign = 1;
  
  if( s[0] == '-' )
  {
    sign = -1;
    i++;
  }
  for( n = 0; s[i] >= '0' && s[i] <= '9'; i++ )
    n = 10 * n + s[i] - '0';
  
  return (sign * n);
}

long rb_atol (char *s)
{
  int i = 0, sign = 1;
  long n;
  
  if( s[0] == '-' )
  {
    sign = -1;
    i++;
  }
  
  for( n = 0; s[i] >= '0' && s[i] <= '9'; i++ )
    n = 10 * n + s[i] - '0';
  
  return( sign * n );
}

char *skipws (char *line)
{
  if( line != NULL )
  {
    while( *line == ' ' )
      line++;
  }
  return( line );
}

int split_line (char *line, char **words)
{
  int n = 0;
  char *sep = line;
  
  while ((line = skipws (sep)) != NULL && *line)
  {
    sep = strchr (line, ' ');

    if (sep)
      *sep++ = 0;

    words[n++] = line;
  }

  return (n);
}

// parse CLI data from serial port
void parse_line (int argc, char **argv)
{
  if (argc)
  {
    // check ESP32 alive signal 
    if (!strcmp (argv[0], "OK"))
    {
      esp32_wd_reset ();
    }

    // apply reset condition ESP32
    if (!strcmp (argv[0], "esp_stop"))
    {
      int time = rb_atoi (argv[1]);
      esp32_stop (time);
    }

    // remove reset condition ESP32 <> needed??
    if (!strcmp (argv[0], "esp_run"))
    {
      esp32_run ();
    }

    // dump log data to ESP32/ToyBox
    if (!strcmp (argv[0], "stat"))
    {
      stat_dump ();
      printf3 (">stat\n");
    }

    // touch buttons active
    if (!strcmp (argv[0], "touch_on"))
    {
      touch_enable ();
      printf3 (">touch buttons active\n");
    }

    // touch buttons disables
    if (!strcmp (argv[0], "touch_off"))
    {
      touch_disable ();
      printf3 (">touch buttons disabled\n");
    }

    // OLED power up
    if (!strcmp (argv[0], "oled_on"))
    {
      oled_power_up ();
      printf3 (">OLED power up\n");
    }

    // OLED power down
    if (!strcmp (argv[0], "oled_off"))
    {
      oled_power_down ();
      printf3 (">OLED power down\n");
    }

    // dump pump parameters
    if (!strcmp (argv[0], "pump_dump"))
    {
      pump_dump ();
      printf3 (">water pump parameters\n");
    }

    // stop waterpump program
    if (!strcmp (argv[0], "pump_stop"))
    {
      pump_regime (PUMPS_OFF);
      printf3 (">stop water pump program\n");
    }

    // waterpump daytime
    if (!strcmp (argv[0], "pump_day"))
    {
      pump_regime (PUMPS_DAY);
      printf3 (">water pumps daytime regime\n");
    }

    // waterpump nighttime
    if (!strcmp (argv[0], "pump_night"))
    {
      pump_regime (PUMPS_NIGHT);
      printf3 (">water pumps nighttime regime\n");
    }

    // waterpump on full
    if (!strcmp (argv[0], "pump_on"))
    {
      pump0_on ();
      pump1_on ();
      printf3 (">water pumps on\n");
    }

    // all waterpumps off
    if (!strcmp (argv[0], "pump_off"))
    {
      pump0_off ();
      pump1_off ();
      printf3 (">water pumps off\n");
    }

    // set water pump timing
    if (!strcmp (argv[0], "pump_set"))
    {
      if (argc == 3)
      {
        uint32_t ontime = rb_atoi (argv[1]);
        uint32_t period = rb_atoi (argv[2]);

        // sanity check
        if (ontime >= period)
          return;

        pump_set_ontime (ontime);
        pump_set_period (period);

        printf3 (">water pumps timing: %d %d [s]\n", ontime, period);
      }
    }

    // start EC sensor calibration
    if (!strcmp (argv[0], "ec_cal"))
    {
      ec_cal_sensor ();
      printf3 (">calibrate EC sensor\n");
    }

    // start logging to serial port
    if (!strcmp (argv[0], "log"))
    {
      log_start ();
      printf3 (">start logging\n");
    }

    // stop logging to serial port
    if (!strcmp (argv[0], "q"))
    {
      log_stop ();
      printf3 (">stop logging\n");           // ESP UART
    }

    // dump help data
    if (!strcmp (argv[0], "help"))
      help ();

    // dump version information
    if (!strcmp (argv[0], "version"))
      version ();

    // toggle WIFI module pass thru mode <> dev only
    if (!strcmp (argv[0], "pass"))
      pass_thru = ~pass_thru;
  }   
} 

// dump help
void help (void)
{
  // send to serial port
  printf2 ("stat                   : return line of log data\n");
  printf2 ("pump_stop              : stop water pump program\n");
  printf2 ("pump_day               : water pumps daytime regime");
  printf2 ("pump_night             : water pumps nighttime regime\n");
  printf2 ("pump_dump              : dump water pump parameters\n");
  printf2 ("pump_set <on> <period> : set pump parameters pumps [s]\n");
  printf2 ("pump_on                : all water pumps on with automatic timeout\n");
  printf2 ("pump_off               : all water pumps off\n");
  printf2 ("touch_on               : touch buttons enabled\n");
  printf2 ("touch_off              : touch buttons disabled\n");
  printf2 ("oled_on                : OLED display on\n");
  printf2 ("oled_off               : OLED display power down\n");
  printf2 ("ec_cal                 : start EC sensor calibration\n");
  printf2 ("log                    : start logging to UART & sending data\n");
  printf2 ("q                      : stop logging to UART & sending data\n");
  printf2 ("version                : version information\n");
  printf2 ("help                   : this help\n");

  // send to AP/PuTTY  (always start with '>')
  printf3 (">stat                   : return line of log data\n");
  printf3 (">pump_stop              : stop water pump program\n");
  printf3 (">pump_day               : water pumps daytime regime");
  printf3 (">pump_night             : water pumps nighttime regime\n");
  printf3 (">pump_dump              : dump water pump parameters\n");
  printf3 (">pump_set <on> <period> : set pump parameters pumps [s]\n");
  printf3 (">pump_on                : all water pumps on with automatic timeout\n");
  printf3 (">pump_off               : all water pumps off\n");
  printf2 (">touch_on               : touch buttons enabled\n");
  printf2 (">touch_off              : touch buttons disabled\n");
  printf3 (">oled_on                : OLED display on\n");
  printf3 (">oled_off               : OLED display power down\n");
  printf3 (">ec_cal                 : start EC sensor calibration\n");
  printf3 (">log                    : start logging to UART & sending data\n");
  printf3 (">q                      : stop logging to UART & sending data\n");
  printf3 (">version                : version information\n");
  printf3 (">help                   : this help\n");
}
     
// dump version info
void version (void)
{
  printf2  ("# -- NFT MASTER - " VERSION " --\n");
  printf3  ("> -- NFT MASTER - " VERSION " --\n");
}

// poll command line interface USART2
void poll_cli (void)
{
  int c = uart2_getc ();  // via TTL serial

  // test for Rx buffer empty
  if (c < 0)
    return;

  // send commandline data to WIFI module <> debug only
  if (pass_thru)
    printf3 ("%c", c);

  // add byte to buffer & parse
  if ((c == '\r') | (c == '\n'))                 // check for enter to end string
  {
    cli_buf[cli_buf_len] = '\0';                 // '/0' terminate buffer
    cli_buf_len = 0;                             // reset pointer
  
    cli_argc = split_line (cli_buf, cli_argv);   // parse command
    parse_line (cli_argc, cli_argv);          
  
  //prompt ();                                   // display new prompt
  }
  else if (c == '\b' || c == 0x7f)               // check for backspace and delete
  {
    if (cli_buf_len > 0)
      cli_buf_len--;
  }
  else if (isprint(c) && (cli_buf_len < INBUF_LEN))
    cli_buf[cli_buf_len++] = c;
}

// poll ESP32 module
void poll_esp (void)
{
  int c = uart3_getc ();

  // test for Rx buffer empty
  if (c < 0)
    return;

  // echo all data from ESP32 <> debug only
  printf2 ("\E[32m%c\E[30m", c);  // green text
  
  // add byte to buffer & parse
  if ((c == '\r') || (c == '\n'))                // check for cr/lf to end string
  {
    esp_buf[esp_buf_len] = '\0';                 // '/0' terminate buffer
    esp_buf_len = 0;                             // reset pointer
  
    esp_argc = split_line (esp_buf, esp_argv);   // parse command
    parse_line (esp_argc, esp_argv);          
  }
  else if (isprint(c) && (esp_buf_len < INBUF_LEN))
    esp_buf[esp_buf_len++] = c;
}















