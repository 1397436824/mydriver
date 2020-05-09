#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#define MEM_CLEAR 26368	 //memory clear cmd


#define PATH "/dev/globalmem"

int main(int argc,char *argv[])
{
        int fd;
        int ret = 0;
        char buf[128] = {0};
	char rece[128] = {0};
        fd = open(PATH,O_RDWR);
        if(fd < 0)
        {
                perror("open is failed!");
                return fd;
        }
	sprintf(buf,"驱动你好呀");
	ret = write(fd,buf,strlen(buf));
	if(ret < 0)
	{
               	perror("write is failed!");
	        return ret;
	}
	printf("write %d numbers:%s\n",ret,buf);
	
	ret = lseek(fd,0,SEEK_SET);
	if(ret < 0)
	{
		perror("lseek is failed");
		return ret;
	}

	ret = read(fd,rece,128);
	if(ret < 0)
	{
                perror("read is failed!");
                return ret;
	}
	printf("read %d numbers:%s\n",ret,rece);
	
	ret = ioctl(fd,MEM_CLEAR);
	if(ret < 0)
	{
		perror("ioctl is failed\n");
		return ret;
	}

	ret = lseek(fd,0,SEEK_SET);
	if(ret < 0)
	{
		perror("lseek is failed");
		return ret;
	}

	ret = read(fd,rece,128);
	if(ret < 0)
	{
                perror("read is failed!");
                return ret;
	}
	printf("read %d numbers:%s\n",ret,rece);
	close(fd);
        return 0;
}


