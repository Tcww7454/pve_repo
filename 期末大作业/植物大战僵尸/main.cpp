#include<iostream>
using namespace std;	//暂时不知道怎么修改
#include<graphics.h>
#include"tools.h"
#include<ctime>
#include<ctime>
#include<cmath>
#include"vector2.h"	//向量工具包
#include<stdio.h>
#include<mmsystem.h>	//音乐
#include<fstream> //文件
#include <setjmp.h>//用于一局游戏退出跳转到主菜单
#include <Windows.h>

jmp_buf jmpbuffer;
jmp_buf jmpbuffer_two;
#pragma comment(lib,"winmm.lib") 

enum {WAN_DAO,XIANG_RI_KUI,SHI_REN_HUA,ZHI_WU_COUNT};	//植物枚举
enum {SUNSHINE_DOWN,SUNSHINE_GROUND,SUNSHINE_COLLECT,SUNSHINE_PRODUCT};  //阳光球状态枚举
enum { GOING, WIN, FAIL };

IMAGE imgBg;	//游戏背景图
IMAGE imgBar;	//状态栏，放植物的背景板
IMAGE imgCards[ZHI_WU_COUNT];	//植物卡牌数组
IMAGE* imgZhiWu[ZHI_WU_COUNT][20];	//植物数组 
IMAGE imgpause_one;
IMAGE imgpause_two;
IMAGE *imgpause[9];//总共9张图片，偷个懒，注意每个元素对应哪张图片吧，使用这种方法渲染墓碑图片会访问冲突
IMAGE imgtombstone;

int curX, curY;	//当前选中植物在移动中的坐标
int curZhiWu;	//当前选中的植物	0-没有选中，1-选中第一种植物

int killZmCount;	//杀掉的僵尸总数
int zmCount;		//生成的僵尸数量
int gameStatus;		//游戏的状态
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

struct sunShineBall	balls[10];	//阳光球池，用来事先存储阳光
IMAGE	imgSunShineBall[29];	//阳光序列帧总数	-	可以定义一个宏，方便后期管理

struct zm {		//僵尸结构体	-后期还是需要像植物一样搞个枚举,方便创建不同类型的僵尸
	int x, y;
	int row;
	int frameIndex;
	bool used;
	int speed;	//僵尸前行速度
	int blood;	//僵尸血量
	bool dead;	//僵尸是否死亡
	bool eating;	//僵尸是否在吃植物
};

struct zm zms[10];	//僵尸池,用来事先存储僵尸
IMAGE	imgZm[22];	
IMAGE	imgZmDead[20];
IMAGE	imgZmEat[21];
IMAGE	imgZmStand[11];

struct bullet {	//豌豆子弹结构体
	int x, y, row,speed;
	bool used;
	bool blast;	//是否爆炸
	int frameIndex;	//爆炸帧序号
};

struct bullet bullets[30];	//豌豆子弹池
IMAGE imgBulletNormal;
IMAGE imgBulletBlast[4];

//小推车
struct car
{
	bool move;//是否处于移动状态
	int x, y;//位置
	bool used;//是否被使用
};
struct car cars[3];
IMAGE imgCAR;

//铲子
struct shovel
{
	bool used;
	int x, y;
	
};

IMAGE imgshovel;
IMAGE imgshovel_frame;
shovel shovel_front;

int ballMax = sizeof(balls) / sizeof(balls[0]);	//阳光池中阳光的总数	-是不是很奇怪，明明上面已经定义了10，
												//这里还要计算一遍，别问，问就是高可用(个人建议把10换成一个宏定义，更加高可用)
int zmMax = sizeof(zms) / sizeof(zms[0]);	//僵尸池中僵尸的总数

int bulletMax = sizeof(bullets) / sizeof(bullets[0]);	//豌豆子弹池的总数

int carmax = sizeof(cars) / sizeof(cars[0]);//小推车总数

struct zhiWu map[3][9];	//地图数组，方便存储植物

int sunShine;	//阳光值

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

//通过对几个池子和几个关键数据进行初始化来实现重新开始本局游戏
void read_regame();

int main() {
		
	gameInit();	//游戏初始化

	setjmp(jmpbuffer);
	
	startUI();	//加载游戏开始界面

	setjmp(jmpbuffer_two);
	//viewScence();	//场景巡场

	//barsDown();	//状态栏下滑


	//双缓冲，先将要绘制的内容一次性绘制在图片上，再把图片输出，避免不断从内存读取数据而导致的屏幕闪烁
	//主要由BeginBatchDraw()、EndBatchDraw()、FlushBatchDraw()组成
	//BeginBatchDraw();		双缓冲加到外面会卡状态栏bug-现在的技术暂时解决不了

	int timer=0;
	bool flag = true;

	while (1)
	{
		userClick();	//获取用户点击事件

		timer += getDelay();	//获取间隔时间
		if (timer > 20) {	//用来限制植物渲染时间
			timer = 0;
			flag = true;
		}
		if (flag) {

			flag = false;

			updateWindow();	//更新游戏窗口（渲染）

			updateGame();	//更新动作

			if (checkOver())break;	//检查游戏是否结束
		}
		//刷新图片，避免黑屏
		//FlushBatchDraw();
	}
	
	//EndBatchDraw();

	system("pause");

	return 0;
}

//游戏初始化实现
void gameInit() {

	//设置随机种子
	srand(time(NULL));

	//加载游戏背景图片
	loadimage(&imgBg, "res/bg.jpg");

	//加载状态栏
	loadimage(&imgBar, "res/bar5.png");

	killZmCount = 0;
	zmCount = 0;
	gameStatus = GOING;

	memset(imgZhiWu, 0, sizeof(imgZhiWu));	//给指针赋空值

	memset(map, 0, sizeof(map));	//初始化地图数组

	memset(balls, 0, sizeof(balls));	//初始化阳光池

	memset(zms, 0, sizeof(zms));	//初始化僵尸池

	memset(bullets, 0, sizeof(bullets));	//初始化豌豆子弹池

	//加载植物卡牌
	char	name[64];
	for (int i = 0; i < ZHI_WU_COUNT; i++) {
		//生成植物卡牌的文件名
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i+1);
		loadimage(&imgCards[i], name);
		for (int j = 0; j < 20; j++) {	//20是固定值，可以寻求更匹配的方式
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png", i, j + 1);
			//判断文件是否存在
			if (fileExist(name)) {
				imgZhiWu[i][j] = new IMAGE;
				loadimage(imgZhiWu[i][j], name);
			}
			else {
				break;
			}
		}		
	}

	//初始化选中植物
	curZhiWu = 0;

	//初始化阳光值
	sunShine = 50;

	//加载阳光
	for (int i = 0; i < 29; i++) {	//29是固定值，可以寻求更匹配的方式
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png",i+1);
		loadimage(&imgSunShineBall[i],name);
	}

	//加载僵尸图片
	for (int i = 0; i < 22; i++) {
		sprintf_s(name, sizeof(name), "res/zm/%d.png", i + 1);
		loadimage(&imgZm[i], name);
	}

	//加载僵尸死亡图片
	for (int i = 0; i < 20; i++) {
		sprintf_s(name, sizeof(name), "res/zm_dead/%d.png", i + 1);
		loadimage(&imgZmDead[i], name);  
	}

	//加载僵尸吃植物图片
	for (int i = 0; i < 21; i++) {
		sprintf_s(name, sizeof(name), "res/zm_eat/%d.png", i + 1);
		loadimage(&imgZmEat[i], name);
	}

	//加载巡场僵尸图片
	for (int i = 0; i < 11; i++) {
		sprintf_s(name, sizeof(name), "res/zm_stand/%d.png", i + 1);
		loadimage(&imgZmStand[i], name);
	}

	//加载豌豆子弹图片
	loadimage(&imgBulletNormal, "res/bullets/bullet_normal.png");
	
	//加载豌豆子弹爆炸图片
	loadimage(&imgBulletBlast[3],"res/bullets/bullet_blast.png");
	for (int i = 0; i < 3; i++) {
		float	k = (i + 2) * 0.2;
		loadimage(&imgBulletBlast[i], "res/bullets/bullet_blast.png",
			imgBulletBlast[3].getwidth() * k, imgBulletBlast[3].getheight() * k, true);
	}

	//加载小推车图片
	loadimage(&imgCAR, "res/Screen/car.png");

	//创建游戏窗口
	initgraph(WIN_WIDTH, WIN_HEIGHT,1/*,1*/);	//参数1表示再开一个控制台窗口	

	//设置字体
	LOGFONT	f;
	gettextstyle(&f);
	f.lfHeight = 30;
	f.lfWeight = 15;
	strcpy(f.lfFaceName, "Segoe UI Black");	//设置字体效果
	f.lfQuality = ANTIALIASED_QUALITY;		//抗锯齿
	settextstyle(&f);
	setbkmode(TRANSPARENT);					//字体模式：背景透明
	setcolor(BLACK);						//字体颜色：黑色


	//铲子框以及铲子图片
	loadimage(&imgshovel,"res/Screen/shovel.png");
	loadimage(&imgshovel_frame, "res/Screen/shovelSlot.png", imgshovel_frame.getwidth() * 2, imgshovel_frame.getheight() * 2, true);

	//初始化暂停的图标，没找到合适的图片，随便凑活凑活
	loadimage(&imgpause_one, "res/pause/SelectorScreen_Quit1.png");//未按下时为灰色
	loadimage(&imgpause_two, "res/pause/SelectorScreen_Quit2.png");//按下时为绿色

	//初始化9张暂停图片
	gameinit_pause();

	//封装一个只需要在游戏开始时创建的物品的函数，这里只有小推车与铲子的创建函数
	creat_front();

	/*loadimage(&imgBulletBlast[i], "res/bullets/bullet_blast.png",
		imgBulletBlast[3].getwidth() * k, imgBulletBlast[3].getheight() * k, true);*/
}

//游戏开始界面实现
void startUI() {
	IMAGE imgMenu,imgMenu1,imgMenu2,imgcontinue1,imgcontinue2; 
	int	flag1 = 0,flag2=0;//以为开始游戏的图标，2为继续上局游戏的图标
	loadimage(&imgMenu, "res/menu.png");	//加载开始背景图
	loadimage(&imgMenu1, "res/menu1.png");
	loadimage(&imgMenu2, "res/menu2.png");
	loadimage(&imgcontinue1, "res/Screen/continue1.png");
	loadimage(&imgcontinue2, "res/Screen/continue2.png");

	while (1) 
	{
		BeginBatchDraw();
		//flag1 = 0, flag2 = 0;
		putimage(0, 0, &imgMenu);	//渲染开始背景图到窗口上
		putimagePNG(474, 75, flag1 == 0 ? &imgMenu2 : &imgMenu1);
		putimagePNG(484, 300, flag2 == 0 ? &imgcontinue2 : &imgcontinue1);

		ExMessage	msg;
		if (peekmessage(&msg)) {
			if (msg.x > 474 && msg.x < 774 && msg.y>75 && msg.y < 215)flag1 = 1; else flag1 = 0;
			if (msg.x > 484 && msg.x < 484 + 286 && msg.y>300 && msg.y < 300 + 122)flag2 = 1; else flag2 = 0;

			if (msg.message == WM_LBUTTONDOWN &&	//鼠标左键落下		扩展：当鼠标经过时也可以高亮
				msg.x > 474 && msg.x < 774 && msg.y>75 && msg.y < 215) {
				flag1 = 1;
			}
			else if (msg.message == WM_LBUTTONDOWN &&	//鼠标左键落下		扩展：当鼠标经过时也可以高亮 286 122
				msg.x > 484 && msg.x < 484+286 && msg.y>300 && msg.y < 300+122) {
				flag2 = 1;
				
			}
		}
		else if (msg.message == WM_LBUTTONUP ) {	//鼠标左键抬起
			if (msg.x > 474 && msg.x < 774 && msg.y>75 && msg.y < 215 && flag1 == 1)
			{
				flag1 = 0;
				EndBatchDraw();
				return;
			}
			else if (msg.x > 484 && msg.x < 484 + 286 && msg.y>300 && msg.y < 300 + 122 && flag2 == 1)
			{
				flag2 = 0;
				read_archive();
				EndBatchDraw();
				return;
			}
			else
			{
				flag1 =flag2= 0;
			}
		}
		EndBatchDraw();
	}
}

//声明场景巡场实现
void viewScence() {
	int xMin = WIN_WIDTH - imgBg.getwidth();
	vector2 points[9] = { {550,80},{530,160},{630,170},{530,200},{525,270},		//9个僵尸站位
		{565,370},{605,340},{705,280},{690,340}};
	int index[9];
	for (int i = 0; i < 9; i++) {
		index[i] = rand() % 11;
	}
	int count = 0;
	for (int x = 0; x >= xMin; x -= 4) {	//每帧移动2像素
		BeginBatchDraw();

		putimage(x, 0, &imgBg);

		count++;
		for (int k = 0; k < 9; k++) {
			putimagePNG(points[k].x - xMin + x, points[k].y, &imgZmStand[index[k]]);
			if (count >= 10) {
				index[k] = (index[k] + 1) % 11;
			}
		}
		if (count >= 10)count = 0;

		EndBatchDraw();
		Sleep(5);
	}

	//停留1s
	for (int i = 0; i < 100; i++) {
		BeginBatchDraw();

		putimage(xMin, 0, &imgBg);
		for (int j = 0; j < 9; j++) {
			putimagePNG(points[j].x, points[j].y, &imgZmStand[index[j]]);
			index[j] = (index[j] + 1) % 11;
		}

		EndBatchDraw();
		Sleep(20);
	}


	//往回移
	for (int x = xMin; x <= -112; x += 4) {	//每帧移动2像素
		BeginBatchDraw();

		putimage(x, 0, &imgBg);

		count++;
		for (int k = 0; k < 9; k++) {
			putimagePNG(points[k].x - xMin + x, points[k].y, &imgZmStand[index[k]]);
			if (count >= 10) {
				index[k] = (index[k] + 1) % 11;
			}
			if (count >= 10)count = 0;
		}
		
		EndBatchDraw();
		Sleep(5);
	}
}

//状态栏下滑实现
//void barsDown() {
//	int height = imgBar.getheight();
//	for (int y = -height; y <= 0; y++) {
//		BeginBatchDraw();
//
//		putimage(-112, 0, &imgBg);
//
//		putimagePNG(250, y, &imgBar);
//
//		for (int i = 0; i < ZHI_WU_COUNT; i++) {
//			int x = 338 + i * 65;
//			putimagePNG(x, 6 + y, &imgCards[i]);
//		}
//		/*
//		 putimage(0,0,&imgBg);        //把图片渲染出来
//		putimagePNG(250, 0, &imgBar);  //背景透明化
//
//		for (int i = 0; i < ZHI_WU_COUNT; i++) {
//			int x = 338 + i * 65;
//			int y = 6;
//			putimage(x, y, &imgCards[i]);
//		}
//
//		*/
//
//		EndBatchDraw();
//		Sleep(5);
//	}
//}

void barsDown()
{
	int height = imgBar.getheight();
	//卡牌槽IMAGE imgBar;
	for (int y = -height; y <= -10; y++)
	{
		BeginBatchDraw();

		putimagePNG(80, y, &imgBar);
		for (int i = 0; i < ZHI_WU_COUNT; i++)
		{
				putimagePNG(163 + i * 65, y + 10, &imgCards[i]);
		}

		EndBatchDraw();
		Sleep(10);
	}

	Sleep(200);
}

//把图片加载到窗口上(渲染)
void updateWindow() 
{

	BeginBatchDraw();

	//putimage(-112, 0, &imgBg);	//加载(渲染)背景板
	//putimagePNG(255, 0, &imgBar);	//加载(渲染)状态栏
	putimage(-112, 0, &imgBg);
	putimagePNG(80, -10, &imgBar);

	//for (int i = 0; i < ZHI_WU_COUNT; i++) {	//加载(渲染)植物卡牌
	//	int x = 343 + i * 65;
	//	int y = 6;
	//	putimagePNG(x, y, &imgCards[i]);
	//}
	for (int i = 0; i < ZHI_WU_COUNT; i++)
	{
		if (i == ZHI_WU_COUNT)
			putimagePNG(163 + i * 65 + 8, -5, &imgCards[i]);
		else
			putimagePNG(163 + i * 65, 0, &imgCards[i]);
	}

	//在地图上加载(渲染)植物
	for (int i = 0; i < 3; i++) {	
		for (int j = 0; j < 9; j++){
			if (map[i][j].type > 0) {
				//int x = 256 + j * 81;
				//int y = 179 + i * 102 + 14;
				int zhiWuType = map[i][j].type - 1;
				int index = map[i][j].frameIndex;
				//putimagePNG(x, y, imgZhiWu[zhiWuType][index]);
				putimagePNG(map[i][j].x, map[i][j].y, imgZhiWu[zhiWuType][index]);
			}
		}
	}

	//渲染铲子
	if (shovel_front.used == false) {
		putimagePNG(235 +7* 65, 0, &imgshovel_frame);
		putimagePNG(235 +7* 65, 0, &imgshovel);
	}
	else if (shovel_front.used == true)
	{
		putimagePNG(235 +7* 65, 0, &imgshovel_frame);
		putimagePNG(shovel_front.x, shovel_front.y, &imgshovel);
	}

	//加载(渲染)拖动的植物
	if (curZhiWu > 0) {
		IMAGE* img = imgZhiWu[curZhiWu - 1][0];	
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, img);
	}

	//for (int i = 0; i < ballMax; i++) {	//此时渲染会被僵尸挡住
	//	if (balls[i].used || balls[i].xoff) {	//加载(渲染)阳光
	//		IMAGE* img = &imgSunShineBall[balls[i].frameIndex];
	//		putimagePNG(balls[i].x, balls[i].y, img);
	//	}
	//}

	//加载(渲染)阳光值
	char scoreText[8];
	sprintf_s(scoreText, sizeof(scoreText), "%d", sunShine);	//把阳光值转换成字符类型
	outtextxy(110, 60, scoreText);			//渲染输出	位置可调整成居中,而不使用固定值y	255-283 80-108

	//加载(渲染)僵尸
	for (int i = 0; i < zmMax; i++) {
		if (zms[i].used) {	
			//IMAGE* img = &imgZm[zms[i].frameIndex];
			//IMAGE* img = (zms[i].dead) ? imgZmDead : imgZm;
			
			IMAGE* img = NULL;
			if (zms[i].eating) {
				img = imgZmEat;
			}else if(zms[i].dead) {
				img = imgZmDead;
			}
			else {
				img = imgZm;
			}

			img += zms[i].frameIndex;
			putimagePNG(zms[i].x, zms[i].y-img->getheight(), img);
		}
	}

	//加载(渲染)豌豆子弹
	for (int i = 0; i < bulletMax; i++) {
		if (bullets[i].used) {
			if (bullets[i].blast) {	//豌豆子弹碰撞渲染
				IMAGE* img = &imgBulletBlast[bullets[i].frameIndex];
				//IMAGE* img = &imgBulletBlast[];
				putimagePNG(bullets[i].x, bullets[i].y, img);
				FlushBatchDraw();
			}
			else {	//豌豆子弹普通形态渲染
				putimagePNG(bullets[i].x, bullets[i].y, &imgBulletNormal);
			}
		}
	}

	//加载(渲染)阳光
	for (int i = 0; i < ballMax; i++) {
		if (balls[i].used /* || balls[i].xoff*/) {
			IMAGE* img = &imgSunShineBall[balls[i].frameIndex];
			//putimagePNG(balls[i].x, balls[i].y, img);
			putimagePNG(balls[i].pCur.x, balls[i].pCur.y, img);
		}
	}

	//putimagePNG((235 + 8 * 65), 0, flag == 0 ? &imgpause_one : &imgpause_two);
	//putimagePNG((235 + 8 * 65), 25, flag == 0 ? imgpause[2] : imgpause[3]);
	putimagePNG((235 + 8 * 65), 25, imgpause[2]);

	int carmax = sizeof(cars) / sizeof(cars[0]);
	for (int i = 0; i < carmax; i++)
	{
		if (cars[i].used)
			putimagePNG(cars[i].x, cars[i].y, &imgCAR);
	}

	EndBatchDraw();
}

//用户点击实现
void userClick() {
	static	int status = 0;
	ExMessage	msg;
	if (peekmessage(&msg)) {	//判断用户是否有操作
		useshovel(&msg);
		clickpause(&msg);
		if (msg.message == WM_LBUTTONDOWN) {	//鼠标左键按下
			if (msg.x > 163 && msg.x < 163 + 65 * ZHI_WU_COUNT && msg.y < 96) {	//点击卡牌的事件
				int index = (msg.x - 163) / 65;
				
				//判断阳光值是否足够购买植物
				//待优化，应该种下后再减的
				if (index == XIANG_RI_KUI) {
					if (sunShine >= 50) {
						status = 1;
						curZhiWu = index + 1;
						//使植物显示在点击位置，避免了植物出现在上次消失位置的小bug
						curX = msg.x;
						curY = msg.y;
						sunShine -= 50;
					}
				}
				else if (index == WAN_DAO) {
					if (sunShine >= 100) {
						status = 1;
						curZhiWu = index + 1;
						//使植物显示在点击位置，避免了植物出现在上次消失位置的小bug
						curX = msg.x;
						curY = msg.y;
						sunShine -= 100;
					}
				}
				else if (index == SHI_REN_HUA) {
					if (sunShine >= 150) {
						status = 1;
						curZhiWu = index + 1;
						//使植物显示在点击位置，避免了植物出现在上次消失位置的小bug
						curX = msg.x;
						curY = msg.y;
						sunShine -= 150;
					}
				}
				//其他的还有待补充

				//status = 1;
				//使植物显示在点击位置，避免了植物出现在上次消失位置的小bug
				//curX = msg.x;
				//curY = msg.y;
			}
			else {	//收集阳光事件
				collectSunshine(&msg);
			}
		}
		else if (msg.message == WM_MOUSEMOVE && status == 1) {	//鼠标移动
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message == WM_RBUTTONDOWN && status == 1) {	//鼠标右键按下
			if (msg.x > 256 - 112 && msg.x < 900 - 30 && msg.y > 179 && msg.y < 489) {
				int	row = (msg.y - 179) / 102;	//获取行
				int	col = (msg.x - 256 + 112) / 81;	//获取列
				if (map[row][col].type == 0) {
					map[row][col].type = curZhiWu;	//给鼠标当前行种下植物
					map[row][col].frameIndex = 0;	//渲染植物第一帧
					map[row][col].shootTimer = 0;	//初始化发射时间

					map[row][col].x = 256 - 112 + col * 81;	//植物坐标
					map[row][col].y = 179 + row * 102 + 14;
				}
			}
			//使植物释放消失
			curZhiWu = 0;
			status = 0;
			//重置植物的坐标
			//curX = 1000;
			//curY = 1000;
		}
	}
}

//判断文件是否存在实现
bool fileExist(const char* name) {
	FILE* fp=fopen(name, "r");
	if (fp == NULL) {
		return false;
	}
	else {
		fclose(fp);
		return true;
	}
}

//更新动作实现
void updateGame() {

	//更新植物动作
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0) {
				map[i][j].frameIndex++;
				int	zhiWuType = map[i][j].type - 1;
				int	index = map[i][j].frameIndex;
				if (imgZhiWu[zhiWuType][index] == NULL) {
					map[i][j].frameIndex = 0;
				}
			}
		}
	}

	

	//小推车与僵尸的碰撞检测
	checkcarzm();

	//更新小推车
	updatecar();

	//创建僵尸
	createZm();

	//更新僵尸动作
	updateZm();
	
	//创建阳光
	createSunShine();

	//更新阳光动作
	updateSunShine();

	//创建豌豆子弹
	createBullets();	//豌豆多了之后频率不一样

	//更新豌豆子弹动作
	updateBullets();

	//豌豆子弹与僵尸碰撞
	collisionCheck();
}

//检测游戏是否结束实现
bool checkOver() {
	BeginBatchDraw();

	bool ret = false;

	if (gameStatus == WIN) {
		Sleep(100);
		loadimage(0, "res/gameWin.png");
		mciSendString("play res/win.mp3", 0, 0, 0);
		ret = true;
	}
	else if (gameStatus == FAIL) {
		Sleep(100);
		loadimage(0, "res/gameFail.png");
		mciSendString("play res/lose.mp3", 0, 0, 0);
		ret = true;
	}
	EndBatchDraw();
	return ret;
}

//创建阳光实现
void createSunShine() {

	static	int	count = 0;
	static	int	fre = 200;
	count++;
	
	if (count >= fre) {	//限制阳光生成的速度

		fre = 100 + rand() % 150;	//第二次生成阳光的时间随机
		count = 0;

		int i;

		//从阳光池中取出可用的阳光
		for (i = 0; i < ballMax && balls[i].used; i++);	//别问，问就是一种新定义方式，跟{}一个样
		if (i >= ballMax)return;

		balls[i].used = true;
		balls[i].frameIndex = 0;
		//balls[i].x = 260 + rand() % (900 - 320);	//随机落点
		//balls[i].y = 60;
		//balls[i].destY = 200 + (rand() % 4) * 90;	//随机停止位置
		//balls[i].xoff = 0;
		//balls[i].yoff = 0;
		balls[i].timer = 0;

		//优化
		balls[i].status = SUNSHINE_DOWN;
		balls[i].t = 0;
		balls[i].p1 = vector2(260 - 112 + rand() % (900 - 320 + 112), 60);
		balls[i].p4 = vector2(balls[i].p1.x, 200 + (rand() % 4) * 90);
		int off = 2;
		float distance = balls[i].p4.y - balls[i].p1.y;
		balls[i].speed = 1.0 / (distance / off);
	}

	//向日葵生产阳光
	for (int i = 0; i < 3; i++) {
		for(int j = 0; j < 9; j++) {
			if (map[i][j].type == XIANG_RI_KUI + 1) {
				map[i][j].timer++;
				if (map[i][j].timer > 200) {
					map[i][j].timer = 0;

					int k;
					for (k = 0; k < ballMax && balls[k].used; k++);
					if (k >= ballMax)return;

					balls[k].used = true;

					balls[k].p1 = vector2(map[i][j].x, map[i][j].y);	//设置贝塞尔曲线的参数
					int w = (50 + rand() % 51) * (rand() % 2 ? 1 : -1);
					balls[k].p4 = vector2(map[i][j].x + w, map[i][j].y + imgZhiWu[XIANG_RI_KUI][0]->getheight()
						- imgSunShineBall->getheight());
					balls[k].p2 = vector2(balls[k].p1.x + w * 0.3, balls[k].p1.y - 100);
					balls[k].p3 = vector2(balls[k].p1.x + w * 0.7, balls[k].p1.y - 150);

					balls[k].status = SUNSHINE_PRODUCT;
					balls[k].speed = 0.05;
					balls[k].t = 0;
				}
			}
		}
	}
}

//更新阳光动作实现
void updateSunShine() {
	for (int i = 0; i < ballMax; i++) {
		if (balls[i].used) {
			balls[i].frameIndex = (balls[i].frameIndex + 1) % 29;	//更新序列帧

			if (balls[i].status == SUNSHINE_DOWN) {
				struct sunShineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = sun->p1 + sun->t * (sun->p4 - sun->p1);
				if (sun->t >= 1) {
					sun->status = SUNSHINE_GROUND;
					sun->t = 0;
					sun->timer = 0;
				}
			}
			else if (balls[i].status == SUNSHINE_GROUND) {
				balls[i].timer++;
				if (balls[i].timer >50) {//阳光自动消失或收集的时间
					//通过更改此处可以选择是否进行自动收集阳光
					{//阳光到时间消失
						balls[i].used = false;
						balls[i].timer = 0;
					}
					//自动收集
						/*balls[i].timer = 0;
						balls[i].status = SUNSHINE_COLLECT;*/
					
				}
			}
			else if (balls[i].status == SUNSHINE_COLLECT) {
				struct sunShineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = sun->p1 + sun->t * (sun->p4 - sun->p1);
				if (sun->t > 1) {
					sunShine += 25;
					sun->used = false;
					sun->t = 0;
				}
			}
			else if (balls[i].status == SUNSHINE_PRODUCT) {
				struct sunShineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = calcBezierPoint(sun->t, sun->p1, sun->p2, sun->p3, sun->p4);
				if (sun->t > 1) {
					sun->status = SUNSHINE_GROUND;
					sun->t = 0;
					sun->timer = 0;
				}
			}

			/*if (balls[i].y < balls[i].destY) {
				balls[i].y += 2;
			}
			if (balls[i].y >= balls[i].destY) {
				balls[i].timer++;
				//cout << "i=" << i << ":" << balls[i].timer << endl;
				if (balls[i].timer > 100) {
					balls[i].timer = 0;	//重置定时器，避免下一次取出同样的阳光球一到达停止位置就消失
					balls[i].used = false;
					//printf((balls[i].timer));
					//cout << "i=" << i << ":" << balls[i].timer << endl;
				}
			}*/
		}
		/*else if (balls[i].xoff) {	//移动阳光球
			//设置阳光球的偏移值
			float	destY = 0;
			float	destX = 262;

			float angle = atan((balls[i].y - destY) / (balls[i].x - destX));	//三角函数

			balls[i].xoff = 4 * cos(angle);
			balls[i].yoff = 4 * sin(angle);

			balls[i].x -= balls[i].xoff;
			balls[i].y -= balls[i].yoff;

			if (balls[i].x < 262 || balls[i].y < 0) {
				//sunShine += 25;	//在这里加阳光值会出bug	
				balls[i].xoff = 0;
				balls[i].yoff = 0;
				
			}
		}*/
	}
}

//收集阳光实现
void collectSunshine(ExMessage* msg) {
	int w = imgSunShineBall[0].getwidth();	//单个阳光球的宽度
	int h = imgSunShineBall[0].getheight();	//单个阳光球的高度
	for (int i = 0; i < ballMax; i++) {
		if (balls[i].used) {	//阳光球被使用了才进行操作
			int x = balls[i].pCur.x;
			int y = balls[i].pCur.y;
			if (msg->x > x && msg->x<x + w &&		//只有当光标在阳光范围内才进行操作
				msg->y>y && msg->y < y + h) {
				
				//balls[i].used = false;	//阳光球消失
				
				balls[i].status = SUNSHINE_COLLECT;

				//sunShine += 25;			//阳光值加25
				
				//mciSendString("play res/sunshine.mp3", 0, 0, 0);	

				PlaySound("res/sunshine.wav", NULL, SND_FILENAME | SND_ASYNC);	//解决点击过快没音效，要使用WAV格式的音乐

				//设置阳光球的偏移值
				//float	destY = 0;
				//float	destX = 262;

				//float angle = atan((balls[i].y - destY) / (balls[i].x - destX));	//三角函数

				//balls[i].xoff = 4 * cos(angle);
				//balls[i].yoff = 4 * sin(angle);

				//优化
				balls[i].p1 = balls[i].pCur;
				balls[i].p4 = vector2(100, 0);
				balls[i].t = 0;
				float distance = dis(balls[i].p1 - balls[i].p4);
				float off = 8;
				balls[i].speed = 1.0 / (distance / off);
				break;
			}
		}
	}
}

//创建僵尸实现
void createZm() {

	if (zmCount >= ZM_MAX) {
		return;
	}

	static	int	count = 0;
	static	int	zmFre = 500;
	count++;

	if (count >= zmFre) {	//限制僵尸生成的速度

		zmFre = 300 + rand() % 200;	//第二次生成僵尸的时间随机
		count = 0;

		int i;

		//从僵尸池中取出可用的僵尸
		for (i = 0; i < zmMax && zms[i].used; i++);	//别问，问就是一种新定义方式，跟{}一个样，就是&&!xx
		if (i >= zmMax)return;

		zms[i].used = true;
		//zms[i].frameIndex = 0;
		zms[i].x = WIN_WIDTH;
		zms[i].row = rand() % 3;
		zms[i].y = 172 + (zms[i].row + 1) * 100;
		zms[i].speed = 3;
		zms[i].blood = 100;
		zms[i].dead = false;
		zms[i].eating = false;

		zmCount++;
	}
}

//更新僵尸动作实现
void updateZm() {
	//更新僵尸位置
	static int count = 0;
	count++;
	if (count > 2) {	//限制僵尸前进速度
		count = 0;
		for (int i = 0; i < zmMax; i++) {
			if (zms[i].used) {
				zms[i].x -= zms[i].speed;
				if (zms[i].x < 38) {
					//结束游戏
					gameStatus = FAIL;
				}
			}
		}
	}
	
	//更新僵尸动作
	static int count2=0;
	count2++;
	if (count2 > 4) {	//限制僵尸动作更新速度，避免鬼畜
		count2 = 0;
		for (int i = 0; i < zmMax; i++) {
			if (zms[i].used) {
				if (zms[i].dead) {
					zms[i].frameIndex++;
					if (zms[i].frameIndex >= 20) {
						zms[i].used = false;
						killZmCount++;
						cout << "杀掉的僵尸数：" << killZmCount << endl;
						if (killZmCount == ZM_MAX) {
							gameStatus = WIN;
						}
					}
				}
				else if (zms[i].eating) {
					zms[i].frameIndex = (zms[i].frameIndex + 1) % 20;
				}
				else
				{
					zms[i].frameIndex = (zms[i].frameIndex + 1) % 21;	//僵尸会闪现	-未解决(发现问题，求余大于10就会闪现，不会解决)
					//cout << "i=" << i << zms[i].frameIndex << endl;	//已破案，加载僵尸图片时，数量加载错了
				}
			}
		}
	}
}

//创建豌豆子弹实现
void createBullets() {
	int lines[3] = { 0 };
	int	dangerX = WIN_WIDTH - imgZm[0].getwidth() + 50;	//定义开始射击距离
	for (int i = 0; i < zmMax; i++) {
		if (zms[i].used && zms[i].x < dangerX) {
			lines[zms[i].row] = 1;	//僵尸所在行改标记
			//cout <<"僵尸所在行" << zms[i].row << endl;
		}
	}
	for (int i = 0; i < 3; i++) {	//3和9固定值，扩展性差，还是建议宏定义
		for (int j = 0; j < 9; j++) {
			if (lines[i] && map[i][j].type == 1) {	//有豌豆且僵尸走到打击范围
				map[i][j].shootTimer++;
				if (map[i][j].shootTimer > 20) {
					map[i][j].shootTimer = 0;
					int k;
					for (k = 0; k < bulletMax && bullets[k].used; k++);
					if (k >= bulletMax) return;
						bullets[k].used = true;		//初始化豌豆子弹
						bullets[k].row = i;
						//cout << "i=" << i << endl;
						bullets[k].speed = 4;
						bullets[k].blast = false;
						bullets[k].frameIndex = 0;

						int zwX = 256 - 112 + j * 81;
						int zwY = 179 + i * 102 + 14;
						bullets[k].x = zwX + imgZhiWu[0][0]->getwidth() - 10;
						bullets[k].y = zwY + 5;
						lines[i] = 0;
				}
			}
		}
	}
}

//更新豌豆子弹动作实现
void updateBullets() {
	for (int i = 0; i < bulletMax; i++) {
		if (bullets[i].used) {
			bullets[i].x += bullets[i].speed;
			if (bullets[i].x > WIN_WIDTH) {
				bullets[i].used = false;
			}
			//碰撞播放完就消失
			if (bullets[i].blast) {
				bullets[i].frameIndex++;
				if (bullets[i].frameIndex > 3) {	//超出3会导致状态栏闪烁
					bullets[i].used = false;
				}
			}
		}
	}
}

//检查僵尸碰撞实现
void collisionCheck() {

	//豌豆子弹与僵尸的碰撞检测
	checkBullet2Zm();

	//僵尸与植物的碰撞检测
	checkZm2ZhiWu();

	
}

//豌豆子弹与僵尸的碰撞检测实现
void checkBullet2Zm() {
	for (int i = 0; i < bulletMax; i++) {
		if (bullets[i].used == false || bullets[i].blast)continue;	//如果豌豆子弹没使用或者已经开始碰撞，就跳过
		for (int j = 0; j < zmMax; j++) {
			if (zms[j].used == false)continue;
			int x1 = zms[j].x + 80;
			int x2 = zms[j].x + 110;
			int x = bullets[i].x;
			if (zms[j].dead == false && bullets[i].row == zms[j].row && x > x1 && x < x2) {	//豌豆子弹与僵尸碰撞后
				zms[j].blood -= 10;//5;//10;//20;
				bullets[i].blast = true;
				bullets[i].speed = 0;

				if (zms[j].blood <= 0) {
					zms[j].dead = true;
					zms[j].speed = 0;
					zms[j].frameIndex = 0;
				}
				break;
			}
		}
	}
}

//僵尸与植物的碰撞检测实现
void checkZm2ZhiWu() {
	for (int i = 0; i < zmMax; i++) {
		if (zms[i].dead)continue;
		int row = zms[i].row;
		for (int k = 0; k < 9; k++) {
			if (map[row][k].type == 0)continue;
			int zhiWuX = 256 - 112 + k * 81;	//定义僵尸开吃范围
			int x1 = zhiWuX + 10;
			int x2 = zhiWuX + 60;
			int x3 = zms[i].x + 80;

			if (x3 > x1 && x3 < x2) {
				if (map[row][k].catched==true){	//僵尸吃的过程中的一些配置
					map[row][k].deadTimer++;
					if (map[row][k].deadTimer > 100) {	//僵尸吃完了-重置参数
						map[row][k].deadTimer = 0;
						map[row][k].type = 0;
						zms[i].eating = false;
						zms[i].frameIndex = 0;
						zms[i].speed = 1;
					}
				}
				else {	//僵尸开吃-配置参数
					map[row][k].catched = true;
					map[row][k].deadTimer = 0;
					zms[i].eating = true;
					zms[i].speed = 0;
					zms[i].frameIndex = 0;
				}
			}
		}
	}
}

//创建小推车
void creatcar()
{
	//这里只用到3行小推车；
	int carmax = sizeof(cars) / sizeof(cars[0]);
	for (int i = 0; i < carmax; i++)
	{
			cars[i].x = 60;
			cars[i].y = 100 + (i+1) * 100 + 10;
			cars[i].used = true;
			cars[i].move = false;
	}
}

void updatecar()
{
	
	for (int i = 0; i < carmax; i++)
	{
		if (cars[i].move==true)
		{
			cars[i].x += 20;
		}
		if (cars[i].x > WIN_WIDTH)
		{
			cars[i].move = false;
			cars[i].used = false;
		}
	}
}

void checkcarzm()
{
	for (int i = 0; i < carmax; i++)
	{
		int carsX = cars[i].x + 70;//60+70=130
		for (int j = 0; j < ZM_MAX; j++)
		{
			if (zms[j].used && zms[j].dead == false && zms[j].row == i)
			{
				int zmX = zms[j].x + 80;//38+80=118
				if (carsX > zmX && cars[i].used)
				{
					if (cars[i].move == false) {
						cars[i].move = true;
						/*killZmCount++;
						zms[j].dead = true;
						zms[j].speed = 0;
						zms[j].frameIndex = 0;*/
					}
					else //小推车在运动后再碰到僵尸
					{
						killZmCount++;
						cout << "杀掉的僵尸数：" << killZmCount << endl;
						zms[j].dead = true;
						zms[j].speed = 0;
						zms[j].frameIndex = 0;
					}
				}
			}
		}
	}
}

void creat_front()
{
	creatcar();
	//创建小推车
	//creatcar();
	/*破案了，之前sb逻辑看错了，小推车只在游戏开始时创建一次，之后不再创建，
	如果把其放在updategame()函数里面，会不断更新小推车状态，导致动一次就不再动，再动得僵尸再接触，且无法杀死僵尸*/
	creatshovel();
}

void creatshovel()
{
	memset(&shovel_front, sizeof(shovel_front), 0);
}

void useshovel(ExMessage* msg)
{
	/*putimagePNG(235 + 8 * 65, 0, &imgshovel_frame);
	putimagePNG(235 + 8 * 65, 0, &imgshovel);*/
	int shovel_x1 = (235 + 7 * 65);
	int shovel_x2 = (235 + 7 * 65)+70;
	int shovel_y1 = 0;
	int shovel_y2 = 70;
	/*int x = 343 + i * 65;//植物卡牌坐标
				int y = 6;*/
	int x1 = 163 + ZHI_WU_COUNT * 65;//x1与x2为植物卡牌栏的横坐标
	int x2 = 163;
	int y1 = 6;//y1与y2为植物卡牌栏的纵坐标
	int y2 = 6 + 90;

	if (msg->message == WM_LBUTTONDOWN)
	{
		if (msg->x > shovel_x1 && msg->x<shovel_x2 && msg->y>shovel_y1 && msg->y < shovel_y2)
		{
			if (shovel_front.used == false)
				shovel_front.used = true;
		}
	}
		if (shovel_front.used == true)
		{
			shovel_front.x = msg->x-40;
			shovel_front.y = msg->y-35;
			if (msg->message == WM_RBUTTONDOWN) //鼠标右键按下
			{
				shovel_front.used = false;
			}
			else if(msg->x>x2&&msg->x<x1&&msg->y>y1&&msg->y<y2&&msg->message== WM_LBUTTONDOWN)
			{ 
				shovel_front.used = false;
			}	
			else
			{
				for (int i = 0; i < 3; i++)
				{
					for (int j = 0; j < 8; j++)
					{
						//180,230为植物地图左上角
						//map[row][col].x = 256 - 112 + col * 81;	//植物坐标
						//map[row][col].y = 179 + row * 102 + 14;
						int plant_x1 = map[i][j].x;
						int plant_x2 = map[i][j].x+81;
						int plant_y1 = map[i][j].y;
						int plant_y2 = map[i][j].y+102;
						if (msg->x > plant_x1&&msg->x<plant_x2&&msg->y>plant_y1&&msg->y < plant_y2&&msg->message == WM_LBUTTONDOWN)
						{
							map[i][j].type = 0;
							shovel_front.used = false;
						}
					}
				}
			}
		}
	
}

void clickpause(ExMessage* msg)
{
	static bool pause_type[3] = { false,false,false };

	//初始化暂停的图标，没找到合适的图片，随便凑活凑活
	//putimagePNG((235 + 8 * 65), 25, flag == 0 ? imgpause[2] : imgpause[3]);
	int pause_x1 = (235 + 8 * 65);
	int pause_x2 = (235 + 8 * 65) + 247;
	int pause_y1 = 25;
	int pause_y2 = 25+46;
    {	
			if (msg->message == WM_LBUTTONDOWN &&	//鼠标左键落下		扩展：当鼠标经过时也可以高亮
				msg->x > pause_x1 && msg->x<pause_x2 && msg->y>pause_y1 && msg->y < pause_y2)
			{
				//flag = 1;
				cout << "暂停"<<endl;
				bool isRunning = true; // 添加一个布尔变量来控制循环的执行
				while (isRunning) // 使用布尔变量作为循环条件
				{
					update_pause(pause_type);
					//putimagePNG((235 + 8 * 65), 25, imgpause[3]);//直接覆盖图片，待优化  主菜单按下时的图片
					
					/*此处不能用上面的msg鼠标，应该那个在每次调用clickpause函数时数据才会更新，而此处循环内已经不会更新了，
					所以在此处创建一个新的鼠标变量来实现退出while循环，取消暂停*/
					ExMessage msg1;
					if (peekmessage(&msg1)) 
					{
						pause_click( &msg1,isRunning,pause_type);
						
					}
					
				}
			}
			
		//else if (msg.message == WM_LBUTTONUP && flag == 1) {	//鼠标左键抬起
		//	EndBatchDraw();
		//	return;
		//}
	}
}

void game_save()
{
	//把几个池子的数据全部存入，再存入几个定义的全局变量
	ofstream outfile("preserve.txt");
	if (!fileExist("preserve.txt"))
		return;
	
	outfile << sunShine << " " << killZmCount << " "<<gameStatus<<endl;

	//阳光数据
	for (int i = 0; i < ballMax; i++)
	{
		outfile <<  balls[i].destY << " " << balls[i].frameIndex << " " <<
					balls[i].p1.x  << " " << balls[i].p1.y  << " " <<
					balls[i].p2.x  << " " << balls[i].p2.y  << " " <<
					balls[i].p3.x  << " " << balls[i].p3.y  << " " <<
					balls[i].p4.x  << " " << balls[i].p4.y  << " " <<
					balls[i].pCur.x << " " << balls[i].pCur.y << " " <<
					balls[i].speed << " " << balls[i].status << " " <<
					balls[i].t     << " " << balls[i].timer << " " <<
					balls[i].used  << " " << balls[i].x << " " <<
					balls[i].xoff  << " " << balls[i].y << " " <<
					balls[i].yoff;
		outfile << endl;
	}

	//植物数据
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			outfile << map[i][j].catched << " " << map[i][j].deadTimer << " "
					<< map[i][j].frameIndex << " " << map[i][j].shootTimer << " "
					<< map[i][j].timer << " " << map[i][j].type << " "
					<< map[i][j].x << " " << map[i][j].y<<" ";
			outfile << endl;
		}
	}

	//僵尸数据
	for (int i = 0; i < zmMax; i++)
	{
		outfile << zms[i].blood << " " << zms[i].dead << " "
				<< zms[i].eating << " " << zms[i].frameIndex << " "
				<< zms[i].row << " " << zms[i].speed << " "
				<< zms[i].used << " " << zms[i].x << " "
				<< zms[i].y << " ";
		outfile << endl;
	}

	//小推车数据
	for (int i = 0; i < carmax; i++)
	{
		outfile << cars[i].move << " " << cars[i].used << " "
				<< cars[i].x << " " << cars[i].y << " ";
		outfile << endl;
	}

	//豌豆子弹数据
	for (int i = 0; i < bulletMax; i++)
	{
		outfile << bullets[i].blast << " " << bullets[i].frameIndex << " "
				<< bullets[i].row << " " << bullets[i].speed << " "
				<< bullets[i].used << " " << bullets[i].x << " "
				<< bullets[i].y << " ";
		outfile << endl;
	}

	outfile.close();
}

void read_archive()
{
	ifstream infile;
	infile.open("preserve.txt");

	if (!fileExist("preserve.txt"))
		return;

	infile >> sunShine >> killZmCount >> gameStatus ;

	//阳光数据
	for (int i = 0; i < ballMax; i++)
	{
		infile >> balls[i].destY >> balls[i].frameIndex >>
			balls[i].p1.x >> balls[i].p1.y  >>
			balls[i].p2.x >> balls[i].p2.y  >>
			balls[i].p3.x >> balls[i].p3.y  >>
			balls[i].p4.x >> balls[i].p4.y  >>
			balls[i].pCur.x >> balls[i].pCur.y  >>
			balls[i].speed >> balls[i].status  >>
			balls[i].t >> balls[i].timer >>
			balls[i].used >> balls[i].x  >>
			balls[i].xoff >> balls[i].y  >>
			balls[i].yoff;
	}

	//植物数据
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			infile >> map[i][j].catched >> map[i][j].deadTimer
				>> map[i][j].frameIndex >> map[i][j].shootTimer
				>> map[i][j].timer >> map[i][j].type 
				>> map[i][j].x >> map[i][j].y ;
		}
	}

	//僵尸数据
	for (int i = 0; i < zmMax; i++)
	{
		infile >> zms[i].blood >> zms[i].dead
			>> zms[i].eating >> zms[i].frameIndex
			>> zms[i].row >> zms[i].speed
			>> zms[i].used >> zms[i].x
			>> zms[i].y ;
		
	}

	//小推车数据
	for (int i = 0; i < carmax; i++)
	{
		infile >> cars[i].move >> cars[i].used
			>> cars[i].x >> cars[i].y ;
		
	}

	//豌豆子弹数据
	for (int i = 0; i < bulletMax; i++)
	{
		infile >> bullets[i].blast >> bullets[i].frameIndex
			>> bullets[i].row >> bullets[i].speed
			>> bullets[i].used >> bullets[i].x
			>> bullets[i].y ;
		
	}

	infile.close();
}

void gameinit_pause()
{
	//总共9张，5组图片，每两张为对应的按下与未按下的图片，一张为墓碑图
	//0.1为“主菜单”图片；2,3为“菜单”图片；4,5,为“返回游戏”图片；6,7为“重新开始本关卡”图片，9为墓碑图
	char name[64];
	memset(name, 0, sizeof(name));
	//IMAGE *imgpause[10]
	imgpause[9] = new IMAGE;
	loadimage(imgpause[9], "res/Screen/pause_0.png");
	loadimage(&imgtombstone, "res/Screen/pause_0.png");

	for (int i = 0; i < 2; i++)//主菜单
	{
		sprintf_s(name, sizeof(name), "res/Screen/mainmenu%d.jpg", i+1);
		if (fileExist(name)) {
			imgpause[i] = new IMAGE;
			loadimage(imgpause[i], name);
		}
		else {
			break;
		}
		//loadimage(imgpause[i], name);
	}
	for (int i = 0; i < 2; i++)//菜单
	{
		sprintf_s(name, sizeof(name), "res/Screen/pause_menu%d.jpg", i + 1);
		if (fileExist(name)) {
			imgpause[i+2] = new IMAGE;
			loadimage(imgpause[i+2], name);
		}
		else {
			break;
		}
		//loadimage(imgpause[i+2], name);
	}
	for (int i = 0; i < 2; i++)//返回游戏
	{
		sprintf_s(name, sizeof(name), "res/Screen/reback%d.jpg", i + 1);
		if (fileExist(name)) {
			imgpause[i + 4] = new IMAGE;
			loadimage(imgpause[i + 4], name);
		}
		else {
			break;
		}
	}
	for (int i = 0; i < 2; i++)//重新开始此关卡
	{
		sprintf_s(name, sizeof(name), "res/Screen/restart%d.jpg", i + 1);
		if (fileExist(name)) {
			imgpause[i + 6] = new IMAGE;
			loadimage(imgpause[i + 6], name);
		}
		else {
			break;
		}
	}
}

void update_pause(bool*type)
{
	BeginBatchDraw();
	//0.1为“主菜单”图片；2,3为“菜单”图片；4,5,为“返回游戏”图片；6,7为“重新开始本关卡”图片，9为墓碑图
	putimagePNG((235 + 8 * 65), 25, imgpause[3]);//直接覆盖图片，待优化  主菜单按下时的图片

	//putimagePNG(238, 50, imgpause[9]);//墓碑菜单 使用这个渲染会访问冲突
	putimagePNG(238, 50, &imgtombstone);//墓碑菜单

	putimagePNG(420 - 90, 450 - 125, type[0] ? imgpause[7]: imgpause[6]);

	putimagePNG(420-94, 450-125+50, type[1] ? imgpause[1] : imgpause[0]);

	putimagePNG(360-80, 450 - 125 + 100+22, type[2] ? imgpause[5] : imgpause[4]);

	EndBatchDraw();
}

void pause_click(ExMessage *msg,bool &isRunning, bool*type)
{
	if (msg->x > 360 - 80 && msg->x < 360 - 80 + 336 && msg->y>450 - 125 + 100 + 22 && msg->y < 450 - 125 + 100 + 22 + 75)//返回游戏的图标
	{
		type[2] = true;
		if (msg->message == WM_LBUTTONDOWN) {
			game_save();
			//flag = 0;
			isRunning = false; // 将布尔变量设置为false，退出循环
			cout << "继续游戏" << endl;
		}
	}
	else type[2] = false;

	if (msg->x > 420 - 94 && msg->x < 420 - 94+247 && msg->y>450 - 125 + 50 && msg->y < 450 - 125 + 50+46)//返回主菜单的图标
	{
		type[1] = true;
		if (msg->message == WM_LBUTTONDOWN) 
		{
			game_save();
			longjmp(jmpbuffer, 1);
		}
	}
	else type[1] = false;

	if (msg->x > 420 - 90 && msg->x < 420 - 90 + 241 && msg->y>450 - 125  && msg->y < 450 - 125 + 50 )//重新开始关卡的图标
	{
		type[0] = true;
		if (msg->message == WM_LBUTTONDOWN)
		{
			read_regame();
			isRunning = false; // 将布尔变量设置为false，退出循环
			cout << "继续游戏" << endl;
			longjmp(jmpbuffer_two, 1);
		}
	}
	else type[0] = false;

	
}

void read_regame()
{
	sunShine = 50;
	killZmCount = gameStatus = 0;

	memset(balls, 0, sizeof(balls));
	memset(map, 0, sizeof(map));
	memset(zms, 0, sizeof(zms));
	//memset(cars, 0, sizeof(cars));
	creatcar();
	memset(bullets, 0, sizeof(bullets));
}