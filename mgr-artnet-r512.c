#include "mgr-artnet-r512.h"

#define ARTNET_MAX_PORTS 1
#define UNCONNECTED -1
#define SHORT_NAME "MGR-ArtNet-Node"
#define LONG_NAME "MGR ArtNet Node DMX512"

#define PORTID_INPUT 0
#define PORTID_OUTPUT 1

uint8_t dmx_store_in[ARTNET_MAX_DMX];
uint8_t dmx_store_out[ARTNET_MAX_DMX];
bool node_running = false;
HANDLE hThread;
artnet_node node;
int ports[2] = {0,1};
int subnet = 0;

/**EXTERNAL DLL FUNCTION**/

//SET PORTS
void* setPORTS(int in, int out)
{
    extern int ports[2];
    ports[0] = in;
    ports[1] = out;

    if (node_running) restartNODE();
    return NULL;
}

//SET SUBNET
void* setSUBNET(int sub)
{
    extern int subnet;
    subnet = sub;

    if (node_running) restartNODE();
    return NULL;
}

//NODE START
int startNODE()
{
    //RESET ARRAYS
    memset(dmx_store_in, 0x00, ARTNET_MAX_DMX);
    memset(dmx_store_out, 0x00, ARTNET_MAX_DMX);

    //START THREAD
    extern bool node_running;
    DWORD dwThreadId, param=1;
    extern HANDLE hThread;

    if (!node_running)
    {
        hThread = CreateThread(
            NULL, // default security attributes
            0, // use default stack size
            start_node, // thread function
            &param, // argument to thread function
            0, // use default creation flags
            &dwThreadId); // returns the thread identifier
    }

    if (hThread == NULL) return 0;
    return 1;
}

//NODE STOP
void* stopNODE()
{
    extern bool node_running;
    extern HANDLE hThread;
    extern artnet_node node;

    if (node_running)
    {
        TerminateThread(hThread,0);
        node_running = false;
        artnet_destroy(node) ;
    }

    return NULL;
}

//NODE RESTART
void* restartNODE()
{
    if (node_running) stopNODE();
    startNODE();
    return NULL;
}

//INPUT VALUE
uint8_t getDMX(uint8_t ch)
{
    if ((ch >= 1) && (ch <= ARTNET_MAX_DMX)) return dmx_store_in[(ch-1)];
    return 0;
}

//OUTPUT VALUE
void* sendDMX(uint8_t ch, uint8_t value)
{
    extern artnet_node node;

    if ((ch >= 1) && (ch <= ARTNET_MAX_DMX) && (value >= 0) && (value <= 255))
    {
        dmx_store_out[(ch-1)] = value;
        artnet_send_dmx(node, PORTID_OUTPUT, ARTNET_MAX_DMX, dmx_store_out);
    }
    return NULL;
}

/**INTERNAL FUNCTION**/

//INIT OPTIONS
void* init(opts_t *ops)
{
	int i ;

    //OPTIONS
	ops->verbose 	= 0;
	ops->bind2Ip	= 0;
	ops->reuse		= 1 ;
	ops->timeout	= 0 ;
	strcpy(ops->cBcastAddr, "") ;

	ops->ip_addr = NULL ;
	ops->subnet_addr = 0 ;
	ops->port_addr = 1 ;
	ops->persist = 0;
	ops->num_ports = 1;

	for(i=0; i < ARTNET_MAX_PORTS; i++)
    {
		ops->fd[i] = UNCONNECTED ;
		memset(ops->dmx[i], 0x00, ARTNET_MAX_DMX+1) ;
	}

	ops->short_name = strdup(SHORT_NAME) ;
	ops->long_name = strdup(LONG_NAME) ;
	ops->config_file = NULL ;

	return NULL;
}

//NODE LOOP
DWORD WINAPI start_node(LPVOID lpParam)
{
    extern bool node_running;
    node_running = true;
    char *ip_addr = NULL ;
    int timeout = 3 ;
    extern int ports[2];
    extern int subnet;

    //INIT
    opts_t ops ;
    init(&ops) ;

    //NET OPTIONS
    if (ops.ip_addr) ip_addr = (char *) strdup(ops.ip_addr) ;
    if (ops.timeout > 0) timeout = ops.timeout ;

    //ARTNET START OPTIONS
    artnet_start_params anp ;
	anp.verbose	= ops.verbose ;
	anp.bind2Ip = ops.bind2Ip ;
	anp.reuse	= ops.reuse ;
	strcpy(anp.cBcastAddr, ops.cBcastAddr) ;

    //CREATE NODE
    extern artnet_node node;
    if ((node = artnet_new_mod(ip_addr, anp)) == NULL )
    {
        node_running = false;
        return 0;
    }

    //NODE SETUP
    artnet_set_short_name(node, SHORT_NAME) ;
    artnet_set_long_name(node, LONG_NAME) ;
    artnet_set_node_type(node, ARTNET_NODE) ;
    artnet_set_subnet_addr(node, subnet) ;

    //INPUT HANDLER
    artnet_set_dmx_handler(node, input_handler, (void*) &ops) ;

    //INPUT SETUP (type is OUTPUT for input Port)
    artnet_set_port_type(node, PORTID_INPUT, ARTNET_ENABLE_OUTPUT, ARTNET_PORT_DMX) ;
    artnet_set_port_addr(node, PORTID_INPUT, ARTNET_OUTPUT_PORT, ports[0]) ;

    //OUTPUT SETUP (type is INPUT for output Port)
    artnet_set_port_type(node, PORTID_OUTPUT, ARTNET_ENABLE_INPUT, ARTNET_PORT_DMX) ;
    artnet_set_port_addr(node, PORTID_OUTPUT, ARTNET_INPUT_PORT, ports[1]) ;

    //NODE START
    if( artnet_start(node) != ARTNET_EOK)
    {
        node_running = false;
        return 0;
    }

    //MAIN LOOP
    while(node_running) {
		artnet_read(node, timeout) ;
	}

	return 0;
}

//DMX RECIEVE HANDLER
int input_handler(artnet_node n, int port, void *d)
{
	uint8_t *data ;
	opts_t *ops = (opts_t *) d ;
	int len ;
	int i;

	if(port < ops->num_ports)
    {
		data = artnet_read_dmx(n, port, &len) ;
		//pthread_mutex_lock(&mem_mutex) ;
		memcpy(&ops->dmx[port][1], data, len) ;
		//pthread_mutex_unlock(&mem_mutex) ;

		//store DMX Values
		extern uint8_t dmx_store_in[ARTNET_MAX_DMX];
		for (i = 0; i < len; i++) if (i < ARTNET_MAX_DMX) dmx_store_in[i] = data[i];
	}
	return 0;
}
