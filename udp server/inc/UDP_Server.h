/*
 * UDP_Server.h
 *
 *  Created on: Dec 3, 2022
 *      Author: Kyrillos Sawiris
 */

#ifndef INC_UDP_SERVER_H_
#define INC_UDP_SERVER_H_


#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "stdio.h"

void udpServer_init(void);
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);


#endif /* INC_UDP_SERVER_H_ */
