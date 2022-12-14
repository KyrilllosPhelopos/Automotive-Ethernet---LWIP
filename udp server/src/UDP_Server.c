#include "UDP_Server.h"


/*
 *Function Name: udpServer_init
 *Objective    : Initialize udp as a server
 *steps        :
 *				1- creat new udp_pcb
 *					using  udp_new();
 *
 *				2- bind the pcb to your ip & choose a port to be used
 *				    uning udp_bind(upcb, &myIPADDR, 7);
 *
 *				3- check if it has bind without no error
 *				4- start receiving data using dp_recv(upcb, udp_receive_callback, NULL);
 *
 */
void udpServer_init(void)
{
	// UDP Control Block structure
   struct udp_pcb *upcb;
   err_t err;

   /* 1. Create a new UDP control block  */
   upcb = udp_new();

   /* 2. Bind the upcb to the local port */
   ip_addr_t myIPADDR;
   IP_ADDR4(&myIPADDR, 169, 254, 84, 58);

   err = udp_bind(upcb, &myIPADDR, 7);  // 7 is the server UDP port


   /* 3. Set a receive callback for the upcb */
   if(err == ERR_OK)
   {
	   udp_recv(upcb, udp_receive_callback, NULL);
   }
   else
   {
	   udp_remove(upcb);
   }
}


/*
 *Function Name:udp_receive_callback
 *
 *Objective    : to be called when the udp server receives data
 *
 *Steps        :
 *Steps          1- create TX buffer to send data in it
 *Steps             struct pbuf *txBuf;
 *Steps
 *Steps          2- allocate memory for the TX Buf
 *Steps              txBuf = pbuf_alloc(PBUF_TRANSPORT,len, PBUF_RAM);
 *Steps
 *Steps          3- put the data you want into Tx Buffer
 *Steps            pbuf_take(txBuf, buf, len);
 *Steps
 *Steps          4- connect to the client
 *Steps             udp_connect(upcb, addr, port);
 *Steps
 *Steps
 *Steps          5- send data in the TX Buffer
 *Steps             udp_send(upcb, txBuf);
 *Steps
 *Steps
 */

//void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
//{
//	struct pbuf *txBuf;
//
//
//
//	char buf[100];
//
//
//	int len = sprintf (buf,"Hello %s From UDP SERVER\n", (char*)p->payload);
//
//	/* allocate pbuf from RAM*/
//	txBuf = pbuf_alloc(PBUF_TRANSPORT,len, PBUF_RAM);
//
//	/* copy the data into the buffer  */
//	pbuf_take(txBuf, buf, len);
//
//	/* Connect to the remote client */
//	udp_connect(upcb, addr, port);
//
//	/* Send a Reply to the Client */
//	udp_send(upcb, txBuf);
//
//	/* free the UDP connection, so we can accept new clients */
//	udp_disconnect(upcb);
//
//	/* Free the p_tx buffer */
//	pbuf_free(txBuf);
//
//	/* Free the p buffer */
//	pbuf_free(p);
//}
//
