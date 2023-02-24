//
//#include "tcp_server.h"
//#include "lwip/tcp.h"
//
//#include <time.h>
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
//#include "stm32f4xx_hal.h"
//#include "EthernetMacros.h"
//
//extern char SendBuf[SendBufLength];
//
//int* x = (int*)0x10000000;
//
//enum tcp_server_states
//{
//	ES_NONE = 0,
//	ES_ACCEPTED,
//	ES_RECEIVED,
//	ES_CLOSING
//};
//
//struct tcp_server_struct
//{
//	u8_t state;
//	u8_t retries;
//	struct tcp_pcb* pcb;
//	struct pbuf* p;
//};
//
//struct tcp_server_struct *esTx = 0;
//struct tcp_pcb *pcbTx = 0;
//
//
//void ethernet_send(char *buf , int len);
//static err_t tcp_server_accept(void* arg, struct tcp_pcb* newpcb, err_t err);
//static err_t tcp_server_recv(void* arg, struct tcp_pcb* newpcb, struct pbuf* p, err_t err);
//static void tcp_server_error(void *arg, err_t err);
//static err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb);
//static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
//
//static void tcp_server_send(struct tcp_pcb *tpcb, struct tcp_server_struct *es);
//static void tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *es);
//
//static void tcp_server_handle(struct tcp_pcb* tpcb, struct tcp_server_struct* es);
//
//void tcp_server_init(void)
//{
//	/* 1. create new tcp pcb */
//	struct tcp_pcb *tpcb;
//
//	tpcb = tcp_new();
//
//	err_t err;
//
//	/* 2. bind _pcb to port 7 ( protocol) */
//	ip_addr_t myIPADDR;
//	IP_ADDR4(&myIPADDR, 169, 254, 84, 58);
//	err = tcp_bind(tpcb, &myIPADDR, 31);
//
//	if (err == ERR_OK)
//	{
//		/* 3. start tcp listening for _pcb */
//		tpcb = tcp_listen(tpcb);
//
//		/* 4. initialize LwIP tcp_accept callback function */
//		tcp_accept(tpcb, tcp_server_accept);
//	}
//	else
//	{
//		/* deallocate the pcb */
//		memp_free(MEMP_TCP_PCB, tpcb);
//	}
//}
//
//static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
//{
//  err_t ret_err;
//  struct tcp_server_struct *es;
//
//  LWIP_UNUSED_ARG(arg);
//  LWIP_UNUSED_ARG(err);
//
//  /* set priority for the newly accepted tcp connection newpcb */
//  tcp_setprio(newpcb, TCP_PRIO_MIN);
//
//  /* allocate structure es to maintain tcp connection information */
//  es = (struct tcp_server_struct *)mem_malloc(sizeof(struct tcp_server_struct));
//  if (es != NULL)
//  {
//    es->state = ES_ACCEPTED;
//    es->pcb = newpcb;
//    es->retries = 0;
//    es->p = NULL;
//
//    /* pass newly allocated es structure as argument to newpcb */
//    tcp_arg(newpcb, es);
//
//    /* initialize lwip tcp_recv callback function for newpcb  */
//    tcp_recv(newpcb, tcp_server_recv);
//
//    /* initialize lwip tcp_err callback function for newpcb  */
//    tcp_err(newpcb, tcp_server_error);
//
//    /* initialize lwip tcp_poll callback function for newpcb */
//    tcp_poll(newpcb, tcp_server_poll, 0);
//
//    *x = 'a';
//
//    tcp_write(newpcb, x, 1, 0);
//
//    ret_err = ERR_OK;
//  }
//  else
//  {
//    /*  close tcp connection */
//    tcp_server_connection_close(newpcb, es);
//    /* return memory error */
//    ret_err = ERR_MEM;
//  }
//  return ret_err;
//}
//
//static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
//{
//  struct tcp_server_struct *es;
//  err_t ret_err;
//
//  LWIP_ASSERT("arg != NULL",arg != NULL);
//
//  es = (struct tcp_server_struct *)arg;
//
//  /* if we receive an empty tcp frame from client => close connection */
//  if (p == NULL)
//  {
//		/* remote host closed connection */
//	  	es->state = ES_CLOSING;
//
//		if(es->p == NULL)
//		{
//		   /* we're done sending, close connection */
//		   tcp_server_connection_close(tpcb, es);
//		}
//		else
//		{
//		  /* we're not done yet */
//		  /* acknowledge received packet */
//		  tcp_sent(tpcb, tcp_server_sent);
//
//		  /* send remaining data*/
//		  tcp_server_send(tpcb, es);
//		}
//		ret_err = ERR_OK;
//  }
//
//
//  /* else : a non empty frame was received from client but for some reason err != ERR_OK */
//  else if(err != ERR_OK)
//  {
//    /* free received pbuf*/
//    if (p != NULL)
//    {
//      es->p = NULL;
//      pbuf_free(p);
//    }
//    ret_err = err;
//  }
//
//  else if(es->state == ES_ACCEPTED)
//  {
//    /* first data chunk in p->payload */
//    es->state = ES_RECEIVED;
//
//    /* store reference to incoming pbuf (chain) */
//    es->p = p;
//
//    /* initialize LwIP tcp_sent callback function */
//    tcp_sent(tpcb, tcp_server_sent);
//
//    /* handle the received data */
//    tcp_server_handle(tpcb, es);
//
//    pbuf_free(p);
//
//    ret_err = ERR_OK;
//  }
//  else if (es->state == ES_RECEIVED)
//  {
//	  es->p = p;
//	  tcp_server_handle(tpcb, es);
//	  pbuf_free(p);
//    /* more data received from client and previous data has been already sent*/
////    if(es->p == NULL)
////    {
////      es->p = p;
//
//      /* handle the received data */
////      tcp_server_handle(tpcb, es);
////    }
////    else
////    {
////      struct pbuf *ptr;
////
////      /* chain pbufs to the end of what we recv'ed previously  */
////      ptr = es->p;
////      pbuf_chain(ptr,p);
////    }
//    ret_err = ERR_OK;
//  }
//  else if(es->state == ES_CLOSING)
//  {
//    /* odd case, remote side closing twice, trash data */
//    tcp_recved(tpcb, p->tot_len);
//    es->p = NULL;
//    pbuf_free(p);
//    ret_err = ERR_OK;
//  }
//  else
//  {
//    /* unknown es->state, trash data  */
//    tcp_recved(tpcb, p->tot_len);
//    es->p = NULL;
//    pbuf_free(p);
//    ret_err = ERR_OK;
//  }
//  return ret_err;
//}
//
//static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
//{
//	struct tcp_server_struct *es;
//
//	LWIP_UNUSED_ARG(len);
//
//	es = (struct tcp_server_struct *)arg;
//	es->retries = 0;
//
//	if(es->p != NULL)
//	{
////		tcp_sent(tpcb, tcp_client_sent);
//		tcp_server_send(tpcb, es);
//	}
//	else
//	{
//		if(es->state ==ES_CLOSING)
//		{
//			tcp_server_connection_close(tpcb, es);
//		}
//	}
//
//	return ERR_OK;
//}
//
//static void tcp_server_error(void *arg, err_t err)
//{
//
//}
////
//
//static void tcp_server_send(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
//{
//	struct pbuf *ptr;
//	err_t wr_err = ERR_OK;
//
//	while((wr_err == ERR_OK) &&(es->p != NULL) &&(es->p->len <= tcp_sndbuf(tpcb)))
//	{
//		ptr = es->p;
//
//		wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);
//
//		if(wr_err == ERR_OK)
//		{
//			u16_t plen;
//			u8_t freed;
//
//			plen = ptr->len;
//
//			es->p = ptr->next;
//
//			if(es->p != NULL)
//			{
//				pbuf_ref(es->p);
//			}
//			do
//			{
//				freed = pbuf_free(ptr);
//			}
//			while(freed==0);
//
//			tcp_recved(tpcb, plen);
//		}
//		else if(wr_err ==ERR_MEM)
//		{
//			es->p = ptr;
//		}
//		else
//		{
//
//		}
//	}
//	tcp_output(tpcb);
//}
//
//static void tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
//{
//	tcp_arg(tpcb, NULL);
//	tcp_sent(tpcb, NULL);
//	tcp_recv(tpcb, NULL);
//	tcp_err(tpcb, NULL);
//	tcp_accept(tpcb, NULL);
//	tcp_poll(tpcb, NULL,0);
//
//	if(es !=NULL)
//	{
//		mem_free(es);
//	}
//
//	tcp_close(tpcb);
//
//}
//
//static err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb)
//{
//	err_t ret_err;
//	struct tcp_server_struct *es;
//
//	es = (struct tcp_server_struct*)arg;
//
//	if(es != NULL)
//	{
//		if(es->p != NULL)
//		{
//
//		}
//		else
//		{
//			if(es->state == ES_CLOSING)
//			{
//				tcp_server_connection_close(tpcb,es);
//			}
//		}
//		ret_err = ERR_OK;
//	}
//	else
//	{
//		tcp_abort(tpcb);
//		ret_err = ERR_ABRT;
//	}
//	return ret_err;
//}
//
//
//static void tcp_server_handle (struct tcp_pcb *tpcb, struct tcp_server_struct *es)
//{
//	int i = 0;
//	static int c = 1;
//
//	for(; i < 256; i++)
//	{
//		x[i + (256 * (c-1))] = *(((int*)(es->p->payload)) + i);
//	}
//	i = es->p->len;
//	c++;
//}
//
//void ethernet_send(char *buf , int len)
//{
//	/* allocate pbuf */
//	esTx->p = pbuf_alloc(PBUF_TRANSPORT, len , PBUF_RAM);
//
//	/* copy data to pbuf */
//	pbuf_take(esTx->p, (char*)buf, len);
//
//	tcp_server_send(pcbTx, esTx);
//
//	pbuf_free(esTx->p);
//}
