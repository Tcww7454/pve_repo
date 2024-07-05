#pragma once
#include<iostream>
using namespace std;
#include<graphics.h>
#include"tools.h"
#include<ctime>
#include<cmath>
#include"vector2.h"	//向量工具包
#include<stdio.h>
#include<mmsystem.h>	//音乐
#include<fstream> //文件
#include <setjmp.h>//用于函数间跳转功能的实现 用于一局游戏退出跳转到主菜单
#include <Windows.h>
#include<vector>

extern jmp_buf jmpbuffer;
extern jmp_buf jmpbuffer_two;
//extern jmp_buf jmpbuffer_three;
#pragma comment(lib,"winmm.lib") 

enum { WAN_DAO, XIANG_RI_KUI, SHI_REN_HUA,CHERRY,WALL_NUT, ZHI_WU_COUNT };	//植物枚举
enum { SUNSHINE_DOWN, SUNSHINE_GROUND, SUNSHINE_COLLECT, SUNSHINE_PRODUCT };  //阳光球状态枚举
enum { GOING, WIN, FAIL };

extern IMAGE imgBg;	//游戏背景图
extern IMAGE imgBar;	//状态栏，放植物的背景板
extern IMAGE imgCards[ZHI_WU_COUNT];	//植物卡牌数组
extern IMAGE* imgZhiWu[ZHI_WU_COUNT][20];	//植物数组 
extern IMAGE imgpause_one;
extern IMAGE imgpause_two;
extern IMAGE *imgpause[9];//总共9张图片，偷个懒，注意每个元素对应哪张图片吧，使用这种方法渲染墓碑图片会访问冲突
extern IMAGE imgtombstone;
extern IMAGE *imgChomperAttack[18];//食人花吃僵尸18帧图片
extern IMAGE *imgChomperDigest[18];//食人花吃僵尸18帧图片


extern int curX, curY;	//当前选中植物在移动中的坐标
extern int curZhiWu;	//当前选中的植物	0-没有选中，1-选中第一种植物
extern int killZmCount;	//杀掉的僵尸总数
extern int zmCount;		//生成的僵尸数量
extern int gameStatus;		//游戏的状态
//int flag = 0;//暂停按钮的状态，待优化

//宏定义游戏窗口大小
#define	WIN_WIDTH 900
#define	WIN_HEIGHT 600

#define ZM_MAX 10	//僵尸总数

struct zhiWu {	//植物结构体
	int type;		//0-没有植物，1-第一种植物
	int frameIndex;	//序列帧的序号
	int shootTimer;	//植物攻击时间间隔
	bool catched;	//植物是否被僵尸捕获
	int deadTimer;	//植物被吃时的死亡倒计时

	int x, y;
	int timer;	//用于向日葵生成阳光的计时器
	bool eating;//植物吃僵尸
	bool digest;//植物的消化状态
	int digest_timer;//消化的计时器
	int catchzm;//植物捕捉到的僵尸的索引 如果为了数据稳定不应该这么写的，外部修改就容易造成越界，野指针等，发生未定义行为
};

struct sunShineBall {	//阳光球结构体
	int	x, y;//阳光球的x、y坐标
	int	frameIndex;	//阳光球序列帧的序号
	int	destY;	//阳光球停止的y坐标
	bool used;	//阳光球是否在使用
	int timer;	//计时器，用来限制阳光球最后的停留时间
	int xoff;	//阳光球归位的x坐标
	int yoff;	//阳光球归位的y坐标

	//优化
	float t;	//贝塞尔曲线的时间点	0，1
	vector2 p1, p2, p3, p4;
	vector2	pCur;	//当前时刻阳光球的位置
	float	speed;
	int	status;	//阳光球的状态
};

extern struct sunShineBall	balls[10];	//阳光球池，用来事先存储阳光
extern IMAGE	imgSunShineBall[29];	//阳光序列帧总数	-	可以定义一个宏，方便后期管理

struct zm {		//僵尸结构体	-后期还是需要像植物一样搞个枚举,方便创建不同类型的僵尸
	int x, y;
	int row;
	int frameIndex;
	bool used;
	int speed;	//僵尸前行速度
	int blood;	//僵尸血量
	bool dead;	//僵尸是否死亡
	bool eating;	//僵尸是否在吃植物
	//bool boom;
};

extern struct zm zms[10];	//僵尸池,用来事先存储僵尸
extern IMAGE	imgZm[22];
extern IMAGE	imgZmDead[20];
extern IMAGE	imgZmEat[21];
extern IMAGE	imgZmStand[11];

struct bullet {	//豌豆子弹结构体
	int x, y, row, speed;
	bool used;
	bool blast;	//是否爆炸
	int frameIndex;	//爆炸帧序号
};

extern struct bullet bullets[30];	//豌豆子弹池
extern IMAGE imgBulletNormal;
extern IMAGE imgBulletBlast[4];

//小推车
struct car
{
	bool move;//是否处于移动状态
	int x, y;//位置
	bool used;//是否被使用
};
extern struct car cars[3];
extern IMAGE imgCAR;

//铲子
struct shovel
{
	bool used;
	int x, y;

};

extern IMAGE imgshovel;
extern IMAGE imgshovel_frame;
extern shovel shovel_front;

extern int ballMax ;	//阳光池中阳光的总数	-是不是很奇怪，明明上面已经定义了10，
												//这里还要计算一遍，别问，问就是高可用(个人建议把10换成一个宏定义，更加高可用)
extern int zmMax ;	//僵尸池中僵尸的总数

extern int bulletMax ;	//豌豆子弹池的总数

extern int carmax ;//小推车总数

extern struct zhiWu map[3][9];	//地图数组，方便存储植物

extern int sunShine;	//阳光值

//声明游戏初始化
void gameInit();

//声明游戏开始界面
void startUI();

//声明场景巡场
void viewScence();

//声明状态栏下滑
void barsDown();

//声明渲染游戏窗口(渲染图片到窗口上)
void updateWindow();

//声明用户点击（鼠标操作）
void userClick();

//声明判断文件是否存在
bool fileExist(const char* name);

//声明更新动作
void updateGame();

//声明检测游戏是否结束
bool checkOver();

//声明创建阳光
void createSunShine();

//声明更新阳光动作
void updateSunShine();

//声明收集阳光
void collectSunshine(ExMessage* msg);

//声明创建僵尸
void createZm();

//声明更新僵尸动作
void updateZm();

//声明创建豌豆子弹
void createBullets();

//声明更新豌豆子弹动作
void updateBullets();

//声明检测僵尸碰撞
void collisionCheck();

//声明豌豆子弹与僵尸的碰撞检测
void checkBullet2Zm();

//声明僵尸与植物的碰撞检测
void checkZm2ZhiWu();

//声明创建小推车
void creatcar();

//更新小推车数据
void updatecar();

//声明小推车与植物的碰撞检测
void checkcarzm();

//创建铲子
void creatshovel();

//使用铲子
void useshovel(ExMessage* msg);

//只需要在游戏开始时创建一次的元素
void creat_front();

//实现游戏暂停
void clickpause(ExMessage* msg);

//保存游戏
void game_save();

//读取上一局存档
void read_archive();

//初始化暂停要用的图片，有10张，单独封装出来好看点
void gameinit_pause();

//渲染暂停时的所有图片
void update_pause(bool*type);

//暂停时鼠标信息的接收
void pause_click(ExMessage *msg, bool & isRunning, bool*type);

//食人花图片的初始化
void gameinit_shirenhua();

//通过对几个池子和几个关键数据进行初始化来实现重新开始本局游戏
void read_regame();

//食人花吃僵尸的实现
void chomper_eating(int x, int y);

//食人花消化僵尸的实现
void chomper_digest(int x, int y);

//樱桃爆炸 废案
void cherry_boom(int x,int y);