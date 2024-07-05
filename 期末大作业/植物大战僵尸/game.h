#pragma once
#include<iostream>
using namespace std;
#include<graphics.h>
#include"tools.h"
#include<ctime>
#include<cmath>
#include"vector2.h"	//�������߰�
#include<stdio.h>
#include<mmsystem.h>	//����
#include<fstream> //�ļ�
#include <setjmp.h>//���ں�������ת���ܵ�ʵ�� ����һ����Ϸ�˳���ת�����˵�
#include <Windows.h>
#include<vector>

extern jmp_buf jmpbuffer;
extern jmp_buf jmpbuffer_two;
//extern jmp_buf jmpbuffer_three;
#pragma comment(lib,"winmm.lib") 

enum { WAN_DAO, XIANG_RI_KUI, SHI_REN_HUA,CHERRY,WALL_NUT, ZHI_WU_COUNT };	//ֲ��ö��
enum { SUNSHINE_DOWN, SUNSHINE_GROUND, SUNSHINE_COLLECT, SUNSHINE_PRODUCT };  //������״̬ö��
enum { GOING, WIN, FAIL };

extern IMAGE imgBg;	//��Ϸ����ͼ
extern IMAGE imgBar;	//״̬������ֲ��ı�����
extern IMAGE imgCards[ZHI_WU_COUNT];	//ֲ�￨������
extern IMAGE* imgZhiWu[ZHI_WU_COUNT][20];	//ֲ������ 
extern IMAGE imgpause_one;
extern IMAGE imgpause_two;
extern IMAGE *imgpause[9];//�ܹ�9��ͼƬ��͵������ע��ÿ��Ԫ�ض�Ӧ����ͼƬ�ɣ�ʹ�����ַ�����ȾĹ��ͼƬ����ʳ�ͻ
extern IMAGE imgtombstone;
extern IMAGE *imgChomperAttack[18];//ʳ�˻��Խ�ʬ18֡ͼƬ
extern IMAGE *imgChomperDigest[18];//ʳ�˻��Խ�ʬ18֡ͼƬ


extern int curX, curY;	//��ǰѡ��ֲ�����ƶ��е�����
extern int curZhiWu;	//��ǰѡ�е�ֲ��	0-û��ѡ�У�1-ѡ�е�һ��ֲ��
extern int killZmCount;	//ɱ���Ľ�ʬ����
extern int zmCount;		//���ɵĽ�ʬ����
extern int gameStatus;		//��Ϸ��״̬
//int flag = 0;//��ͣ��ť��״̬�����Ż�

//�궨����Ϸ���ڴ�С
#define	WIN_WIDTH 900
#define	WIN_HEIGHT 600

#define ZM_MAX 10	//��ʬ����

struct zhiWu {	//ֲ��ṹ��
	int type;		//0-û��ֲ�1-��һ��ֲ��
	int frameIndex;	//����֡�����
	int shootTimer;	//ֲ�﹥��ʱ����
	bool catched;	//ֲ���Ƿ񱻽�ʬ����
	int deadTimer;	//ֲ�ﱻ��ʱ����������ʱ

	int x, y;
	int timer;	//�������տ���������ļ�ʱ��
	bool eating;//ֲ��Խ�ʬ
	bool digest;//ֲ�������״̬
	int digest_timer;//�����ļ�ʱ��
	int catchzm;//ֲ�ﲶ׽���Ľ�ʬ������ ���Ϊ�������ȶ���Ӧ����ôд�ģ��ⲿ�޸ľ��������Խ�磬Ұָ��ȣ�����δ������Ϊ
};

struct sunShineBall {	//������ṹ��
	int	x, y;//�������x��y����
	int	frameIndex;	//����������֡�����
	int	destY;	//������ֹͣ��y����
	bool used;	//�������Ƿ���ʹ��
	int timer;	//��ʱ����������������������ͣ��ʱ��
	int xoff;	//�������λ��x����
	int yoff;	//�������λ��y����

	//�Ż�
	float t;	//���������ߵ�ʱ���	0��1
	vector2 p1, p2, p3, p4;
	vector2	pCur;	//��ǰʱ���������λ��
	float	speed;
	int	status;	//�������״̬
};

extern struct sunShineBall	balls[10];	//������أ��������ȴ洢����
extern IMAGE	imgSunShineBall[29];	//��������֡����	-	���Զ���һ���꣬������ڹ���

struct zm {		//��ʬ�ṹ��	-���ڻ�����Ҫ��ֲ��һ�����ö��,���㴴����ͬ���͵Ľ�ʬ
	int x, y;
	int row;
	int frameIndex;
	bool used;
	int speed;	//��ʬǰ���ٶ�
	int blood;	//��ʬѪ��
	bool dead;	//��ʬ�Ƿ�����
	bool eating;	//��ʬ�Ƿ��ڳ�ֲ��
	//bool boom;
};

extern struct zm zms[10];	//��ʬ��,�������ȴ洢��ʬ
extern IMAGE	imgZm[22];
extern IMAGE	imgZmDead[20];
extern IMAGE	imgZmEat[21];
extern IMAGE	imgZmStand[11];

struct bullet {	//�㶹�ӵ��ṹ��
	int x, y, row, speed;
	bool used;
	bool blast;	//�Ƿ�ը
	int frameIndex;	//��ը֡���
};

extern struct bullet bullets[30];	//�㶹�ӵ���
extern IMAGE imgBulletNormal;
extern IMAGE imgBulletBlast[4];

//С�Ƴ�
struct car
{
	bool move;//�Ƿ����ƶ�״̬
	int x, y;//λ��
	bool used;//�Ƿ�ʹ��
};
extern struct car cars[3];
extern IMAGE imgCAR;

//����
struct shovel
{
	bool used;
	int x, y;

};

extern IMAGE imgshovel;
extern IMAGE imgshovel_frame;
extern shovel shovel_front;

extern int ballMax ;	//����������������	-�ǲ��Ǻ���֣����������Ѿ�������10��
												//���ﻹҪ����һ�飬���ʣ��ʾ��Ǹ߿���(���˽����10����һ���궨�壬���Ӹ߿���)
extern int zmMax ;	//��ʬ���н�ʬ������

extern int bulletMax ;	//�㶹�ӵ��ص�����

extern int carmax ;//С�Ƴ�����

extern struct zhiWu map[3][9];	//��ͼ���飬����洢ֲ��

extern int sunShine;	//����ֵ

//������Ϸ��ʼ��
void gameInit();

//������Ϸ��ʼ����
void startUI();

//��������Ѳ��
void viewScence();

//����״̬���»�
void barsDown();

//������Ⱦ��Ϸ����(��ȾͼƬ��������)
void updateWindow();

//�����û��������������
void userClick();

//�����ж��ļ��Ƿ����
bool fileExist(const char* name);

//�������¶���
void updateGame();

//���������Ϸ�Ƿ����
bool checkOver();

//������������
void createSunShine();

//�����������⶯��
void updateSunShine();

//�����ռ�����
void collectSunshine(ExMessage* msg);

//����������ʬ
void createZm();

//�������½�ʬ����
void updateZm();

//���������㶹�ӵ�
void createBullets();

//���������㶹�ӵ�����
void updateBullets();

//������⽩ʬ��ײ
void collisionCheck();

//�����㶹�ӵ��뽩ʬ����ײ���
void checkBullet2Zm();

//������ʬ��ֲ�����ײ���
void checkZm2ZhiWu();

//��������С�Ƴ�
void creatcar();

//����С�Ƴ�����
void updatecar();

//����С�Ƴ���ֲ�����ײ���
void checkcarzm();

//��������
void creatshovel();

//ʹ�ò���
void useshovel(ExMessage* msg);

//ֻ��Ҫ����Ϸ��ʼʱ����һ�ε�Ԫ��
void creat_front();

//ʵ����Ϸ��ͣ
void clickpause(ExMessage* msg);

//������Ϸ
void game_save();

//��ȡ��һ�ִ浵
void read_archive();

//��ʼ����ͣҪ�õ�ͼƬ����10�ţ�������װ�����ÿ���
void gameinit_pause();

//��Ⱦ��ͣʱ������ͼƬ
void update_pause(bool*type);

//��ͣʱ�����Ϣ�Ľ���
void pause_click(ExMessage *msg, bool & isRunning, bool*type);

//ʳ�˻�ͼƬ�ĳ�ʼ��
void gameinit_shirenhua();

//ͨ���Լ������Ӻͼ����ؼ����ݽ��г�ʼ����ʵ�����¿�ʼ������Ϸ
void read_regame();

//ʳ�˻��Խ�ʬ��ʵ��
void chomper_eating(int x, int y);

//ʳ�˻�������ʬ��ʵ��
void chomper_digest(int x, int y);

//ӣ�ұ�ը �ϰ�
void cherry_boom(int x,int y);