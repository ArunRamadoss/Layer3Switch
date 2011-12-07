#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <termio.h>
#include "defs.h"
#include "cli.h"

#define MAX_TRY 3

int process_logout()
{
	process_login ();
}

void show_login_prompt()
{     
	write_string("************************************************************\n");
	write_string("*       $$      Open Source Switch Solution     $$         *\n");
	write_string("************************************************************");
	process_login ();
}

int process_lock (void)
{
	char pword[MAX_USER_PASSWORD];
	char user[MAX_USER_PASSWORD];

	memset (user, 0, sizeof(user));
	memset(pword,0, sizeof(pword));

	write_string ("\n");

	get_current_user_name (user);
retry:
	write_string("\rEnter Password To UnLock The Console:");
	read_username_password (pword, 0);

	if (validate_username_password (user, pword)  < 0) {
		goto retry;
	}

	write_string ("\n");

	return 0;
}

int process_login (void)
{
	char pword[MAX_USER_NAME];
	char user[MAX_USER_PASSWORD];
	int  u1count = 0;

	memset (user, 0, sizeof(user));

	memset (pword, 0, sizeof(pword));

retry:
	if (u1count == MAX_TRY) {
		process_lock ();
		u1count = 0;
		goto login;
	}
	else {
login:
		write_string ("\nlogin:");
		read_username_password (user, 1);
		write_string ("\nPassword:");
		read_username_password (pword, 0);

		if (validate_username_password (user, pword)  < 0) {
			write_string ("\nIncorrect Login, Please try again");
			u1count++;
			goto retry;
		}
		set_current_user_name (user);
	}
	write_string ("\n");
	return 0;
}

void read_username_password (char *pword, int flag)
{
	int i = 0;
	int ch;
	while ((ch = read_input()) != '\n') {

		if (IS_BACK_SPACE(ch))  {
			if(pword[0] == '\0')
				continue;
			if (i > 0) {
				i--;
				pword[i] = '\0';
				if (flag)
					write_string ("\b \b");
			}
		}
		else {
			pword[i++] = (char)ch;
			if (flag)
				write_input_on_screen (ch);

		}
	}

	pword[i] = '\0';

	return;
}
