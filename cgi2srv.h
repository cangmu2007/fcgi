#ifndef HEAD_H__
#define HEAD_H__

#include <stdint.h>

#define UNIX_PATH "/home/1/fcgi/cgi_link"	//域套接字连接符路径(与服务端的同名宏路径一致,尽量创建在CGI目录下)

enum DEV_TYPE
{
	IGNORE=0x0,
	ANDROID,
	IOS,
	WP,
};

typedef struct cgi_msg
{
	uint32_t packet_len;
	uint32_t type;
	char sender[48];
	char recver[48];
	uint32_t dev_type;
	uint32_t len;
	char context[0];
}CGI_MSG,*CM;

#endif
