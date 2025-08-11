/*********************************************************************************
 *      Copyright:  (C) 2025 Dameng Studio
 *                  All rights reserved.
 *
 *       Filename:  parse.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(2025年08月09日)
 *         Author:  lorawan <m15540097929@163.com>
 *      ChangeLog:  1, Release initial version on "2025年08月09日 03时33分38秒"
 *                 
 ********************************************************************************/
#if 0
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include "parson.h"

#endif

#include "parse.h"

int pares_mqtt_control(const char * conf_file)
{
	const char conf_obj_name[] = "mqtt_conf";
	JSON_Value *root_val;
	JSON_Object *conf_obj = NULL;
	JSON_Value *val;

	root_val = json_parse_file_with_comments(conf_file);
	if (root_val == NULL) 
	{   
		printf("ERROR: %s is not a valid JSON file\n", conf_file);
		return -1; 
	}

	conf_obj = json_object_get_object(json_value_get_object(root_val), conf_obj_name);
	if (conf_obj == NULL) {
		printf("INFO: %s does not contain a JSON object named %s\n", conf_file, conf_obj_name);
		return -2; 
	} else {
		printf("INFO: %s does contain a JSON object named %s, parsing gateway parameters\n", conf_file, conf_obj_name);
	}

	val = json_object_get_value(conf_obj, "mqtt"); /*  fetch value (if possible) */
	if (json_value_get_type(val) == JSONBoolean) {
		if ((bool)json_value_get_boolean(val) == true)
		{
			printf("Enable MQTT forwarding\n");
			return 0;
		}
		else
		{
			printf("Disable MQTT forwarding\n");
			return 1;
		}
	} else {
		printf("WARNING: Data type for mqtt seems wrong, please check\n");
		return -3;
	}
}

int parse_mqtt_configuration(const char * conf_file, mqtt_config_t *note_cfg, int i) 
{
	const char conf_obj_name[] = "mqtt_conf";
	JSON_Value *root_val;
	JSON_Object *conf_obj = NULL;
	JSON_Value *val = NULL; /*  needed to detect the absence of some fields */
	const char *str; /*  pointer to sub-strings in the JSON data */
	unsigned long long ull = 0;

	root_val = json_parse_file_with_comments(conf_file);
	if (root_val == NULL) 
	{
		printf("ERROR: %s is not a valid JSON file\n", conf_file);
		return -1;
	}    
	
	conf_obj = json_object_get_object(json_value_get_object(root_val), conf_obj_name);
	if (conf_obj == NULL) {
		printf("INFO: %s does not contain a JSON object named %s\n", conf_file, conf_obj_name);
		return -1;
	} else {
		printf("INFO: %s does contain a JSON object named %s, parsing gateway parameters\n", conf_file, conf_obj_name);
	}

	JSON_Object *note_conf_obj = NULL;

	printf("------------------------------------------------------------\n");

	if ( 0 == i ){
		note_conf_obj = json_object_get_object(conf_obj, "note_conf");
		printf("ready to get note config\n");
	} else if ( 1 == i) {
		note_conf_obj = json_object_get_object(conf_obj, "emqx_conf");
		printf("ready to get server config\n");
	} else {
		printf("input error\n");
		return -1;
	}

	if (note_conf_obj == NULL) {
		printf("error to get object\n");
		return -1;
	} else {
		str = json_object_get_string(note_conf_obj, "id");
		if (str != NULL) {
			strncpy(note_cfg->id, str, sizeof note_cfg->id);
			note_cfg->id[sizeof note_cfg->id - 1] = '\0'; /*  ensure string termination */
			printf("get id: %s\n", note_cfg->id);
		}

		str = json_object_get_string(note_conf_obj, "host");
		if (str != NULL) {
			strncpy(note_cfg->host, str, sizeof note_cfg->host);
			note_cfg->host[sizeof note_cfg->host - 1] = '\0'; /*   ensure string termination */
			printf("get host: %s\n", note_cfg->host);
		}

		val = json_object_get_value(note_conf_obj, "port");
		if (val != NULL) {
			note_cfg->port = (int)json_value_get_number(val);
			printf("get port: \"%d\"\n", note_cfg->port);
		}

		str = json_object_get_string(note_conf_obj, "username");
		if (str != NULL) {
			strncpy(note_cfg->username, str, sizeof note_cfg->username);
			note_cfg->username[sizeof note_cfg->username - 1] = '\0'; /*    ensure string termination */
			printf("get username: %s\n", note_cfg->username);
		}

		str = json_object_get_string(note_conf_obj, "password");
		if (str != NULL) {
			strncpy(note_cfg->password, str, sizeof note_cfg->password);
			note_cfg->password[sizeof note_cfg->password - 1] = '\0'; /*    ensure string termination */
			printf("get password: %s\n", note_cfg->password);
		}

		str = json_object_get_string(note_conf_obj, "up_topic");
		if (str != NULL) {
			strncpy(note_cfg->up_topic, str, sizeof note_cfg->up_topic);
			note_cfg->up_topic[sizeof note_cfg->up_topic - 1] = '\0'; /*    ensure string termination */
			printf("get up_topic: %s\n", note_cfg->up_topic);
		}

		str = json_object_get_string(note_conf_obj, "down_topic");
		if (str != NULL) {
			strncpy(note_cfg->down_topic, str, sizeof note_cfg->down_topic);
			note_cfg->down_topic[sizeof note_cfg->down_topic - 1] = '\0'; /*     ensure string termination */
			printf("get down_topic: %s\n", note_cfg->down_topic);
		}
		
		val = json_object_get_value(note_conf_obj, "keepalive");
		if (val != NULL) {         
			note_cfg->keepalive = (int)json_value_get_number(val);
			printf("get keepalive: \"%d\"\n", note_cfg->keepalive);
		}
	}

	return 0;
}
