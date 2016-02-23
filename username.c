/****************************************************************************
 * Functions for getting user names from user id's.
 ****************************************************************************/

#include "main.h"

typedef struct {
	uid_t uid;
	char name[USERNAME_SIZE + 1];
	void *next;
} UserNameType;

static UserNameType *userNames = NULL;

static void GetUserNames();

/****************************************************************************
 * Look up a user name.
 ****************************************************************************/
char *GetUserName(uid_t uid) {
	UserNameType *up;

	if(!userNames) {
		GetUserNames();
	}

	for(up = userNames; up; up = up->next) {
		if(up->uid == uid) {
			return up->name;
		}
	}
	return NULL;

}

/****************************************************************************
 * Look up a user id.
 ****************************************************************************/
uid_t GetUserID(const char *name) {
	UserNameType *up;

	if(!userNames) {
		GetUserNames();
	}

	for(up = userNames; up; up = up->next) {
		if(!strcmp(up->name, name)) {
			return up->uid;
		}
	}
	return -1;
}

/****************************************************************************
 * Read the user names from passwd.
 ****************************************************************************/
void GetUserNames() {
	UserNameType *last = NULL;
	UserNameType *up;
	FILE *fd;
	int x, ch;

	fd = fopen(_PATH_PASSWD, "r");
	if(!fd) {
		TerminateMessage("could not open passwd");
	}

	for(;;) {

		up = malloc(sizeof(UserNameType));
		if(!up) {
			TerminateMessage("out of memory");
		}
		up->next = NULL;
		if(userNames) {
			last->next = up;
		} else {
			userNames = up;
		}
		last = up;

		for(x = 0; x < USERNAME_SIZE; x++) {
			ch = fgetc(fd);
			if(ch == EOF) {
				return;
			} else if(ch == ':') {
				break;
			}
			up->name[x] = ch;
		}
		up->name[x] = 0;
		do {
			ch = fgetc(fd);
			if(ch == EOF) {
				return;
			}
		} while(ch != ':');
		fscanf(fd, " %lu", &up->uid);
		do {
			ch = fgetc(fd);
			if(ch == EOF) {
				return;
			}
		} while(ch != '\n');
	}

}

