/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, M	A 02111-1307, USA.
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include <artnet/artnet.h>

#define ARTNET_MAX_PORTS 1 // ### LH
//#define ARTNET_MAX_DMX   512 // ### LH

#define UNCONNECTED -1 

#define SHORT_NAME "ArtNet-Recv"
#define LONG_NAME "ArtNet-Receiver demo without output"

uint8_t dmx_store[512];

typedef struct {
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

int DMXOut(int len, unsigned char * cDMX)
{
	int i ;
	for (i = 0; i < 512; i++)
	{
		if (i < len) 
		{
			printf("%02X ", cDMX[i]) ;
		}
		else printf("00 ") ;
	}
	printf("\n") ;
	
	return 0 ;
}

int DMXEcho()
{
	int i ;
	for (i = 0; i < 3; i++)
	{
		extern uint8_t dmx_store[512];
		printf("%02X ", dmx_store[i]) ;
	}
	printf("\n") ;
	
	return 0 ;
}


/*
 * Called when we have dmx data pending
 */
int dmx_handler(artnet_node n, int port, void *d) {
	uint8_t *data ;
	opts_t *ops = (opts_t *) d ;
	int len ;
	int i;
	
	if(port < ops->num_ports) {
		data = artnet_read_dmx(n, port, &len) ;
		//pthread_mutex_lock(&mem_mutex) ;
		memcpy(&ops->dmx[port][1], data, len) ;
		//pthread_mutex_unlock(&mem_mutex) ;
		
		//echo DMX Values
		//DMXOut(len, data) ;
		//DMXEcho();
		
		//store DMX Values
		extern uint8_t dmx_store[512];
		for (i = 0; i < len; i++) if (i < 512) dmx_store[i] = data[i];
		
	}
	return 0;
}

/*
 * Set our default options, command line args will overide this
 */
void init_ops(opts_t *ops) {
	int i ;
	
	ops->verbose 	= 0;
	ops->bind2Ip	= 0;
	ops->reuse		= 1 ;
	ops->timeout	= 0 ;
	strcpy(ops->cBcastAddr, "") ;

	ops->ip_addr = NULL ;
	ops->subnet_addr = 0 ;
	ops->port_addr = 0 ;
	ops->persist = 0;
	ops->num_ports = 1;

	for(i=0; i < ARTNET_MAX_PORTS; i++) {
		ops->fd[i] = UNCONNECTED ;
		memset(ops->dmx[i], 0x00, ARTNET_MAX_DMX+1) ;
	}
	/*
	// dodgy but saves using vasprintf
	ops->dev[0] = strdup(DEFAULT_DEVICE) ;
	ops->dev[1] = strdup(DEFAULT_DEVICE2) ;
	ops->dev[2] = strdup(DEFAULT_DEVICE3) ;
	ops->dev[3] = strdup(DEFAULT_DEVICE4) ;
	*/
	
	ops->short_name = strdup(SHORT_NAME) ;
	ops->long_name = strdup(LONG_NAME) ;
	ops->config_file = NULL ;
}

/*
 * Parse command lines options and save in opts_s struct
 *
 */
void parse_args(opts_t *ops, int argc, char *argv[]) {
	// parse args
	int optc, port_addr, subnet_addr, num_ports ;

	// parse options 
	while ((optc = getopt (argc, argv, "c:s:p:d:a:vn:zt:brc:")) != EOF) {
		switch (optc) {
 			case 'a':
				free(ops->ip_addr) ;
				ops->ip_addr = (char *) strdup(optarg) ;
		        break;
			/*
 			case 'c':
				free(ops->config_file) ;
				ops->config_file = (char *) strdup(optarg) ;
		        break;
			*/
			case 't':
				ops->timeout = atoi(optarg) ;
		    break;		
			case 'b':
				ops->bind2Ip = 1 ;
				break;     
			case 'r':
				ops->reuse = 1 ;
				break;
			case 'u':
				ops->reuse = 0 ;
				break; 
			case 'c':
				strcpy(ops->cBcastAddr, (char *) strdup(optarg)) ;				
		    break;
			
			case 'v':
				ops->verbose = 1 ;
		        break;
			case 'd':
				free(ops->dev) ;
				ops->dev[0] = (char *) strdup(optarg) ;
		        break;
			case 's':
				subnet_addr = atoi(optarg) ;

				if(subnet_addr < 0 && subnet_addr > 15) 
					printf("Subnet address must be between 0 and 15\n") ;
				else
					ops->subnet_addr = subnet_addr ;
				break ;
			case 'n':
				num_ports = atoi(optarg) ;
				
				if(num_ports < 1 || num_ports > ARTNET_MAX_PORTS) {
					printf("number of ports must be between 1 and %d\n", ARTNET_MAX_PORTS) ;
				} else 
					ops->num_ports = num_ports ;
				break ;
			case 'p':
				port_addr = atoi(optarg) ;

				if(port_addr < 0 && port_addr > 15)
					printf("Port address must be between 0 and 15\n") ;
				else 
					ops->port_addr = port_addr ;
				break ;
			case 'z':
				ops->persist = 1 ;
      		default:
				break;
    	}
	}
}


int main(int argc, char *argv[]) {
	artnet_node node ;
	char *ip_addr = NULL ;
	int timeout = 2 ;
	artnet_start_params anp ;
	opts_t ops ;
	uint8_t dmx_store[512];
  
	init_ops(&ops) ;
	parse_args(&ops, argc, argv) ;  
	

	if (ops.ip_addr)
	  ip_addr = (char *) strdup(ops.ip_addr) ;
		
	anp.verbose		= ops.verbose ;
	anp.bind2Ip 	= ops.bind2Ip ;
	anp.reuse			= ops.reuse ;
	strcpy(anp.cBcastAddr, ops.cBcastAddr) ;
	if (ops.timeout > 0)
		timeout	= ops.timeout ;
  
  if ( (node = artnet_new_mod(ip_addr, anp) ) == NULL ) {
  // ### LH end
		printf("new failed %s\n" , artnet_strerror() ) ;
		goto error ;
	}

  
 	artnet_set_short_name(node, SHORT_NAME) ;
	artnet_set_long_name(node, LONG_NAME) ;
	artnet_set_node_type(node, ARTNET_NODE) ;
	artnet_set_subnet_addr(node, 0) ;

	// set dmx handler
	artnet_set_dmx_handler(node, dmx_handler, (void*) &ops) ;  

	// set the first port to output dmx data
	artnet_set_port_type(node, 0, ARTNET_ENABLE_OUTPUT, ARTNET_PORT_DMX) ;

	// set the universe address of the first port
	artnet_set_port_addr(node, 0, ARTNET_OUTPUT_PORT, ops.port_addr) ;

	if( artnet_start(node) != ARTNET_EOK) {
		printf("Failed to start: %s\n", artnet_strerror() ) ;
		goto error_destroy ;
	}
	
	while(1) {
		// set a 1 second timeout on the read
		// this way we send a DMX frame every second
		// even if we don't get any ArtNet packets
		artnet_read(node, 0) ;
		DMXEcho();
	}	

error_destroy :
	artnet_destroy(node) ;
	
error:
	free(ip_addr) ;
	exit(1) ;
}
