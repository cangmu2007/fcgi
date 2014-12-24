#include "cgi_task.h"

void* cgi_task(void* arg)
{
	FCGX_Request* request=(FCGX_Request*)arg;
	CM cm=NULL;

	//判断请求方式
	char* request_method=FCGX_GetParam("REQUEST_METHOD", request->envp);
	if((0==strcmp(request_method, "POST"))||(0==strcmp(request_method, "PUT")))
	{
		cm=post_request(request);
	}
	else if(0==strcmp(request_method, "GET"))
	{
		cm=get_request(request);
	}
	
	//交换数据
	if(NULL!=cm)
	{

		//设备判断
		char* type=FCGX_GetParam("HTTP_USER_AGENT",request->envp);
		if(0==strcmp(type,"Android"))
		{
			cm->dev_type=ANDROID;
		}
		else if(0==strcmp(type,"iOS"))
		{
			cm->dev_type=IOS;
		}
		else if(0==strcmp(type,"Windows Phone"))
		{
			cm->dev_type=WP;
		}
		else
		{
			cm->dev_type=IGNORE;
		}

		if(data_exchange(cm,request->out)<0)
		{
			FCGX_FPrintF(request->out, "ERROR!");
		}
		free(cm);
		cm=NULL;
	}
	
	//擦屁股
	FCGX_Finish_r(request);
	FCGX_Free(request,1);
	if(NULL!=request)
	{
		free(request);
		request=NULL;
	}
	return NULL;
}

int data_exchange(CM cm,FCGX_Stream* out)
{
	//1、将结构体send发送至服务端
	int fd=-1,ret=-1;
	struct sockaddr_un srv_addr;

	if((fd = socket(PF_UNIX, SOCK_STREAM, 0))<0)
	{
		return -1;
	}

	memset(&srv_addr,0,sizeof(struct sockaddr_un));
	srv_addr.sun_family = PF_UNIX;
	strcpy(srv_addr.sun_path,UNIX_PATH);

	if(connect(fd, (struct sockaddr *)&srv_addr, sizeof(struct sockaddr_un))<0)
	{
		shutdown(fd, SHUT_RDWR);
		close(fd);
		return -1;
	}

	if((ret=send(fd,(char*)cm,cm->packet_len,MSG_NOSIGNAL))<0)
	{
		shutdown(fd, SHUT_RDWR);
		close(fd);
		return -1;
	}

	//设置延时
	struct timeval timeout;
	timeout.tv_sec=270;   
	timeout.tv_usec=0;
	if(setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout.tv_sec,sizeof(struct timeval))<0)
	{
		shutdown(fd, SHUT_RDWR);
		close(fd);
		return -1;
	}

	//2、阻塞接收数据
	int size=-1;
	ret=recv(fd,(char*)&size,sizeof(size),0);
	if(ret<0)
	{
		close(fd);
		return -1;
	}
	if(size<=0)
	{		
		shutdown(fd, SHUT_RDWR);
		close(fd);
		return -1;
	}
	uint leftsize=sizeof(char) * size;
	char* ndata=(char*)malloc(leftsize);
	if(NULL==ndata)
	{
		shutdown(fd, SHUT_RDWR);
		close(fd);
		return -1;
	}
	memset(ndata,0,leftsize);

	uint recv_count=0;
	while(recv_count<leftsize)
	{
		ret=recv(fd,ndata+recv_count,leftsize-recv_count,0);
		if(ret==-1)
		{
			free(ndata);
			ndata=NULL;
			shutdown(fd, SHUT_RDWR);
			close(fd);
			return -1;
		}else if(ret==0)
		{
			break;
		}else
			recv_count+=ret;
	}
	if(recv_count>0)
	{
		FCGX_PutStr(ndata, recv_count, out);
	}else
		FCGX_FPrintF(out, "ERROR!");

	free(ndata);
	ndata=NULL;
	shutdown(fd, SHUT_RDWR);
	close(fd);
	return 0;
}

CM post_request(FCGX_Request* arg)
{
	FCGX_Request* request=arg;
	char *lenmsg = FCGX_GetParam("CONTENT_LENGTH",request->envp);
	uint plen=0;
	if((lenmsg == NULL) || ((plen = atoi(lenmsg)) <= 0))
	{
		return NULL;
	}

	char* data = (char*)malloc(plen+1);
	if(NULL==data)
	{
		return NULL;
	}
	FCGX_GetStr(data,plen,request->in);
	data[plen] = 0;
	FCGX_FFlush(request->in);

	char* ptr = strstr(data, "context=");
	uint len=plen-(ptr-data)-7;

	CM cm=(CM)malloc(sizeof(CGI_MSG)+len);
	if(NULL==cm)
	{
		return NULL;
	}
	memset(cm,0,sizeof(CGI_MSG)+len);
	cm->packet_len=sizeof(CGI_MSG)+len;
	cm->len=len-1;

	if(sscanf(data, "%*[^=]=%d%*[^=]=%[^&]%*[^=]=%[^&]", &cm->type, cm->sender, cm->recver)!= 3)
	{
		free(cm);
		cm=NULL;
		return NULL;
	}

	memcpy(cm->context,data+(plen-len+1),len-1);
	free(data);

	return cm;
}

CM get_request(FCGX_Request* arg)
{
	FCGX_Request* request=arg;

	char* data=FCGX_GetParam("QUERY_STRING", request->envp);
	char* ptr = strstr(data, "context=");
	uint len=strlen(data)-(ptr-data)-7;
	CM cm=(CM)malloc(sizeof(CGI_MSG)+len);
	if(NULL==cm)
	{
		return NULL;
	}

	memset(cm,0,sizeof(CGI_MSG)+len);
	cm->packet_len=sizeof(CGI_MSG)+len;
	cm->len=len-1;

	if(sscanf(data, "%*[^=]=%d%*[^=]=%[^&]%*[^=]=%[^&]%*[^=]=%s", &cm->type, cm->sender, cm->recver, cm->context)!= 4)
	{
		free(cm);
		cm=NULL;
		return NULL;
	}

	return cm;
}
