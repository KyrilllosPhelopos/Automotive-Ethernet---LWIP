

#include "tcp_client.h"
#include "lwip/tcp.h"

char* data_received = (char*)0x10000000;

enum tcp_client_states
{
	ES_NONE = 0,
	ES_CONNECTED,
	ES_RECEIVING,
	ES_CLOSING
};

struct tcp_client_struct
{
	u8_t state;
	u8_t retries;
	struct tcp_pcb* pcb;
	struct pbuf* p;
};

static err_t tcp_client_connected(void* arg, struct tcp_pcb* newpcb, err_t err);

static err_t tcp_client_recv(void* arg, struct tcp_pcb* newpcb, struct pbuf* p, err_t err);

static err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb);

static err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);

static void tcp_client_send(struct tcp_pcb *tpcb, struct tcp_client_struct *es);

static void tcp_client_connection_close(struct tcp_pcb *tpcb, struct tcp_client_struct *es);

static void tcp_client_handle(struct tcp_pcb* tpcb, struct tcp_client_struct* es);


int counter = 0;

uint8_t data[100];

extern TIM_HandleTypeDef htim1;

struct tcp_client_struct *esTx = 0;
struct tcp_pcb *pcbTx = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	char buf[100];

	/* Prepare the first message to send to the server */
	int len = sprintf (buf, "o", counter);

	char* x = "o";
	if (counter >= 0)
	{
		/* allocate pbuf */
//		esTx->p = pbuf_alloc(PBUF_TRANSPORT, len , PBUF_RAM);


		/* copy data to pbuf */
//		pbuf_take(esTx->p, (char*)buf, len);

//		tcp_write(tpcb,x , 1, 1);

		tcp_client_send(pcbTx, esTx);



//		pbuf_free(esTx->p);

//		counter = 0;
	}
}

void tcp_client_init(void)
{
	/* 1. create new tcp pcb */
	struct tcp_pcb *tpcb;

	tpcb = tcp_new();

	/* 2. Connect to the server */
	ip_addr_t destIPADDR;
	IP_ADDR4(&destIPADDR,169, 254, 84, 58);
	tcp_connect(tpcb, &destIPADDR, 31, tcp_client_connected);
}

static err_t tcp_client_connected(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  err_t ret_err;
  struct tcp_client_struct *es;

  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);

  /* allocate structure es to maintain tcp connection information */
  es = (struct tcp_client_struct *)mem_malloc(sizeof(struct tcp_client_struct));
  if (es != NULL)
  {
    es->state = ES_CONNECTED;
    es->pcb = newpcb;
    es->retries = 0;
    es->p = NULL;

    /* pass newly allocated es structure as argument to newpcb */
    tcp_arg(newpcb, es);
    /* initialize lwip tcp_recv callback function for newpcb  */
    tcp_recv(newpcb, tcp_client_recv);

    /* initialize lwip tcp_poll callback function for newpcb */
    tcp_poll(newpcb, tcp_client_poll, 0);


    /* initialize LwIP tcp_sent callback function */
    tcp_sent(newpcb, tcp_client_sent);

    /* handle the TCP data */
//    tcp_client_handle(newpcb, es);

    ret_err = ERR_OK;
  }
  else
  {
    /*  close tcp connection */
    tcp_client_connection_close(newpcb, es);
    /* return memory error */
    ret_err = ERR_MEM;
  }
  return ret_err;
}

static err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  struct tcp_client_struct *es;
  err_t ret_err;

//  if(es->state == ES_CONNECTED)
//  {
	  tcp_client_handle(tpcb, es);

     ret_err = ERR_OK;
//  }

  return ret_err;
}

static void tcp_client_handle (struct tcp_pcb *tpcb, struct tcp_client_struct *es)
{
	/* get the Remote IP */
	ip4_addr_t inIP = tpcb->remote_ip;
	uint16_t inPort = tpcb->remote_port;

	/* Extract the IP */
	char *remIP = ipaddr_ntoa(&inIP);

//	esTx->state = es->state;
//	esTx->pcb = es->pcb;
//	esTx->p = es->p;

	esTx = es;
	pcbTx = tpcb;

//	if(!(strcmp(es->p->payload,"o")))
//	{
		tcp_client_send(pcbTx, esTx);
//	}

//	counter++;

}

static void tcp_client_send(struct tcp_pcb *tpcb, struct tcp_client_struct *es)
{
	struct pbuf *ptr;
	err_t wr_err = ERR_OK;

//	for(int i = 0; i < 7000; i++)
//	{
//		if(i<=1000)
//		{
//			data_received[i] = 'a';
//		}
//		if((i<=2000) && (i>1000))
//		{
//			data_received[i] = 'b';
//		}
//		if((i<=3000) && (i>2000))
//		{
//			data_received[i] = 'c';
//		}
//		if((i<=4000) && (i>3000))
//		{
//			data_received[i] = 'd';
//		}
//		if((i<=5000) && (i>4000))
//		{
//			x[i] = 'e';
//		}
//		if((i<=6000) && (i>5000))
//		{
//			x[i] = 'f';
//
//		}
//		if((i<=7000) && (i>6000))
//		{
//			x[i] = 'k';
//		}
//
//	}

//	while((wr_err == ERR_OK) && (es->p != NULL) && (es->p->len <= tcp_sndbuf(tpcb)))
//	{
//		ptr = es->p;

		tcp_write(tpcb, data_received, 7000, 0);

}

static err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	struct tcp_client_struct *es;

	LWIP_UNUSED_ARG(len);

	es = (struct tcp_client_struct *)arg;
	es->retries = 0;

	if(es->p != NULL)
	{
//		tcp_sent(tpcb, tcp_client_sent);
//		tcp_client_send(tpcb, es);
	}
	else
	{
		if(es->state ==ES_CLOSING)
		{
			tcp_client_connection_close(tpcb, es);
		}
	}

	return ERR_OK;
}

static void tcp_client_connection_close(struct tcp_pcb *tpcb, struct tcp_client_struct *es)
{
	tcp_arg(tpcb, NULL);
	tcp_sent(tpcb, NULL);
	tcp_recv(tpcb, NULL);
	tcp_err(tpcb, NULL);
	tcp_poll(tpcb, NULL,0);

	if(es !=NULL)
	{
		mem_free(es);
	}

	tcp_close(tpcb);

}

static err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb)
{
	err_t ret_err;
	struct tcp_client_struct *es;

	es = (struct tcp_client_struct*)arg;

	if(es != NULL)
	{
		if(es->p != NULL)
		{

		}
		else
		{
			if(es->state == ES_CLOSING)
			{
				tcp_client_connection_close(tpcb,es);
			}
		}
		ret_err = ERR_OK;
	}
	else
	{
		tcp_abort(tpcb);
		ret_err = ERR_ABRT;
	}
	return ret_err;
}
