/*
 ============================================================================
 Name        : Test_logger.c
 Author      : Carlos H. Barriquello
 Version     :
 Description : Data logger
 ============================================================================

 - Arquivo de log local

1) um arquivo para cada monitor. Monitor é entendido como um conjunto de dados estruturados,
geralmente, associado a um único equipamento. Ex.: medidas elétricas associdas ao um
medidor de grandezas elétricas.

2) Dados devem guardados na forma de séries temporais ordenadas.
Uma série temporal é um conjunto de dados associados a uma estampa de tempo.

3) As séries são armazenadas no formato ASCII hexadecimal como um conjunto de bytes finalizados
por "nova linha e retorno" (\r\n), correspondendo cada linha a uma entrada de dados com
estampa de tempo.

4) As estampas de tempo podem estar nas entradas de dados, ou serem calculados a partir de uma
estampa inicial (ex. data e hora da criação do arquivo), quando estiverem separadas por um
intervalo constante. O formato da estampa é determinada pela versão do log. V00 - com estampas calculadas a partir da estampa inicial. As configurações são
guardadas no cabeçalho, V01 - com estampas incluídas

5) Cada arquivo inicia com um cabeçalho contendo (3 linhas com até 16 caracteres cada):
L1: Versao e Monitor ID, Bytes por linha, intervalo, terminador (#)
L2: data e hora inicial, flag de sincronização do arquivo,
L3: indice da última linha enviada (4 bytes).
L4: contador de entradas (linhas) (4 bytes)

Ex.: .
V00M00B0080I0030\r\n
TaaaammddhhmmssN\r\n
P0000\r\n
C0000\r\n
00
01
02
03
04


6) Os arquivos do logger são nomeados com a data/hora de sua criação no formato "AAMMDDHH.txt". Ao iniciar o logger, ele ordena os arquivos por data e tenta abrir o último arquivo salvo.
Caso ele não esteja sincronizado, os dados são armazenados no arquivo "99123123.txt". Este arquivo deverá ser renomeado com a data/hora de sua criação, quando houver a sincronização do relógio do logger. A data/hora de criação será obtida a partir da data atual retrocedida de acordo com a quantidade de entradas de dados no arquivo. Caso já haja um arquivo com o mesmo nome, o mesmo deverá ser renomeado para a data/ hora logo posterior.

Ex.:

15010107.txt >
...I0030...
T20150101073000S
...
00
01
02
03


99123123.txt >
...
T00000000000000N
...
00
01
02

sincronização em T20150101073400S

-> renomear para:

15010108.txt >
...
T20150101073300S ->
...
00 -> 3300
01 -> 3330
02 -> 3400


7) Caso o contador de entradas seja igual a FFFF (65535). Fecha-se o arquivo e inicia-se um arquivo novo.
8) Caso o indice da última linha enviada seja igual a FFFE (65534), o arquivo deverá ser renomeado com alteração da extensão (.txt -> .txd).
9) OS arquivos de log de cada monitor são guardados em diretórios separados nomeados com a ID do monitor.


 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <sys\timeb.h>

#include "minIni.h"
#include "logger.h"


void test_openlog(void)
{

   LOG_FILETYPE fp;
   char* filename = "file.txt";

   if(log_openread(filename,&fp))
   {
	   (void)log_close(&fp);
   }else
   {
	   if(log_openwrite(filename,&fp))
	   {
		   log_newheader(filename,0,30,12);
		   (void)log_close(&fp);
	   }
   }

}

void test_writelogts(void)
{

	LOG_FILETYPE fp;
	char timestamp[20];
	uint16_t ret;

    struct tm ts = *localtime(&(time_t){time(NULL)});
    strftime(timestamp,80,"T%Y%m%d%H%M%SS\r\n",&ts);
    puts(timestamp);

   if(log_openread("file.txt",&fp))
   {
	   ret = log_write(timestamp,&fp);
	   (void)log_close(&fp);
	   assert(ret == 1);
   }else
   {
	   assert(0);
   }
}


void test_createentry(void)
{

	uint16_t vetor_dados[3]={0x1111,0x2222,0x3333};
	char string[20];

	log_createentry(string,vetor_dados,3);

	assert((string[0] == '1') && (string[1]== '1') && (string[2] == '1') && (string[3]== '1') &&
			(string[0+4] == '2') && (string[1+4]== '2') && (string[2+4] == '2') && (string[3+4]== '2') &&
			(string[0+2*4] == '3') && (string[1+2*4]== '3') && (string[2+2*4] == '3') && (string[3+2*4]== '3'));

}

void test_setheader(void)
{
	log_header_t h = {{0,0,0,0},{0,0,0,0,0,0,0},0,0};
	h.h1.version = 0;
	h.h1.mon_id = 0;
	h.h1.time_interv = 30;
	h.h1.entry_size = 32;
	log_setheader("file.txt", &h);
}

void test_getheader(void)
{
	log_header_t h = {{0,0,0,0},{0,0,0,0,0,0,0},0,0};
	log_getheader("file.txt", &h);
}


#include "string.h"
void test_writeentry(void)
{

	uint16_t vetor_dados[3]={0x1111,0x2222,0x3333};
	char string[20];
	char string2[20];

	log_createentry(string,vetor_dados,3);

	assert((string[0] == '1') && (string[1]== '1') && (string[2] == '1') && (string[3]== '1') &&
			(string[0+4] == '2') && (string[1+4]== '2') && (string[2+4] == '2') && (string[3+4]== '2') &&
			(string[0+2*4] == '3') && (string[1+2*4]== '3') && (string[2+2*4] == '3') && (string[3+2*4]== '3'));

	log_writeentry("file.txt",string);
	log_readentry("file.txt", string2);

	assert(strcmp(string,string2) == 0);

}

void test_readentry(void)
{

	char string[20];
	log_readentry("file.txt", string);
	puts(string);

}

int main(void) {
	struct timeb start, end;
	uint16_t diff;

	ftime(&start);

	test_logger();
	test_openlog();

	//test_setheader();
	//test_getheader();
	//test_createentry();

	test_writeentry();
	test_readentry();


	/*
	test_writelogts();
	*/

	//test_minini();

	ftime(&end);
	diff = (uint16_t) (1000.0 * (end.time - start.time)
		+ (end.millitm - start.millitm));

	printf("\nOperation took %u milliseconds\n", diff);

	getchar();
	return EXIT_SUCCESS;
}
