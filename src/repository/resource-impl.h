/*
 * resource-impl.h
 *
 *  Created on: 31-mar-2009
 *      Author: filosganga
 */

#ifndef RESOURCEIMPL_H_
#define RESOURCEIMPL_H_

#include "resource.h"
#include "repository/devicedef.h"
#include "utils/collection/hashmap.h"

struct _resource_data_t {
	char* version;
	hashmap_t* devices;
};

#endif /* RESOURCEIMPL_H_ */