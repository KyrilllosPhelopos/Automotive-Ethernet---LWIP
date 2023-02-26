/*
 * tcp_server.c
 *
 *  Created on: Feb 24, 2023
 *      Author: Kyrillos PHelopos Sawiris
 */


#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/sys.h"
#include "tcp_server.h"
#include "string.h"

//=============================================================================

//============netconn parameters=============================================
static struct netconn *conn, *newconn;
static struct netbuf *buf;
static ip_addr_t *addr;
static unsigned short port;

//=========================================================================

//==================ARRAYS to Hold DATA===================================
char ReceivedMessage[100];
char* ProgramToReceived = (char*)0x10000000;
char ToSendMessage[200];

int ProgramLength = 0;
//=========================================================

//===================static functions used ===========================
static void tcpsend (char *data);
static void tcp_ReseveMessage (void *arg );
static void tcp_ReseveProgram (void  );
static void tcp_SendMessage (void *arg);
//=====================================================================

/**** Send RESPONSE every time the client sends some data ******/
static void tcp_thread(void *arg)
{
	err_t err, accept_err, recv_error;

	/* Create a new connection identifier. */
	conn = netconn_new(NETCONN_TCP);

	if (conn!=NULL)
	{
		/* Bind connection to the port number 7. */
		err = netconn_bind(conn, IP_ADDR_ANY, 7);

		if (err == ERR_OK)
		{
			/* Tell connection to go into listening mode. */
			netconn_listen(conn);
			/* Grab new connection. */
			accept_err = netconn_accept(conn, &newconn);

			/* Process the new connection. */
			if (accept_err == ERR_OK)
			{
				sys_thread_new("tcp_ReseveMessage", tcp_ReseveMessage, NULL, DEFAULT_THREAD_STACKSIZE,osPriorityAboveNormal);
			}

		}
	}
	else
	{
		netconn_delete(conn);
	}
}



void tcpsend (char *data)
{
	netconn_write(conn, data, strlen(data), NETCONN_COPY);
}

void tcp_ReseveMessage (void *arg )
{

	while (1)
	{
		/* wait until the data is sent by the server */
		if (netconn_recv(conn, &buf) == ERR_OK)
		{
			/* Extract the address and port in case they are required */
			addr = netbuf_fromaddr(buf);  // get the address of the client
			port = netbuf_fromport(buf);  // get the Port of the client

			/* If there is some data remaining to be sent, the following process will continue */


			strncpy (ReceivedMessage, buf->p->payload, buf->p->len);   // get the message from the server

			if(strcmp(ReceivedMessage , "HI")== 0)
			{
				int len = sprintf (ToSendMessage, " send program Length \n");
				netconn_write(newconn, ToSendMessage, len, NETCONN_COPY);
				memset (ReceivedMessage, '\0', 100);  // clear the buffer
				netbuf_delete(buf);

				if (netconn_recv(conn, &buf) == ERR_OK){
					strncpy (ReceivedMessage, buf->p->payload, buf->p->len);
					ProgramLength=atoi(ReceivedMessage);
					memset (ReceivedMessage, '\0', 100);  // clear the buffer

					tcp_ReseveProgram ();
				}


			}
			else
			{
				// send the received data  using tcp_SendMessage
				//sys_thread_new("tcp_SendMessage", tcp_SendMessage, NULL, DEFAULT_THREAD_STACKSIZE,osPriorityNormal);
			}


			netbuf_delete(buf);
		}
	}
}

static void tcp_ReseveProgram (void )
{

	int len = sprintf (ToSendMessage, " start sending the program \n");
	netconn_write(newconn, ToSendMessage, len, NETCONN_COPY);

	static int i=0;
	netbuf_delete(buf);
	if(netconn_recv(conn, &buf) == ERR_OK)
	{
		do
		{
			//To store data

			static int c=0;
			strncpy((ProgramToReceived+i*c)  ,buf->p->payload, buf->p->len );
			i = buf->p->len;
			c++;
		}
		while (netbuf_next(buf) >0 && i < ProgramLength);

	}

	len = sprintf (ToSendMessage, " Program Received\n");
	netconn_write(newconn, ToSendMessage, len, NETCONN_COPY);  // send the message back to the client
	netbuf_delete(buf);


}


void tcpserver_init(void)
{
	sys_thread_new("tcp_thread", tcp_thread, NULL, DEFAULT_THREAD_STACKSIZE,osPriorityNormal);
}
