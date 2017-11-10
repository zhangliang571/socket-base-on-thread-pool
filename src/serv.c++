
#include <iostream>
#include <string>
#include <vector>

#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <pthread.h>

#include "thread_pool.h"

using namespace std;


int listen_fd(const char *sip, int sport)
{
    int fd = -1;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd <= 0)
    {
        cout<<("socket err\n");
        return -1;
    }

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(sport);
    servaddr.sin_addr.s_addr = inet_addr(sip);

    if(bind(fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        cout<<("bind err\n");
        return -1;
    }
	if(listen(fd, 10) < 0)
		return -1;

    return fd;

}
void server_handle(void *args)
{
	int fd = 0;
	int ret = 0;
	char buf[1024];
	if(args == NULL)
		return;
	fd = *((int*)args);
	cout<<pthread_self()<<" server thread running "<<fd<<endl;
	for(;;)
	{
		memset(buf, 0, sizeof(buf));
		ret = recv(fd, buf, sizeof(buf), 0);
		if(ret <= 0)
		break;
		cout<<fd<<" len:"<<ret<<" recv:"<<buf<<endl;
		ret = send(fd, buf, strlen(buf), 0);
		cout<<fd<<" write :"<<ret<<endl;
	}

	cout<<pthread_self()<<" server thread overoverover"<<fd<<endl;
	close(fd);
}
int main(int argc, char *argv[])
{
	const int sport = 20000;
	const char *sip = "192.168.171.133";
	char buf[100]={0};
	char rbuf[1024]={0};
	int fd = -1;
	int newfd = -1;
	int i,ret;
	for(i=0;i<20;i++)
		buf[i] = 's';

	threadPool *tpool = new threadPool(10);
	sleep(1);

	fd = listen_fd(sip, sport);
	if(fd <= 0)
		return -1;

	while(1)
	{
		if((newfd=accept(fd, NULL, NULL))< 0)
			cout<<("accept err\n");
		else
			cout<<"accept newfd:"<<newfd<<endl;

		tpool->thread_add(server_handle, &newfd);
	}

	close(fd);
	
}
