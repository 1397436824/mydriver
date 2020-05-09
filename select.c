#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <poll.h>
#include <sys/epoll.h>
#include <string.h>


#define MEM_CLEAR 26368
#define BUFFER_LEN 20


struct timeval tm = {
	.tv_sec = 2,
	.tv_usec = 0,
};

struct pollfd pd;
//{
//               fd;         /* file descriptor */
//               short events;     /* requested events */
//               short revents;    /* returned events */
//};

int main(void)
{
        int fd;
        int efd;
        int ret;

        fd = open("/dev/globalmem",O_RDONLY|O_NONBLOCK);
        if(fd != -1)
	{
//                if (ioctl(fd, MEM_CLEAR, 0))
                if(fcntl(fd,F_SETOWN,getpid()))
                        printf("ioctl command failed\n");
                pd.fd = fd;
                pd.events |= POLLIN;
                pd.revents = 0;
        	efd = epoll_create(1);
        	if(efd < 0)
        	{
        	        printf("epoll_create failed\n"); 
        	}
		struct epoll_event event;
		memset(&event,0,sizeof(event));	
		event.events |= EPOLLIN;
		struct epoll_event event1;
		memset(&event1,0,sizeof(event1));	

		if(epoll_ctl(efd,EPOLL_CTL_ADD,fd,&event))
		{
			perror("epoll_ctl is error");
			return -1;
		}
	        while (1) 
		{
			//printf("event.events = %d\n",event.events);
			printf("%d:event.events = %d\n",__LINE__,event.events);
			ret = epoll_wait(efd,&event1,1,2000);
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
				//printf("FIFO is not empty\n");				
				printf("%d:event1.events = %d\n",__LINE__,event1.events);
			//	memset(&event1,0,sizeof(event1));	
                		if (ioctl(fd, MEM_CLEAR, 0))
		                        printf("ioctl command failed\n");
				event1.events = 0;
			}	
        	}
		close(fd);
		close(efd);
        }
       	else 
	{
                printf("Device open failure\n");
	}
        return 0;
}
