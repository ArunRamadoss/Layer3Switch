#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/types.h>      

char   mq_vmgr_name[] = "/VMGR";

int main (int argc, char **argv)
{
	char msg[1512];
	int size = 1000, i = 0;
	mqd_t mq_vmgr_id = 0;
	int inst = 0;
	char   mq_myname[32];
	int mqid = 0, prio = 0;

	struct mq_attr attr;

	mq_vmgr_id =  mq_open (mq_vmgr_name, O_WRONLY);

	memset (msg, 1, sizeof(msg));

	if (mq_vmgr_id == (mqd_t)-1) {
		perror ("mq_open: ");
		exit (1);
	}

	inst = atoi (argv[1]);

	sprintf (mq_myname, "%s%d","/INST",inst);


#define MAX_FRAMES_IN_Q  32

	memset (&attr, 0, sizeof(attr));

	attr.mq_flags = 0;
	attr.mq_maxmsg = MAX_FRAMES_IN_Q;
	attr.mq_msgsize = 1512;

	mqid =  mq_open (mq_myname, O_RDWR | O_CREAT,  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH , &attr);

	if (mqid == (mqd_t)-1) {
		perror ("mq_open: ");
		exit (1);
	}


	while (i < 100) {
		if (mq_send(mq_vmgr_id, msg, size, 0) < 0) {
			perror("MQ_SEND: ");
		}
		size += (i * 10);
		sleep (1);
		if (size > 1512)
			size = 1000;
		i++;
		printf ("Msg send\n");
		if (mq_receive(mqid , msg, 1512, &prio) < 0) 
			continue;
		printf ("PKT RCVD FROM inst %d\n", msg[0]);
		msg[0] = (char)inst;
		msg[1] = 1;
	}
}
