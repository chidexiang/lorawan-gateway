/*********************************************************************************
 * Copyright:  (C) 2025 Dameng Studio
 * All rights reserved.
 *
 * Filename:  broker.c
 * Description:  An improved MQTT gateway broker.
 * * Version:  2.0.0(2025年08月08日)
 * Author:  lorawan <m15540097929@163.com>
 * ChangeLog:  1, Refactored version to fix bugs and improve robustness.
 * ********************************************************************************/

#if 0
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <mosquitto.h>

//Note side MQTT config
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
#endif

#include "broker.h"

#if 0
void note_connect_callback(struct mosquitto *mosq, void *obj, int rc)
{
    broker_context_t *ctx = (broker_context_t *)obj;

    printf("-----------------------------------------------\n");

    if (rc == 0) 
    {
        printf("NOTE client connect success.\n");
        
        //subscribe when note connect success
        if (mosquitto_subscribe(mosq, NULL, ctx->note_config->topic, 0) != MOSQ_ERR_SUCCESS) 
        {
            printf("NOTE client subscribe failure: %s\n", strerror(errno));
        } 
        else 
        {
            printf("NOTE client subscribed to topic: %s\n", ctx->note_config->topic);
        }
    } 
    else 
    {
        printf("NOTE client connect failure: %s\n", mosquitto_connack_string(rc));
    }

    printf("-----------------------------------------------\n");
}

void server_connect_callback(struct mosquitto *mosq, void *obj, int rc)
{
    broker_context_t *ctx = (broker_context_t *)obj;

    printf("-----------------------------------------------\n");

    if (rc == 0) 
    {
        printf("SERVER client connect success.\n");
        
        //subscribe when server connect success
        if (mosquitto_subscribe(mosq, NULL, ctx->server_config->topic, 0) != MOSQ_ERR_SUCCESS) 
        {
            printf("SERVER client subscribe failure: %s\n", strerror(errno));
        }
        else 
        {
            printf("SERVER client subscribed to topic: %s\n", ctx->server_config->topic);
        }
    } 
    else 
    {
        printf("SERVER client connect failure: %s\n", mosquitto_connack_string(rc));
    }

    printf("-----------------------------------------------\n");
}

void note_message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
    broker_context_t *ctx = (broker_context_t *)obj;

    printf("-----------------------------------------------\n");
    printf("Received message from NOTE on topic '%s': %s\n", message->topic, (char *)message->payload);

    //publish to server
    if (mosquitto_publish(ctx->mosq_server, NULL, ctx->server_config->topic, message->payloadlen, message->payload, 0, false) != MOSQ_ERR_SUCCESS) 
    {
        printf("Failed to forward message to SERVER.\n");
    } 
    else 
    {
        printf("Message forwarded to SERVER on topic '%s'.\n", ctx->server_config->topic);
    }

    printf("-----------------------------------------------\n");
}

void server_message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
    broker_context_t *ctx = (broker_context_t *)obj;

    printf("-----------------------------------------------\n");
    printf("Received message from SERVER on topic '%s': %s\n", message->topic, (char *)message->payload);

    // publish to note
    if (mosquitto_publish(ctx->mosq_note, NULL, ctx->note_config->topic, message->payloadlen, message->payload, 0, false) != MOSQ_ERR_SUCCESS) 
    {
        printf("Failed to forward message to NOTE.\n");
    } 
    else 
    {
        printf("Message forwarded to NOTE on topic '%s'.\n", ctx->note_config->topic);
    }

    printf("-----------------------------------------------\n");
}
#endif
#if 0
int gateway_mqtt_broker(mqtt_config_t *note_cfg, mqtt_config_t *server_cfg)
{
#if 0
    mqtt_config_t note_cfg = {
        .id = "gateway-note-client",
        .host = "127.0.0.1",
        .port = 1883,
        .username = "note_user",
        .password = "note_pass",
        .topic = "lorawan/node/up",
        .keepalive = 60
    };

    emqx_config_t server_cfg = {
        .id = "gateway-server-client",
        .host = "broker.emqx.io",
        .port = 1883,
        .topic = "lorawan/server/down",
        .keepalive = 60
    };
#endif

    broker_context_t context = {0};
    context.note_config = note_cfg;
    context.server_config = server_cfg;

    int rv = 0;

    mosquitto_lib_init();

    //creat note client
    context.mosq_note = mosquitto_new(note_cfg->id, true, &context);
    if ( !context.mosq_note ) 
    {
        printf("mosq_note new failure: %s\n", strerror(errno));
        rv = -1;
        goto cleanup;
    }

    //creat server client
    context.mosq_server = mosquitto_new(server_cfg->id, true, &context);
    if ( !context.mosq_server ) 
    {
        printf("mosq_server new failure: %s\n", strerror(errno));
        rv = -1;
        goto cleanup;
    }

    //set note pw
    if (mosquitto_username_pw_set(context.mosq_note, note_cfg->username, note_cfg->password) != MOSQ_ERR_SUCCESS)
    {
        printf("mosq_up set username and passwd failure:%s\n", strerror(errno));
        rv = -2;
        goto cleanup;
    }

    //set callback
    mosquitto_connect_callback_set(context.mosq_note, note_connect_callback);
    mosquitto_message_callback_set(context.mosq_note, note_message_callback);
    mosquitto_connect_callback_set(context.mosq_server, server_connect_callback);
    mosquitto_message_callback_set(context.mosq_server, server_message_callback);

	printf("ready to connect\n");

    //note connect
    if (mosquitto_connect(context.mosq_note, note_cfg->host, note_cfg->port, note_cfg->keepalive) != MOSQ_ERR_SUCCESS) 
    {
        printf("mosq_note connect failure: %s\n", strerror(errno));
        rv = -3;
        goto cleanup;
    }

    //server connect
    if (mosquitto_connect(context.mosq_server, server_cfg->host, server_cfg->port, server_cfg->keepalive) != MOSQ_ERR_SUCCESS) 
    {
        printf("mosq_server connect failure: %s\n", strerror(errno));
        rv = -3;
        goto cleanup;
    }

    //loop start
    mosquitto_loop_start(context.mosq_note);
    mosquitto_loop_start(context.mosq_server);

    printf("MQTT Broker Gateway started. \n");

    while ( 1 ) 
    {
        sleep(1);
    }

    printf("Shutting down...\n");

cleanup:
    if (context.mosq_note) 
    {
        mosquitto_loop_stop(context.mosq_note, true);
        mosquitto_disconnect(context.mosq_note);
        mosquitto_destroy(context.mosq_note);
    }
    if (context.mosq_server) 
    {
        mosquitto_loop_stop(context.mosq_server, true);
        mosquitto_disconnect(context.mosq_server);
        mosquitto_destroy(context.mosq_server);
    }

    mosquitto_lib_cleanup();

    printf("Cleanup finished. Exiting.\n");

    return rv;
}
#endif
