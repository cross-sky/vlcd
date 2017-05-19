#include "cominc.h"

//add task
//1.add init 2.add task.h->task 3.add taskcomps

TPC_TASK TaskComps[]=
{
	{0, 0, 301,  301, LED1on},			// 5*200=1s   ok	
	{0, 0, 101, 101, GLCD_refresh},		//lcd refrech
	{0, 0, 15,  15, BTN_taskCheckKey},  //btn task
	{0, 0, 15,  15, Menu_process},		//menu
	{0, 0, 100,  100, Uart1_taskProces},		//uart1 Uart1_sendTest
//	{0, 0, 500,  500, Uart1_sendCodeA},
};



void TaskInit(void)
{
	//第2组：最高2位用于指定抢占式优先级-PreemptionPriority，最低2位用于指定响应优先级-SubPriority
	//“组”优先级别>“抢”占优先级别>“副”优先级别
	//数值越小所代表的优先级就越高,有必要设置组0
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	UB_LED_Init();		//PC1,PC3,PC13  ok	
	SystickInit();

	GLCD_init();
	BTN_init();
	Uart1_init();
	Menu_initPara();
	//@@@@@@@@@显示板要延时一点才启动，不过也没关系，后面有重发
}



void TPCRemarks( TPC_TASK *pTask)
{
	u8 i;
	
	for ( i=0; i<TASKS_MAX; i++)
	{
		if( pTask[i].Timer > 0 && pTask[i].Suspend == 0)
		{
			pTask[i].Timer--;
			if (pTask[i].Timer == 0)
			{
				pTask[i].Timer = pTask[i].ItvTime;
				pTask[i].Run = TPC_RUN_STM; 
			}
		}
	}
}

void TPCProcess(TPC_TASK *pTask)
{
	u8 i;

	for (i=0; i<TASKS_MAX; i++)
	{
		if (pTask[i].Run == TPC_RUN_STM && pTask[i].Suspend == 0)
		{
			pTask[i].TaskHook();
			pTask[i].Run = TPC_RUN_CLM;
		}
	}
}



