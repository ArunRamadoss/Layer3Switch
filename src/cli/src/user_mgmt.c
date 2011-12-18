#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <termio.h>
#include "defs.h"


struct user_db {
	char user_name[MAX_USER_NAME];
	char password[MAX_USER_PASSWORD];
	int  priv_level;
	int  status;
};

static struct  user_db userdb[MAX_USERS];

int user_db_init (void)
{
	if (create_user ("guest", "Guest1", 5)  < 0) {
		write_string ("Default User \"guest\" creation failed\n");
		return -1;
	}
	if (create_user ("admin", "Admin123", 0) < 0) {
		write_string ("Default User \"admin\" creation failed\n");
		return -1;
	}
	return 0;

}

struct user_db * get_user_info (char *username)
{
	int i = MAX_USERS;

	if (!username || !username[0]) 
		return NULL;

	if (!strlen (username))
		return NULL;

	while (i--) {
		if (userdb[i].status && 
		    !strcmp (userdb[i].user_name, username))
				return &userdb[i];
	}
	return NULL;
}

int validate_username_password (char *user, char *passwd)
{
	struct user_db * p = get_user_info (user);

	if (!p)
		return -1;

	if (strcmp (p->password, passwd))
		return -1;
	return 0;
}

int create_user (char *username, char *password, int priv_level)
{

	int i = MAX_USERS;

	if (!username || !password)
		return -1;

	if (password_validation (password) < 0) {
		return -1;
	}

	if (priv_level < 0 || priv_level > 5)
		return -1;

	if (!update_user_info (username, password, priv_level))
		return -1;

	while (i--) {
		if (!userdb[i].status) {
			strncpy (userdb[i].user_name, username, MAX_USER_NAME);
			strncpy (userdb[i].password, password, MAX_USER_PASSWORD);
			if (priv_level >= 0)
				userdb[i].priv_level = priv_level;
			else 
				userdb[i].priv_level = 5;
			userdb[i].status  = 1;
			return 0;
		}
	}
	
	return -1;
}

int update_user_info (char *username, char *password, int priv_level)
{

	struct user_db * p = NULL;

	if (!(p = get_user_info (username)))
		return -1;

	if (priv_level >= 0) {
		if (p->priv_level != priv_level)
			p->priv_level = priv_level;
	}

	strncpy (p->password, password, MAX_USER_PASSWORD);

	return 0;
}

int password_validation (char *pswd)
{
	char is_u = 0;
	char is_l = 0;
	char is_d = 0;
	char c = 0;

	if (!pswd || !pswd[0])
		return -1;

	do {
		c = *pswd;
		if (isspace (c))
			return -1;
		if (!is_u && isupper (c))
			is_u = 1;
		else if (!is_l && islower (c))
			is_l = 1;
		else if (!is_d && isdigit (c))
			is_d = 1;
		pswd++;

	} while (*pswd);

	if (is_d && is_l && is_u)
		return 0;
	return -1;

}

int user_del (char *username)
{
	struct user_db *del = NULL;

	if (!username)
		return -1;

	if (!(del = get_user_info (username))) {
		return -1;
	}

	memset (del, 0, sizeof(struct user_db));

	return 0;
} 


char * encrypt_password (char *password)
{

}

char * decrypt_password (char *password)
{
}

int show_users (void)
{
	int  i = -1;
	printf (" %-16s   %-16s    %-16s\n","Username","Password","Level");
	printf (" %-16s   %-16s    %-16s\n","--------","--------", "--------");
	while (++i < MAX_USERS) {
		if (userdb[i].status)
			printf (" %-16s   %-16s    %4d\n",
				userdb[i].user_name, "******", userdb[i].priv_level);
	}
	return 0;
}
