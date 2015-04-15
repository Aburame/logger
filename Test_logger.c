/*
 ============================================================================
 Name        : Test_logger.c
 Author      : Carlos H. Barriquello
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================

 - Arquivo de log local

1) um arquivo para cada monitor. Monitor é entendido como um conjunto de dados estruturados,
geralmente, associado a um único equipamento. Ex.: medidas elétricas associdas ao um
medidor de grandezas elétricas.

2) Dados devem guardados na forma de séries temporais ordenadas.
Uma série temporal é um conjunto de dados associados a uma estampa de tempo.

3) As séries são armazenadas no formato ASCII hexadecimal como um conjunto de bytes finalizados
por um caracter de nova linha (\r\n), correspondendo cada linha a uma entrada de dados com
estampa de tempo.

4) As estampas de tempo podem estar nas entradas de dados, ou serem calculados a partir de uma
estampa inicial (ex. data e hora da criação do arquivo), quando estiverem separadas por um
intervalo constante. O formato da estampa é determinada pela versão do log. V00 - com estampas
incluídas, V01 -  com estampas calculadas a partir da estampa inicial. As configurações são
guardadas no cabeçalho

5) Cada arquivo inicia com um cabeçalho contendo (4 linhas de 16 caracteres cada):
L1: Versao e Monitor ID, Bytes por linha, intervalo,
L2: data e hora inicial, flag de sincronização do arquivo,
L3: contador de entradas (linhas) (2 bytes),
L4: indice da última linha enviada (2 bytes).


Ex.: .
V01M00I0030B0080
TaaaammddhhmmssN
0000
0000
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

#define LOG_FILETYPE                  FILE*
#define log_openread(filename,file)   ((*(file) = fopen((filename),"rb")) != NULL)
#define log_openwrite(filename,file)  ((*(file) = fopen((filename),"wb")) != NULL)
#define log_close(file)               (fclose(*(file)) == 0)
#define log_read(buffer,size,file)    (fgets((buffer),(size),*(file)) != NULL)
#define log_write(buffer,file)        (fputs((buffer),*(file)) >= 0)
#define log_rename(source,dest)       (rename((source), (dest)) == 0)
#define log_remove(filename)          (remove(filename) == 0)

#define LOG_FILEPOS                   fpos_t
#define log_tell(file,pos)            (fgetpos(*(file), (pos)) == 0)
#define log_seek(file,pos)            (fsetpos(*(file), (pos)) == 0)

void test_openlog(void)
{

   LOG_FILETYPE fp;
   if(log_openwrite("file.txt",&fp))
   {
	   (void)log_close(&fp);
   }else
   {
	   assert(0);
   }


}

void test_writelogts(void)
{

	LOG_FILETYPE fp;
	char timestamp[80];
	int ret;

    struct tm ts = *localtime(&(time_t){time(NULL)});
    strftime(timestamp,80,"T%Y%m%d%H%M%SS",&ts);
    puts(timestamp);

   if(log_openwrite("file.txt",&fp))
   {
	   ret = log_write(timestamp,&fp);
	   (void)log_close(&fp);
	   assert(ret == 1);
   }else
   {
	   assert(0);
   }
}

char tohex(char val)
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
void char2hex(char *ret, unsigned char c)
{
	ret[0] = tohex((c>>4)&0x0F);
	ret[1] = tohex(c&0x0F);
}

void test_converthex(void)
{

	char ret[2];
	char2hex(ret,0xFF);
	assert((ret[0] == 'F') && (ret[1]== 'F'));

	char2hex(ret,0x11);
	assert((ret[0] == '1') && (ret[1]== '1'));

	char2hex(ret,0xA0);
	assert((ret[0] == 'A') && (ret[1]== '0'));

	char2hex(ret,0x9E);
	assert((ret[0] == '9') && (ret[1]== 'E'));

}

void test_createentry(void)
{

	int vetor_dados[3]={0x1111,0x2222,0x3333};
	char string[20];
	int j=0;
	int k=0;
	for(k=0;k<3;k++)
	{
		char2hex((char*)&string[j],(unsigned char)(vetor_dados[k]>>8)&0xFF);
		char2hex((char*)&string[j+2],(unsigned char)(vetor_dados[k])&0xFF);
		j+=4;
	}

	assert((string[0] == '1') && (string[1]== '1') && (string[2] == '1') && (string[3]== '1') &&
			(string[0+4] == '2') && (string[1+4]== '2') && (string[2+4] == '2') && (string[3+4]== '2') &&
			(string[0+2*4] == '3') && (string[1+2*4]== '3') && (string[2+2*4] == '3') && (string[3+2*4]== '3'));



}



int main(void) {
	struct timeb start, end;
	int diff;

	ftime(&start);

	test_openlog();
	test_writelogts();
	test_converthex();
	test_createentry();

	//test_minini();

	ftime(&end);
	diff = (int) (1000.0 * (end.time - start.time)
		+ (end.millitm - start.millitm));

	printf("\nOperation took %u milliseconds\n", diff);

	getchar();
	return EXIT_SUCCESS;
}
