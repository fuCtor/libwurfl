/*
 * model.h
 *
 *  Created on: 23-mar-2009
 *      Author: filosganga
 */

#ifndef MODEL_H_
#define MODEL_H_

#include "collection/hashmap.h"
#include "collection/linkedlist.h"

// DeviceDefinition *******************************************************

typedef struct _devicedef_t devicedef_t;

devicedef_t* devicedef_create(char* id, char* user_agent, char* fall_back, int actual_device_root, hashmap_t* capabilities, hashmap_t* groups);

void devicedef_destroy(devicedef_t* device);

char* devicedef_get_id(devicedef_t* devicedef);

char* devicedef_get_user_agent(devicedef_t* devicedef);

char* devicedef_get_fall_back(devicedef_t* devicedef);

char* devicedef_is_root(devicedef_t* devicedef);

int devicedef_cmp(const devicedef_t* ldevicedef, const devicedef_t* rdevicedef);

// Hierarchy **************************************************************

typedef struct _device_hierarchy device_hierarchy_t;

device_hierarchy_t* create_hierarchy(linkedlist_t* devices);

void destroy_hierarchy(device_hierarchy_t* hierarchy);

int hierarchy_get_size(device_hierarchy_t* hierarchy);

char* hierarchy_get_capability(device_hierarchy_t* hierarchy, char* name);

devicedef_t* hierarchy_get_target(device_hierarchy_t* hierarchy);

devicedef_t* hierarchy_get_device(device_hierarchy_t* hierarchy, u_int32_t index);

// Repository *************************************************************

typedef struct _repository repository;

repository* create_repository(char* root_path, char** patches_paths);

void destroy_repository(repository* rep);

#endif /* MODEL_H_ */
