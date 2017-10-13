#include "MicroBit.h"

MicroBit uBit;

/******************ָ���**********************/
#define SYS_DIS 0x00                //�ر�ϵͳ��
#define SYS_EN  0x01                //��ϵͳ��
#define LED_OFF 0x02                //�ر�LED��ʾ
#define LED_ON  0x03                //��LED��ʾ
#define BLINK_OFF   0x08            //�ر���˸
#define BLINK_ON    0x09            //����˸                             
#define SLAVE_MODE  0x10            //��ģʽ
#define RC_MASTER_MODE  0x18        //�ڲ�RCʱ��
#define COM_OPTION  0x24            //24:16COM��NMOS��C��COM��PMOS��
#define PWM_DUTY    0xAF            //PWM���ȿ���
/****************���Ŷ���**********************/

DigitalOut HT_CS(P0_16);
DigitalOut HT_RD(P0_22);
DigitalOut HT_WR(P0_23);
DigitalOut HT_DAT(P0_21);

static unsigned char show[24] = {0b00011111, 0b10000000, 0b00100000, 0b01000000, 0b01001011, 0b00100000, 0b10000000, 0b00010000, 0b10100010, 0b01010000, 0b10101110, 0b00010000, 0b10000111, 0b01010000, 0b10100100, 0b01010000, 0b10000000, 0b00010000, 0b01001101, 0b00100000, 0b00100000, 0b01000000, 0b00011111, 0b10000000};

static unsigned char com[12] = {0x00,0x04,0x08,0x0C,0x10,0x14,0x18,0x1C,0x20,0x24,0x28,0x2C};

void HT1632C_Write(unsigned char Data,unsigned char cnt)      //MCU��HT1632Cд���ݺ�������λ��ǰ
{
    unsigned char i;
    for(i=0; i<cnt; i++) {
        HT_WR=0;
        if(Data&0x80)
            HT_DAT=1;
        else
            HT_DAT=0;
        Data<<=1;
        HT_WR=1;
    }
}
void HT1632C_Write_CMD(unsigned char cmd)                     //MCU��HT1632cд����
{
    HT_CS=0;
    HT1632C_Write(0x80,3);                                    //ID:100
    HT1632C_Write(cmd,9);                                     //��������λ����λΪ
    HT_CS=1;
}

void HT1632C_Write_DAT(unsigned char Addr,unsigned char data[],unsigned char num)
{
    unsigned char i;
    HT_CS=0;
    HT1632C_Write(0xa0,3);                                    //ID:101
    HT1632C_Write(Addr<<1,7);

    for(i=0; i<8; i++) {
        HT_WR=0;
        if(data[2*(num-1)]&0x80)
            HT_DAT=1;
        else
            HT_DAT=0;
        data[2*(num-1)]<<=1;
        HT_WR=1;
    }

    for(i=0; i<4; i++) {
        HT_WR=0;
        if(data[2*num-1]&0x80)
            HT_DAT=1;
        else
            HT_DAT=0;
        data[2*num-1]<<=1;
        HT_WR=1;
    }

    HT_CS=1;
}

void HT1632C_clr(void)  //��������
{
    unsigned char i;
    HT_CS=0;
    HT1632C_Write(0xa0,3);
    HT1632C_Write(0x00,7);
    for(i=0; i<48; i++)
        HT1632C_Write(0,8);
    HT_CS=1;
}
void HT1632C_Init(void)     //HT1632C��ʼ������
{
    HT_CS=1;
    HT_WR=1;
    HT_DAT=1;
    HT1632C_Write_CMD(SYS_DIS);         //�ر�HT1632C�ڲ�RCʱ��
    HT1632C_Write_CMD(COM_OPTION);      //ѡ��HT1632C����ģʽ��COM��NMOSģʽ
    HT1632C_Write_CMD(RC_MASTER_MODE);  //ѡ���ڲ�RCʱ��
    HT1632C_Write_CMD(SYS_EN);          //��HT1632C�ڲ�RCʱ��
    HT1632C_Write_CMD(PWM_DUTY);        //PWM��������
    HT1632C_Write_CMD(BLINK_OFF);       //�ر���˸
    HT1632C_Write_CMD(LED_ON);          //��LED��ʾ
}


void HT1632C_Read_DATA(unsigned char Addr)
{
    unsigned char i;
    HT_CS=0;
    HT1632C_Write(0xc0,3);                                    //ID:101
    HT1632C_Write(Addr<<1,7);
    for(i=0; i<12; i++) {
        HT_RD=0;
        asm("nop");
        HT_RD=1;
    }
    HT_CS=1;
}

int main()
{
    HT1632C_Init();
    HT1632C_clr();
    uBit.init();
    for(int i=0; i<12; i++) {
        HT1632C_Write_DAT(com[i],show,i+1);
    }
    wait(0.5);
    while(1) {
        for(int i=0; i<12; i++) {
            HT1632C_Read_DATA(com[i]);
        }
    }

}

