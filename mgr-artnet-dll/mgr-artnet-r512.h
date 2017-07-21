#ifndef MGR_ARTNET_H
#define MGR_ARTNET_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#include "artnet/artnet.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DLLZ __declspec(dllexport)

//DLL external function
void*   DLLZ setPORTS(int in, int out);
void*   DLLZ setSUBNET(int sub);
int     DLLZ startNODE();
void*   DLLZ stopNODE();
void*   DLLZ restartNODE();
uint8_t DLLZ getDMX(uint8_t ch);
void*   DLLZ sendDMX(uint8_t ch, uint8_t value);

//CONFIG OPTIONS
typedef struct
{
	int verbose ;
	char *dev[ARTNET_MAX_PORTS];
	char *ip_addr ;
	int subnet_addr ;
	int port_addr ;
	int persist ;
	int num_ports;
	int fd[ARTNET_MAX_PORTS] ;
	uint8_t dmx[ARTNET_MAX_PORTS][ARTNET_MAX_DMX +1] ;
	char *short_name ;
	char *long_name ;
	char *config_file ;
	int	 bind2Ip ;
	int  reuse ;
	char cBcastAddr[32] ;
	int  timeout ;
} opts_t;

//internal function
void* init(opts_t *ops);
DWORD WINAPI start_node(LPVOID lpParam);
int input_handler(artnet_node n, int port, void *d);

#ifdef __cplusplus
}
#endif
#endif  //MGR_ARTNET_H
