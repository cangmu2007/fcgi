#ifndef HEAD_H__
#define HEAD_H__

#include <stdint.h>

#define UNIX_PATH "/home/mu/apache/cgi-bin/cgi_link"	//���׽������ӷ�·��(�����˵�ͬ����·��һ��,����������CGIĿ¼��)

typedef struct cgi_msg
{
	uint32_t packet_len;
	uint32_t type;
	char sender[32];
	char recver[32];
	uint32_t len;
	char context[0];
}CGI_MSG,*CM;

#endif
