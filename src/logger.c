/*
 * logger.c
 *
 *  Created on: 15/04/2015
 *      Author: Universidade Federal
 */

#include "logger.h"

static char tohex(uint8_t val)
{
	if(val>15) val = 15;

	if(val>9)
	{
		return ((val-10) + 'A');
	}else
	{
		return (val + '0');
	}
}

static char tobcd(uint8_t val)
{
	if(val>9) val = 9;
	return (val + '0');
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

void u8tobcd(char *ret, uint8_t c)
{
	while(c>100){c-=100;} // keep below 99
	uint8_t d = (c/10);
	ret[0] = tobcd(d);
	ret[1] = tobcd(c-d*10);
}

void log_createentry(char* string, uint16_t *dados, uint16_t len)
{

	uint16_t dado = 0;
	do
	{
		dado=*dados;
		int2hex(string,dado);
		string+=4;
		dados++;
		len--;
	}while(len > 0);

	*string++='\r';
	*string++='\n';
	*string++='\0';
}

void log_makeheader(char log_header[], log_header_t * h)
{

   log_header[0] = 'V'; // versão
   byte2hex(&log_header[1],h->h1.version);
   log_header[3] = 'M'; // monitor ID
   byte2hex(&log_header[4],h->h1.mon_id);
   log_header[6] = 'B'; // tamanho da entrada de dados
   int2hex(&log_header[7],h->h1.entry_size);
   log_header[11] = 'I'; // intervalo de amostragem
   int2hex(&log_header[12],h->h1.time_interv);
   log_header[16] = '\r';
   log_header[17] = '\n';
   log_header[18] = 'T'; // estampa de tempo
   int2hex(&log_header[19],h->h2.year);
   byte2hex(&log_header[23],h->h2.mon);
   byte2hex(&log_header[25],h->h2.mday);
   byte2hex(&log_header[27],h->h2.hour);
   byte2hex(&log_header[29],h->h2.min);
   byte2hex(&log_header[31],h->h2.sec);
   log_header[33] = tohex(h->h2.synched);
   log_header[34] = '\r';
   log_header[35] = '\n';
   log_header[36] = 'P'; // índice da última entrada enviada
   int2hex(&log_header[37],h->last_idx);
   log_header[41] = '\r';
   log_header[42] = '\n';
   log_header[43] = 'C'; // contador de entradas do log
   int2hex(&log_header[44],h->count);
   log_header[48] = '\r';
   log_header[49] = '\n';
   log_header[50] = '\0';
}

void log_setheader(char* filename, log_header_t * h)
{
   LOG_FILETYPE fp;
   char log_header[LOG_HEADER_LEN+1];

   if(log_openread(filename,&fp))
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

void test_inttohex(void)
{

	char ret[4];
	byte2hex(ret,0xFF);
	assert((ret[0] == 'F') && (ret[1]== 'F'));

	byte2hex(ret,0x11);
	assert((ret[0] == '1') && (ret[1]== '1'));

	byte2hex(ret,0xA0);
	assert((ret[0] == 'A') && (ret[1]== '0'));

	byte2hex(ret,0x9E);
	assert((ret[0] == '9') && (ret[1]== 'E'));

	int2hex(ret,0xFF11);
	assert((ret[0] == 'F') && (ret[1]== 'F') && (ret[2] == '1') && (ret[3]== '1'));

	int2hex(ret,0xA09E);
	assert((ret[0] == 'A') && (ret[1]== '0') && (ret[2] == '9') && (ret[3]== 'E'));
}

void test_u8tobcd(void)
{
	char ret[4];

	u8tobcd(ret,99);
	assert((ret[0] == '9') && (ret[1]== '9'));

	u8tobcd(ret,13);
	assert((ret[0] == '1') && (ret[1]== '3'));
}

void test_logger(void)
{
	test_hextoint();
	test_inttohex();
	test_u8tobcd();
}

void log_getheader(char* filename, log_header_t * h)
{

	   LOG_FILETYPE fp;
	   char log_header[LOG_HEADER_LEN+1];
	   int idx = 0;
	   char hex1,hex2;
	   uint8_t b1,b2;

#define NEXT_2(res)	do{hex1 = log_header[idx++]; hex2 = log_header[idx++];} while(0); (res) = hex2byte(hex1,hex2);
#define NEXT_4(res) do{hex1 = log_header[idx++]; hex2 = log_header[idx++]; b1 = hex2byte(hex1,hex2); hex1 = log_header[idx++];hex2 = log_header[idx++]; b2 = hex2byte(hex1,hex2);}while(0); (res) = byte2int(b1,b2);

	   if(log_openread(filename,&fp))
	   {
		   if(log_read(log_header,LOG_HEADER_LEN,&fp))
		   {
			   if(log_header[idx++] == 'V')
			   {
				   NEXT_2(h->h1.version);
			   }
			   if(log_header[idx++]  == 'M')
			   {
				   NEXT_2(h->h1.mon_id);
			   }
			   if(log_header[idx++] == 'B')
			   {
				   NEXT_4(h->h1.entry_size);
			   }
			   if(log_header[idx++] == 'I')
			   {
				   NEXT_4(h->h1.time_interv);
			   }
			   assert (idx == 16);

		   }

		   // read next line
		   if(log_read(log_header,LOG_HEADER_LEN,&fp))
		   {
			   idx = 0;
			   if(log_header[idx++] == 'T')
			   {
				   NEXT_4(h->h2.year);
				   NEXT_2(h->h2.mon);
				   NEXT_2(h->h2.mday);
				   NEXT_2(h->h2.hour);
				   NEXT_2(h->h2.min);
				   NEXT_2(h->h2.sec);
				   h->h2.synched = hex2val(log_header[idx++]);
			   }
			   assert (idx == 16);
		   }

		   // read next line
		   if(log_read(log_header,LOG_HEADER_LEN,&fp))
		   {
			   idx = 0;
			   if(log_header[idx++] == 'P')
			   {
				   NEXT_4(h->last_idx);
			   }
			   assert (idx == 5);
		   }

		   // read next line
		   if(log_read(log_header,LOG_HEADER_LEN,&fp))
		   {
			   idx = 0;
			   if(log_header[idx++] == 'C')
			   {
				   NEXT_4(h->count);
			   }
			   assert (idx == 5);
		   }


		   (void)log_close(&fp);
	   }
}

void log_newheader(char* filename, uint8_t monitor_id, uint16_t interval, uint16_t entry_size)
{
	log_header_t h = {{0,0,0,0},{0,0,0,0,0,0,0},0,0};
	h.h1.mon_id = monitor_id;
	h.h1.time_interv = interval;
	h.h1.entry_size = entry_size;
	log_setheader(filename, &h);
}

void log_gettimestamp(char* timestamp)
{
	struct tm ts = *localtime(&(time_t){time(NULL)});
    strftime(timestamp,20,"T%Y%m%d%H%M%SS\r\n",&ts);
}

void log_settimestamp(char* filename)
{
	uint32_t time_elapsed_s = 0;
	time_t time_now = time(NULL);
	struct tm ts;
	log_header_t h = {{0,0,0,0},{0,0,0,0,0,0,0},0,0};
	log_getheader(filename, &h);
	time_elapsed_s = (h.h1.time_interv)*(h.count);
	time_now = time_now - time_elapsed_s;
	ts = *localtime(&(time_t){time_now});

#if 1
	h.h2.year = (uint16_t)(ts.tm_year + 1900);
	h.h2.mon = (uint8_t)(ts.tm_mon + 1);
	h.h2.mday = (uint8_t)ts.tm_mday;
	h.h2.hour = (uint8_t)ts.tm_hour;
	h.h2.min = (uint8_t)ts.tm_min;
	h.h2.sec = (uint8_t)ts.tm_sec;
#else
	h.h2.year = (uint16_t)2015;
	h.h2.mon = (uint8_t)04;
	h.h2.mday = (uint8_t)21;
	h.h2.hour = (uint8_t)18;
	h.h2.min = (uint8_t)10;
	h.h2.sec = (uint8_t)33;
	printf("\r\n%d%d%d%d%d%d\r\n", h.h2.year,h.h2.mon,h.h2.mday,h.h2.hour,h.h2.min,h.h2.sec);
#endif
	h.h2.synched = 1;

	do
	{
		char timestamp[20];
		strftime(timestamp,20,"T%Y%m%d%H%M%SS\r\n",&ts);
		puts(timestamp);
	}while(0);

	log_setheader(filename, &h);
}

void log_writeentry(char* filename, char* entry)
{
	uint16_t ret;
	LOG_FILETYPE fp;

	log_header_t h = {{0,0,0,0},{0,0,0,0,0,0,0},0,0};
	log_getheader(filename, &h);

	if(log_openappend(filename,&fp))
	{
	   ret = log_write(entry,&fp);
	   (void)log_close(&fp);
	   assert(ret == 1);

	   h.count++; // incrementa contador de entradas
	   log_setheader(filename, &h);

	}else
	{
	   assert(0);
	}
}

uint8_t log_readentry(char* filename, log_entry_t* entry)
{
	uint8_t entry_size;
	LOG_FILETYPE fp;
	LOG_FILEPOS  pos = LOG_HEADER_LEN;

	struct tm ts;

	log_header_t h = {{0,0,0,0},{0,0,0,0,0,0,0},0,0};
	log_getheader(filename, &h);

	entry_size = (h.h1.entry_size) + 2; // inclui \r\n

	if(h.last_idx < h.count)
	{
		ts.tm_year = h.h2.year - 1900;
		ts.tm_mon = h.h2.mon - 1;
		ts.tm_mday = h.h2.mday;
		ts.tm_hour = h.h2.hour;
		ts.tm_min = h.h2.min;
		ts.tm_sec = h.h2.sec;
		ts.tm_isdst = -1;

		time_t unix_time = mktime(&ts);

		if(unix_time > 0)
		{
			unix_time += (h.last_idx)*(h.h1.time_interv);
			entry->ts = unix_time;
		}

		if(log_openread(filename,&fp))
		{
			pos = pos + (h.last_idx)*entry_size;

			if(log_seek(&fp,&pos))
			{
			   (void)log_read(entry->values,entry_size+2,&fp);
			   (void)log_close(&fp);

			   h.last_idx++; // incrementa indice da última entrada lida
			   log_setheader(filename, &h);

			   return h.last_idx;
			}
		}
	}
	return 0;
}

