/********************************************************************************
 *      Copyright:  (C) 2025 Chen Chongyuan<1309010124@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  broker.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(09/08/25)
 *         Author:  lorawan <m15540097929@163.com>
 *      ChangeLog:  1, Release initial version on "09/08/25 20:44:27"
 *                 
 ********************************************************************************/

#ifndef _BROKER_H
#define _BROKER_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <mosquitto.h>

typedef struct mqtt_config_s
{
	char        id[64];
	char        host[64];
	int         port;
	char        username[32];
	char        password[32];
	char        topic[64];
	int         keepalive;
} mqtt_config_t;

typedef struct broker_context_s
{
	struct mosquitto *mosq_note;    
	struct mosquitto *mosq_server;  
	mqtt_config_t    *note_config;
	mqtt_config_t    *server_config;
} broker_context_t;

void note_connect_callback(struct mosquitto *mosq, void *obj, int rc);
void server_connect_callback(struct mosquitto *mosq, void *obj, int rc);
void note_message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message);
void server_message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message);
int gateway_mqtt_broker(mqtt_config_t *note_cfg, mqtt_config_t *server_cfg);

#endif

