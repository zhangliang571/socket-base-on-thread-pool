
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <cstring>
#include <ctime>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>

#include "thread_pool.h"

using namespace std;

int conn(const char *sip, int sport)
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

    if(connect(fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        cout<<("connect err\n");
        return -1;
    }
    return fd;

}
void client_handle(void *args)
{
	const int sport = 20000;
	const char *sip = "192.168.171.133";
	char buf[1024]={0};
	char rbuf[1024]={0};
	int fd = -1;
	int ret  = 0;

	fd = conn(sip, sport);
	if(fd <= 0)
		return;

	memset(buf, 'c', 15);
	cout<<pthread_self()<<" client thread running "<<fd<<endl;
	for(;;)
	{
		memset(rbuf, 0, sizeof(rbuf));

		ret = send(fd,buf,strlen(buf),0);
		cout<<fd<<" send:"<<ret<<endl;
		ret = recv(fd, rbuf, sizeof(rbuf), 0);
		sleep(2);
		cout<<fd<<" len:"<<ret<<" recv:"<<rbuf<<endl;

	}
	close(fd);

}
int main(int argc, char *argv[])
{
	int ret  = 0;
	int i = 0;
	int n = 20;

	threadPool *tpool = new threadPool(n);
	sleep(1);

	for(i=0;i<n;i++)
	tpool->thread_add(client_handle, NULL);
	
	while(1)
		sleep(1);
}
