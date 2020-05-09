#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <poll.h>
#include <sys/epoll.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int fd;
int efd;

static void signalio_handler(int signum)
{
	char buf[128] = {0};
	int ret = 0;
        struct epoll_event event1;
        memset(&event1,0,sizeof(event1));
	printf("receive a signal from globalfifo,signalnum:%d\n" ,signum);
	ret = epoll_wait(efd,&event1,1,0);
	if(ret < 0)
	{
		perror("epoll_wait\n");
	}
	else if(ret == 0)
	{
		printf("time out\n");
	}
	else
	{ 
		if(event1.events & POLL_IN)
		{		
			ret = read(fd,buf,128);
			if(ret < 0)
			{
				printf("read is error\n");
			}
			printf("read data:%s\n",buf);
			memset(buf,0x00,sizeof(buf));
		}
		memset(&event1,0,sizeof(event1));       
		event1.events = 0;
        }
}

int main(void)
{
	int oflags;
        struct epoll_event event;
        memset(&event,0,sizeof(event));
	event.events |= EPOLLIN;


	fd = open("/dev/globalmem", O_RDWR|S_IRUSR |S_IWUSR);
	if(fd != -1) 
	{
		efd = epoll_create(1);
        	if(efd < 0)
        	{
        	        printf("epoll_create failed\n");
		}
		if(epoll_ctl(efd,EPOLL_CTL_ADD,fd,&event))
        	{
        	        perror("epoll_ctl is error");
        	        return -1;
        	}
		signal(SIGIO, signalio_handler);
		fcntl(fd,F_SETOWN,getpid());
		oflags = fcntl(fd, F_GETFL);
		fcntl(fd, F_SETFL, oflags | FASYNC);
		while(1)
		{
			sleep(100);	
		}
	}
	else
	{
		printf("device open failure!\n");
	}		      
	return 0;
}


