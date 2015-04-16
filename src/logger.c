/*
 * logger.c
 *
 *  Created on: 15/04/2015
 *      Author: Universidade Federal
 */

#include "logger.h"

static char tohex(uint8_t val)
{
	if(val>15) val = 15; // saturate

	if(val>9)
	{
		return ((val-10) + 'A');
	}else
	{
		return (val + '0');
	}
}

void byte2hex(char *ret, uint8_t c)
{
	ret[0] = tohex((c>>4)&0x0F);
	ret[1] = tohex(c&0x0F);
}

void int2hex(char *ret, uint16_t c)
{
	byte2hex(ret,(uint8_t)(c>>8)&0xFF);
	byte2hex(ret+2,(uint8_t)(c)&0xFF);
}

#define HEADER_LEN 19

void log_setheader(char* filename, log_header_t * h)
{
   LOG_FILETYPE fp;
   char log_header[HEADER_LEN];

   if(log_openwrite(filename,&fp))
   {
	   log_header[0] = 'V';
	   byte2hex(&log_header[1],h->h1.version);
	   log_header[3] = 'M';
	   byte2hex(&log_header[4],h->h1.mon_id);
	   log_header[6] = 'B';
	   int2hex(&log_header[7],h->h1.entry_size);
	   log_header[11] = 'I';
	   int2hex(&log_header[12],h->h1.time_interv);
	   log_header[16] = '\r';
	   log_header[17] = '\n';
	   log_header[18] = '\0';
	   (void)log_write(log_header,&fp);
	   (void)log_close(&fp);
   }

}
void log_getheader(char* filename, log_header_t * h)
{

}

