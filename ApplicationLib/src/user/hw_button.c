#include "cominc.h"

const BTNHW_T TabBtn[BTN_MAX]=
{
	{BTN1_SHUT,	1},
	{BTN2_DOWN, 3},
	{BTN3_UP,	4},
	{BTN4,		5},
	{BTN5,		8},
};

#define BTN_MASK 0x1f

#define BTN_RCC RCC_APB2Periph_GPIOB
#define BTN_PORT GPIOB
#define BTN_PIN  GPIO_Pin_1|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_8

static uint16_t s_longKey = 0;
static uint16_t trg=0, cont=0, cnt_last=0, cnt_plus = 0;

BTNKEY_t s_btnKey;

static void BTN_KeyBuffInit(void)
{
	s_btnKey.in=0;
	s_btnKey.out=0;
}

uint16_t BTN_KeyPop(void)
{
	uint16_t tValue;

	if (s_btnKey.out != s_btnKey.in)
	{
		s_btnKey.out++;
		if (s_btnKey.out >= KeyBuffLen)
		{
			s_btnKey.out = 0;
		}
		tValue = s_btnKey.buff[s_btnKey.out];
	}
	else
	{
		tValue = BTN_KEY_NULL;
	}
	return tValue;
}

bool BTN_KeyPush(uint16_t keyValue)
{
	if ((s_btnKey.in+1)%KeyBuffLen == s_btnKey.out)
	{
		return false;
	}

	if ( keyValue != BTN_KEY_NULL)
	{
		s_btnKey.in++;
		if (s_btnKey.in >= KeyBuffLen)
		{
			s_btnKey.in = 0;
		}
		s_btnKey.buff[s_btnKey.in]=keyValue;

		//MenuParam.beepFlag = 1;
		//MenuParam.bgledFlag = 1;//light and beep

		return true;		
	}
	else
		return false;
}

void BTN_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = BTN_PIN;
	GPIO_Init(BTN_PORT, &GPIO_InitStructure);
	BTN_KeyBuffInit();
}

void BTN_create(void)
{
	BTN_KeyBuffInit();
}

void BTN_destory(void)
{
	s_longKey = 0;
	cnt_plus = 0;
	cont = 0;
}
static uint16_t inputConvert(uint16_t hwValue)
{
	uint8_t i;
	uint16_t sfValue=0;
	for (i = 0; i < BTN_MAX; i++)
	{
		sfValue |= ((hwValue >> TabBtn[i].hwPin) & 0x01) << TabBtn[i].sfPin;
	}

	return sfValue;
}

static void setLongKey(uint16_t longKey)
{
	s_longKey = longKey;
}

uint16_t BTN_getLongKey(void)
{
	return s_longKey;
}

uint16_t BTN_softProcess(uint16_t hwValue)
{

	uint16_t tvalue;
	uint16_t sfValue= inputConvert(hwValue);
	
	/**
	unsigned char ReadData = PINB^0xff;   // 1
	Trg = ReadData & (ReadData ^ Cont);   // 2
	Cont = ReadData;                      // 3
	***/
	//0µ¼Í¨£¬ 1¶Ï¿ª
	cnt_last = cont;
	tvalue = sfValue ^ BTN_MASK;
	trg = tvalue & (tvalue ^ cont);
	cont = tvalue;

	if (cnt_plus > 20 )
	{
		//run 21 + 1 times
		setLongKey((cnt_last & BTN_MASK) | 0x1000);
		cnt_plus = 0;
		//return long key
		return (cnt_last & BTN_MASK) | 0x1000;
	}
	//check long key 
	if (cnt_last == cont && cont!= 0)
		cnt_plus++;
	else
		cnt_plus = 0;

	//return short key
	return trg & BTN_MASK;
}

uint16_t BTN_getInput(void)
{
	return GPIO_ReadInputData(BTN_PORT) & BTN_PIN;
	//return BTN_PORT.ODR;
}
void BTN_taskCheckKey(void)
{
	uint16_t key = BTN_softProcess(BTN_getInput());
	if (key != BTN_MASK && key != 0)
	{
		BTN_KeyPush(key);
	}
}

