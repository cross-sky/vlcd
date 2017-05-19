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
	//��2�飺���2λ����ָ����ռʽ���ȼ�-PreemptionPriority�����2λ����ָ����Ӧ���ȼ�-SubPriority
	//���顱���ȼ���>������ռ���ȼ���>���������ȼ���
	//��ֵԽС����������ȼ���Խ��,�б�Ҫ������0
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	UB_LED_Init();		//PC1,PC3,PC13  ok	
	SystickInit();

	GLCD_init();
	BTN_init();
	Uart1_init();
	Menu_initPara();
	//@@@@@@@@@��ʾ��Ҫ��ʱһ�������������Ҳû��ϵ���������ط�
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



