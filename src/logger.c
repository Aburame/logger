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

#define HEADER_LEN 49
void log_makeheader(char log_header[], log_header_t * h)
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
   log_header[18] = 'T';
   int2hex(&log_header[19],h->h2.year);
   byte2hex(&log_header[23],h->h2.mon);
   byte2hex(&log_header[25],h->h2.mday);
   byte2hex(&log_header[27],h->h2.hour);
   byte2hex(&log_header[29],h->h2.min);
   byte2hex(&log_header[31],h->h2.sec);
   log_header[33] = tohex(h->h2.synched);
   log_header[34] = '\r';
   log_header[35] = '\n';
   int2hex(&log_header[36],h->last_idx);
   log_header[40] = '\r';
   log_header[41] = '\n';
   int2hex(&log_header[42],h->count);
   log_header[46] = '\r';
   log_header[47] = '\n';
   log_header[48] = '\0';
}

void log_setheader(char* filename, log_header_t * h)
{
   LOG_FILETYPE fp;
   char log_header[HEADER_LEN];

   if(log_openwrite(filename,&fp))
   {
	   log_makeheader(log_header,h);
	   (void)log_write(log_header,&fp);
	   (void)log_close(&fp);
   }

}

uint8_t hex2val(char c)
{
	if(c >= 'A' && c <= 'F') return (c - 'A' + 10);
	if(c >= '0' && c <= '9') return (c - '0');
	return 0; // 0 for any other case
}

static uint8_t hex2byte(char c1, char c2)
{
	return (hex2val(c1)*16 + hex2val(c2));
}

static uint16_t byte2int(uint8_t c1, uint8_t c2)
{
	return (uint16_t)(c1*256 + c2);
}

#include <assert.h>
void test_hextoint(void)
{
	uint16_t r;
	r = hex2byte('A','B');
	assert(r==0xAB);

	r = hex2byte('1','9');
	assert(r==0x19);

	r = byte2int(hex2byte('A','B'),hex2byte('1','9'));
	assert(r==0xAB19);
}

void log_getheader(char* filename, log_header_t * h)
{

	   LOG_FILETYPE fp;
	   char log_header[HEADER_LEN];
	   int idx = 0;
	   char hex1,hex2;
	   uint8_t b1,b2;

	   if(log_openread(filename,&fp))
	   {
		   if(log_read(log_header,HEADER_LEN,&fp))
		   {
			   if(log_header[idx++] == 'V')
			   {
				   hex1 = log_header[idx++];
				   hex2 = log_header[idx++];
				   h->h1.version = hex2byte(hex1,hex2);
			   }
			   if(log_header[idx++]  == 'M')
			   {
				   hex1 = log_header[idx++];
				   hex2 = log_header[idx++];
				   h->h1.mon_id = hex2byte(hex1,hex2);
			   }
			   if(log_header[idx++] == 'B')
			   {
				   hex1 = log_header[idx++];
				   hex2 = log_header[idx++];
				   b1 = hex2byte(hex1,hex2);
				   hex1 = log_header[idx++];
				   hex2 = log_header[idx++];
				   b2 = hex2byte(hex1,hex2);
				   h->h1.entry_size = byte2int(b1,b2);
			   }
			   if(log_header[idx++] == 'I')
			   {
				   hex1 = log_header[idx++];
				   hex2 = log_header[idx++];
				   b1 = hex2byte(hex1,hex2);
				   hex1 = log_header[idx++];
				   hex2 = log_header[idx++];
				   b2 = hex2byte(hex1,hex2);
				   h->h1.time_interv = byte2int(b1,b2);
			   }
			   assert (idx == 16);

			   idx+=2; /* skip new line */
			   if(log_header[idx++] == 'T')
			   {

			   }

			   assert (idx == 16);
		   }
		   (void)log_close(&fp);
	   }
}

