/*
 * tcp_client.c
 *
 *  Created on: Feb 24, 2023
 *      Author: Kyrillos Sawiris
 */

#include "lwip/opt.h"

#include "lwip/api.h"
#include "lwip/sys.h"

#include "tcp_client.h"
#include "string.h"

static struct netconn *conn;
static struct netbuf *buf;
static ip_addr_t *addr, dest_addr;
static unsigned short port, dest_port;
char msgc[100];
char smsgc[200];

char message [10] = "hello";
char* ProgramToSend = (char*)0x10000000;
//int indx = 0;

// Function to send the data to the server
void tcpsend (char *data);

// tcpsem is the binary semaphore to prevent the access to tcpsend
//sys_sem_t tcpsem;

static void tcpinit_thread(void *arg)
{
	err_t err, connect_error;

	/* Create a new connection identifier. */
	conn = netconn_new(NETCONN_TCP);

	if (conn!=NULL)
	{
		/* Bind connection to the port number 7 (port of the Client). */
		err = netconn_bind(conn, IP_ADDR_ANY, 7);

		if (err == ERR_OK)
		{
			/* The desination IP adress of the computer */
			IP_ADDR4(&dest_addr, 169, 254, 84, 57);
			dest_port = 10;  // server port

			/* Connect to the TCP Server */
			connect_error = netconn_connect(conn, &dest_addr, dest_port);

			// If the connection to the server is established, the following will continue, else delete the connection
			if (connect_error == ERR_OK)
			{
				// Release the semaphore once the connection is successful
				//sys_sem_signal(&tcpsem);
				while (1)
				{
					/* wait until the data is sent by the server */
					if (netconn_recv(conn, &buf) == ERR_OK)
					{
						/* Extract the address and port in case they are required */
						addr = netbuf_fromaddr(buf);  // get the address of the client
						port = netbuf_fromport(buf);  // get the Port of the client

						/* If there is some data remaining to be sent, the following process will continue */
						do
						{

							strncpy (msgc, buf->p->payload, buf->p->len);   // get the message from the server

							// Or modify the message received, so that we can send it back to the server
							sprintf (smsgc, "\"%s\" was sent by the Server\n", msgc);

							// semaphore must be taken before accessing the tcpsend function
							//sys_arch_sem_wait(&tcpsem, 500);

							if(strcmp(msgc , "SEND")==0)
							{
							  sys_thread_new("tcpinit_thread", tcpinit_thread, NULL, DEFAULT_THREAD_STACKSIZE,osPriorityNormal);

							}

							// send the data to the TCP Server
							tcpsend (smsgc);

							memset (msgc, '\0', 100);  // clear the buffer
						}
						while (netbuf_next(buf) >0);

						netbuf_delete(buf);
					}
				}
			}

			else
			{
				/* Close connection and discard connection identifier. */
				netconn_close(conn);
				netconn_delete(conn);
			}
		}
		else
		{
			// if the binding wasn't successful, delete the netconn connection
			netconn_delete(conn);
		}
	}
}

void tcpsend (char *data)
{
	// send the data to the connected connection
	netconn_write(conn, data, strlen(data), NETCONN_COPY);
	// relaese the semaphore
	//sys_sem_signal(&tcpsem);
}


static void tcp_SendProgram (void *arg)
{

	tcpsend(ProgramToSend);

	//	for(int i=0;i<3000;i++)
	//		{
	//			ProgramToSend[i] = 'a';
	//		}
	//	for (;;)
	//	{
	//		//sprintf (smsgc, "index value = %d\n", indx++);
	//	// semaphore must be taken before accessing the tcpsend function
	//		//sys_arch_sem_wait(&tcpsem, 500);
	//		// send the data to the server
	//
	//		tcpsend(ProgramToSend);
	//		//osDelay(500);
	//	}
	osThreadId_t id;
	id = osThreadGetId();
	osThreadTerminate(id);
}


static void tcp_SendMessage (void *arg)
{
	tcpsend(message);
	osThreadId_t id;
	id = osThreadGetId();
	osThreadTerminate(id);

}

void tcp_ReseveMessage (void *arg )
{
	if (netconn_recv(conn, &buf) == ERR_OK)
	{
		do
		{

			strncpy (msgc, buf->p->payload, buf->p->len);   // get the message from the server

			// Or modify the message received, so that we can send it back to the server
			sprintf (smsgc, "\"%s\" was sent by the Server\n", msgc);

			// semaphore must be taken before accessing the tcpsend function
			//sys_arch_sem_wait(&tcpsem, 500);

			// send the data to the TCP Server
			tcpsend (smsgc);

			memset (msgc, '\0', 100);  // clear the buffer
		}
		while (netbuf_next(buf) >0);

		netbuf_delete(buf);
	}

	//	id = osThreadGetId();
	//	osThreadTerminate(id);
}



void tcpclient_init (void)
{
	for(int i=0;i<3000;i++)
	{
		ProgramToSend[i] = 'a';
	}
	//sys_sem_new(tcpsem, 0);  // the semaphore would prevent simultaneous access to tcpsend

	sys_thread_new("tcpinit_thread", tcpinit_thread, NULL, DEFAULT_THREAD_STACKSIZE,osPriorityNormal);
	//sys_thread_new("tcpSendProgram_thread", tcp_SendProgram, NULL, DEFAULT_THREAD_STACKSIZE,osPriorityNormal);
	//sys_thread_new("tcp_ReseveMessage", tcp_ReseveMessage, NULL, DEFAULT_THREAD_STACKSIZE,osPriorityNormal);
	//sys_thread_new("tcp_SendMessage", tcp_SendMessage, NULL, DEFAULT_THREAD_STACKSIZE,osPriorityNormal);

}
