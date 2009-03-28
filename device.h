/*
 * device.h
 *
 *  Created on: 23-mar-2009
 *      Author: filosganga
 */
#ifndef DEVICE_H_
#define DEVICE_H_

#include "repository.h"

typedef struct _device device;

device* device_create(device_hierarchy* hierarchy);

void device_destroy(device* device);

char* device_get_capability(device* device, char* name);

char* device_get_id(device* device);

char* device_get_user_agent(device* device);

#endif /* DEVICE_H_ */
