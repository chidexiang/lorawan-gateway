/********************************************************************************
 *      Copyright:  (C) 2025 Chen Chongyuan<1309010124@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  parse.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(09/08/25)
 *         Author:  lorawan <m15540097929@163.com>
 *      ChangeLog:  1, Release initial version on "09/08/25 21:03:15"
 *                 
 ********************************************************************************/

#ifndef _PARSE_H
#define _PARSE_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include "parson.h"

int parse_mqtt_configuration(const char * conf_file, mqtt_config_t *note_cfg, int i);
int pares_mqtt_control(const char * confile);

#endif
