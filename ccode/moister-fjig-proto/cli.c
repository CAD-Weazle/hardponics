// file    : cli.c
// author  : rb
// purpose : command line interface 
// date    : 081104
// last    : 200523
//

#include "includes.h"
#include <ctype.h>
#include <strings.h>

// globals
uint8_t buf_len;
char    buf[INBUF_LEN];

int   argc;
char *argv[10];

// print first prompt
void first_prompt (void)
{
  printf2 ("\n>");
}

// print prompt
void prompt (void)
{ 
  // normal prompt
  printf2 (">");
}

// convert string to integer
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

// convert string to long
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

// skip white space
char *skipws (char *line)
{
  if( line != NULL )
  {
    while( *line == ' ' )
      line++;
  }
  return( line );
}

// break up line in words
int split_line (char *line, char **words)
{
  int n = 0;
  char *sep = line;
  
  while( (line = skipws(sep)) != NULL && *line )
  {
    sep = strchr( line, ' ' );
    if (sep)
      *sep++ = 0;
    words[n++] = line;
  }
  return( n );
}

// command parser
void parse_line (int argc, char **argv)
{
  if (argc)
  {
    // dump help data
    if (!strcmp (argv[0], "help"))
      help ();

    // dump version information
    if (!strcmp (argv[0], "version"))
      version ();
  }   
} 

// dump help
void help (void)
{
  printf2 ("help              : this help\n");
  printf2 ("version           : version information\n");
}
     
// dump version info
void version (void)
{
  printf2 ("#- MOISTER - " VERSION " -\n");
}

// poll command line interface 
void poll_cli (void)
{
  // read next char
  int c = uart2_getc ();

  // test for Rx buffer empty
  if (c < 0)
    return;

  printf2 ("%c", c);

  // add byte to buffer & parse
  if ((c == '\r') | (c == '\n'))       // check for enter to end string
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
    if (buf_len > 0)
      buf_len--;
  }
  else if (isprint(c) && buf_len < INBUF_LEN)
    buf[buf_len++] = c;
}


