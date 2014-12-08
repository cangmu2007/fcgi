#ifndef __HEAD_H__
#define __HEAD_H__

#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/time.h>
#include "cgi2srv.h"
#include <fcgi_stdio.h>
#include <fcgiapp.h>

//主要用于区分请求方式并调用相应的处理函数，最后释放空间
void* cgi_task(void* arg);

//GET请求，不解释
CM get_request(FCGX_Request* arg);

//POST/PUT请求，也不解释
CM post_request(FCGX_Request* arg);

//和服务端交换数据
int data_exchange(CM cm,FCGX_Stream* out);

#endif