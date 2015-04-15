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

