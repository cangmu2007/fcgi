#include "CThread.h"
#include "cgi_task.h"

#define THREAD_DEAL 20	//处理用户业务线程

static void* doit(void* a)  
{
	int rc;

    for (;;)  
    { 
		FCGX_Request* fr=(FCGX_Request*)malloc(sizeof(FCGX_Request));
		if(NULL==fr)
		{
			break;
		}
		memset(fr,0,sizeof(FCGX_Request));

		FCGX_InitRequest(fr, 0, 0);

        rc = FCGX_Accept_r(fr);
  
        if (rc < 0)
		{
			FCGX_Free(fr,1);
			if(NULL!=fr)
			{
				free(fr);
				fr=NULL;
			}
            break;
		}
		FCGX_FPrintF(fr->out,"\n");
		
		pool_add_worker(cgi_task,(void*)fr);	//给线程池添加任务
    }
	return NULL;
}

int main(void)
{
	pool_init(THREAD_DEAL);

	FCGX_Init();
	doit(0);
	
	pool_destroy();
	return 0;
}
