/****************************************************************************
 * Header file for the functions to lookup user names.
 ****************************************************************************/

#ifndef USERNAME_H
#define USERNAME_H

#include "main.h"

char *GetUserName(uid_t uid);
uid_t GetUserID(const char *name);

#endif

