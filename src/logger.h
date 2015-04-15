/*
 * logger.h
 *
 *  Created on: 15/04/2015
 *      Author: Universidade Federal
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include "stdint.h"
#include "time.h"

#ifndef NULL
#define NULL  (void*)0
#endif

#define LOG_FILETYPE                  FILE*
#define log_openread(filename,file)   ((*(file) = fopen((filename),"rb")) != NULL)
#define log_openwrite(filename,file)  ((*(file) = fopen((filename),"wb")) != NULL)
#define log_openappend(filename,file)  ((*(file) = fopen((filename),"ab")) != NULL)
#define log_close(file)               (fclose(*(file)) == 0)
#define log_read(buffer,size,file)    (fgets((char*)(buffer),(size),*(file)) != NULL)
#define log_write(buffer,file)        (fputs((char*)(buffer),*(file)) >= 0)
#define log_rename(source,dest)       (rename((source), (dest)) == 0)
#define log_remove(filename)          (remove(filename) == 0)

#define LOG_FILEPOS                   fpos_t
#define log_tell(file,pos)            (fgetpos(*(file), (pos)) == 0)
#define log_seek(file,pos)            (fsetpos(*(file), (pos)) == 0)

/* type verification code */
static union
{
    char            int8_t_incorrect[sizeof( int8_t ) == 1];
    char            uint8_t_incorrect[sizeof( uint8_t ) == 1];
    char            int16_t_incorrect[sizeof( int16_t ) == 2];
    char            uint16_t_incorrect[sizeof( uint16_t ) == 2];
    char            int32_t_incorrect[sizeof( int32_t ) == 4];
    char            uint32_t_incorrect[sizeof( uint32_t ) == 4];
};

typedef struct
{
	uint16_t year;	/* Years since 0000 */
	uint8_t mon;	/* Months *since* january: 0-11 */
	uint8_t	mday;	/* Day of the month: 1-31 */
	uint8_t	hour;	/* Hours since midnight: 0-23 */
	uint8_t	min;	/* Minutes: 0-59 */
	uint8_t	sec;	/* Seconds: 0-59 */
}timestamp_t;

typedef struct
{
	uint8_t version;
	uint8_t mon_id;
	uint16_t entry_size;
	uint16_t time_interv;
}log_headerl1_t;

typedef struct
{
	uint16_t year;	/* Years since 0000 */
	uint8_t mon;	/* Months *since* january: 0-11 */
	uint8_t	mday;	/* Day of the month: 1-31 */
	uint8_t	hour;	/* Hours since midnight: 0-23 */
	uint8_t	min;	/* Minutes: 0-59 */
	uint8_t	sec;	/* Seconds: 0-59 */
	uint8_t synched; /* synch flag: (1) TRUE (0) FALSE */
}log_headerl2_t;

typedef struct
{
	log_headerl1_t h1; /* version, id, entry size and time interval */
	log_headerl1_t h2; /* timestamp and synch flag */
	uint16_t last_idx;	/* index of last sent line */
	uint16_t count;		/* entries count */
}log_header_t;

void byte2hex(char *ret, uint8_t c);
void int2hex(char *ret, uint16_t c);

#endif /* LOGGER_H_ */
