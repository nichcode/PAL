
/**
 * PAL - Prime Abstraction Layer (PAL)
 * A cross platform abstraction layer over graphics and windowing APIs
 * -------------------------------------------------------------------
 *
 * Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef LOG_H_
#define LOG_H_

#include "pal2/pal_core.h"
#include "shared.h"

/** Contains Log TLS state.*/
typedef struct LogTLSData
{
    char tmp[LOG_MSG_SIZE_];
    char buffer[LOG_MSG_SIZE_];
    bool isLogging;
} LogTLSData;

/** This is declared over here so other platforms will have accessed to it
 * for this TLS creation. The definition is in log.c.*/
void destroyTLSData_(void* data);

/** This will be implemented by the platforms.*/
void createLogTLS_(void);

/** This will be implemented by the platforms.*/
LogTLSData* getLogTLSData_(void);

/** This will be implemented by the platforms.*/
void setLogTLSData_(LogTLSData* data);

#endif // LOG_H_