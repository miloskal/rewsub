/*  
    This library is intended for movie-subtitle synchronization.
  
    Copyright (C) 2020 Miloš Kaličanin <miloskal0108997@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>
#include <string.h>

#define MAX_LINE 1024

const static char 
timestamp_line[84] = "[0-9]{2}:[0-5][0-9]:[0-5][0-9],[0-9]{3} --> "
                     "[0-9]{2}:[0-5][0-9]:[0-5][0-9],[0-9]{3}";

/* error handler */
static void
error_fatal(char* msg);

/* rewinds file 'input' for 'delay_quantity' miliseconds and saves modified 
 * file as 'output'                                                       */
void
rewind_subtitle(char *input, long long delay_quantity, char *output);

/* rewinds line 'hh:mm:ss,mmm --> hh:mm:ss,mmm' for 'rewind_quantity' 
 * miliseconds and stores result in string 'conversion_result'            */
static void
rewind_line(char *line, long long rewind_quantity, char *conversion_result);




/* IMPLEMENTATION */

static void
error_fatal(char* msg)
{
  perror(msg);
  exit(EXIT_FAILURE);
}

static void rewind_line (char *line, long long delay_quantity, 
                         char *conversion_result)
{
  char start_timestamp[128], end_timestamp[128];
  unsigned long long sum_ms;
  int hh, mm, ss, ms;

  sscanf(line, "%s --> %s", start_timestamp, end_timestamp);
  
  sscanf(start_timestamp, "%d:%d:%d,%d", &hh, &mm, &ss, &ms);
  sum_ms = ms + 1000*ss + 60000*mm + 3600000*hh;
  sum_ms += delay_quantity;
  hh = sum_ms / 3600000;
  mm = sum_ms % 3600000 / 60000;
  ss = sum_ms % 60000 / 1000;
  ms = sum_ms % 1000;
  sprintf(start_timestamp, "%02d:%02d:%02d,%03d", hh, mm, ss, ms);

   
  sscanf(end_timestamp, "%d:%d:%d,%d", &hh, &mm, &ss, &ms);
  sum_ms = ms + 1000*ss + 60000*mm + 3600000*hh;
  sum_ms += delay_quantity;
  hh = sum_ms / 3600000;
  mm = sum_ms % 3600000 / 60000;
  ss = sum_ms % 60000 / 1000;
  ms = sum_ms % 1000;
  sprintf(end_timestamp, "%02d:%02d:%02d,%03d", hh, mm, ss, ms);

  sprintf(conversion_result, "%s --> %s\n", start_timestamp, end_timestamp);
}

void
rewind_subtitle(char *input, long long delay_quantity, char *output)
{
  regex_t preg;
  FILE *f_in, *f_out;
  char line[MAX_LINE], conversion_result[MAX_LINE];

  if (regcomp(&preg, timestamp_line, REG_EXTENDED) != 0)
    error_fatal("regcomp");

  if ((f_in = fopen(input, "r")) == NULL)
    error_fatal("fopen1");

  if ((f_out = fopen(output, "a")) == NULL)
    error_fatal("fopen2");
  
  while(fgets(line, MAX_LINE, f_in) != NULL)
    {
      if (!regexec(&preg, line, 0, NULL, 0))
        {
          rewind_line(line, delay_quantity, conversion_result);
          fputs(conversion_result, f_out);
        }
      else 
        {
          fputs(line, f_out);
        }
    }

  regfree(&preg); 
  fclose(f_in);
  fclose(f_out);
}
