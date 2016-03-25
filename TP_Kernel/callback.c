#include <stdio.h>
#include <sched.h>
#include <unistd.h>	
#include <errno.h>
		
int main(int argc, char* argv[])
{
	struct sched_param param;
	int res;
	
	res=sched_setscheduler(getpid(), SCHED_FIFO , &param);
	printf("res: %d, errno %d\n", res, errno);
	res=sched_setscheduler(getpid(), SCHED_RR, &param);
	printf("res: %d, errno %d\n", res, errno);
	res=sched_setscheduler(getpid(), SCHED_FIFO , &param);
	printf("res: %d, errno %d\n", res, errno);
	
	return 0;
}
