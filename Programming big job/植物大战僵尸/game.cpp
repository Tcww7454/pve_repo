#include"game.h"

jmp_buf jmpbuffer;
jmp_buf jmpbuffer_two;
jmp_buf jmpbuffer_three;

IMAGE imgBg;	//��Ϸ����ͼ
IMAGE imgBar;	//״̬������ֲ��ı�����
IMAGE imgCards[ZHI_WU_COUNT];	//ֲ�￨������
IMAGE* imgZhiWu[ZHI_WU_COUNT][20];	//ֲ������ 
IMAGE imgpause_one;
IMAGE imgpause_two;
IMAGE *imgpause[9];//�ܹ�9��ͼƬ��͵������ע��ÿ��Ԫ�ض�Ӧ����ͼƬ�ɣ�ʹ�����ַ�����ȾĹ��ͼƬ����ʳ�ͻ
IMAGE imgtombstone;
IMAGE *imgChomperAttack[18];//ʳ�˻��Խ�ʬ18֡ͼƬ
IMAGE *imgChomperDigest[18];//ʳ�˻�������ʬ18֡ͼƬ
IMAGE *imgloadacount[4];//ϵͳ�ĵ�¼(ûʱ��д�ˣ��׸�Ƥ)

int curX, curY;	//��ǰѡ��ֲ�����ƶ��е�����
int curZhiWu;	//��ǰѡ�е�ֲ��	0-û��ѡ�У�1-ѡ�е�һ��ֲ��
int killZmCount;	//ɱ���Ľ�ʬ����
int zmCount;		//���ɵĽ�ʬ����
int gameStatus;		//��Ϸ��״̬

struct zm zms[10];	//��ʬ��,�������ȴ洢��ʬ
IMAGE	imgZm[22];	//imgzm���ܵ� ״̬���飬�������治ͬ״̬�µĽ�ʬ��ͼƬ�����������¼���״̬
IMAGE	imgZmDead[20];//��ʬ����
IMAGE	imgZmEat[21];//��ʬ��
IMAGE	imgZmStand[11];//ƬͷѲ����ʬվ��

 struct sunShineBall	balls[10];	//������أ��������ȴ洢����
 IMAGE	imgSunShineBall[29];	//��������֡����	-	���Զ���һ���꣬������ڹ���

struct bullet bullets[30];	//�㶹�ӵ���
IMAGE imgBulletNormal;
IMAGE imgBulletBlast[4];

struct car cars[3];
IMAGE imgCAR;

IMAGE imgshovel;
IMAGE imgshovel_frame;
shovel shovel_front;

int ballMax = sizeof(balls) / sizeof(balls[0]);	//����������������	-�ǲ��Ǻ���֣����������Ѿ�������10��
												//���ﻹҪ����һ�飬���ʣ��ʾ��Ǹ߿���(���˽����10����һ���궨�壬���Ӹ߿���)
int zmMax = sizeof(zms) / sizeof(zms[0]);	//��ʬ���н�ʬ������

int bulletMax = sizeof(bullets) / sizeof(bullets[0]);	//�㶹�ӵ��ص�����

int carmax = sizeof(cars) / sizeof(cars[0]);//С�Ƴ�����

struct zhiWu plant_map[3][9];	//��ͼ���飬����洢ֲ��

int sunShine=50;	//����ֵ

//��Ϸ��ʼ��ʵ��
void gameInit() {

	//�����������
	srand(time(NULL));

	//������Ϸ����ͼƬ
	loadimage(&imgBg, "res/bg.jpg");

	//����״̬��
	loadimage(&imgBar, "res/bar5.png");

	killZmCount = 0;
	zmCount = 0;
	gameStatus = GOING;

	memset(imgZhiWu, 0, sizeof(imgZhiWu));	//��ָ�븳��ֵ

	memset(plant_map, 0, sizeof(plant_map));	//��ʼ����ͼ����

	memset(balls, 0, sizeof(balls));	//��ʼ�������

	memset(zms, 0, sizeof(zms));	//��ʼ����ʬ��

	memset(bullets, 0, sizeof(bullets));	//��ʼ���㶹�ӵ���

	//����ֲ�￨��
	char	name[64];
	for (int i = 0; i < ZHI_WU_COUNT; i++) {
		//����ֲ�￨�Ƶ��ļ���
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		loadimage(&imgCards[i], name);
		for (int j = 0; j < 20; j++) {	//20�ǹ̶�ֵ������Ѱ���ƥ��ķ�ʽ
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png", i, j + 1);
			//�ж��ļ��Ƿ����
			if (fileExist(name)) {
				imgZhiWu[i][j] = new IMAGE;
				loadimage(imgZhiWu[i][j], name);
			}
			else {
				break;
			}
		}
	}

	//��ʼ��ѡ��ֲ��
	curZhiWu = 0;

	//��ʼ������ֵ
	sunShine = 50;



	//��������
	for (int i = 0; i < 29; i++) {	//29�ǹ̶�ֵ������Ѱ���ƥ��ķ�ʽ
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i + 1);
		loadimage(&imgSunShineBall[i], name);
	}

	//���ؽ�ʬͼƬ
	for (int i = 0; i < 22; i++) {
		sprintf_s(name, sizeof(name), "res/zm/%d.png", i + 1);
		loadimage(&imgZm[i], name);
	}

	//���ؽ�ʬ����ͼƬ
	for (int i = 0; i < 20; i++) {
		sprintf_s(name, sizeof(name), "res/zm_dead/%d.png", i + 1);
		loadimage(&imgZmDead[i], name);
	}

	//���ؽ�ʬ��ֲ��ͼƬ
	for (int i = 0; i < 21; i++) {
		sprintf_s(name, sizeof(name), "res/zm_eat/%d.png", i + 1);
		loadimage(&imgZmEat[i], name);
	}

	//����Ѳ����ʬͼƬ
	for (int i = 0; i < 11; i++) {
		sprintf_s(name, sizeof(name), "res/zm_stand/%d.png", i + 1);
		loadimage(&imgZmStand[i], name);
	}

	//�����㶹�ӵ�ͼƬ
	loadimage(&imgBulletNormal, "res/bullets/bullet_normal.png");

	//�����㶹�ӵ���ըͼƬ
	loadimage(&imgBulletBlast[3], "res/bullets/bullet_blast.png");
	for (int i = 0; i < 3; i++) {
		float	k = (i + 2) * 0.2;
		loadimage(&imgBulletBlast[i], "res/bullets/bullet_blast.png",
			imgBulletBlast[3].getwidth() * k, imgBulletBlast[3].getheight() * k, true);
	}

	//����С�Ƴ�ͼƬ
	loadimage(&imgCAR, "res/Screen/car.png");

	//������Ϸ����
	initgraph(WIN_WIDTH, WIN_HEIGHT, 1/*,1*/);	//����1��ʾ�ٿ�һ������̨����	

	//��������
	LOGFONT	f;
	gettextstyle(&f);
	f.lfHeight = 30;
	f.lfWeight = 15;
	strcpy(f.lfFaceName, "Segoe UI Black");	//��������Ч��
	f.lfQuality = ANTIALIASED_QUALITY;		//�����
	settextstyle(&f);
	setbkmode(TRANSPARENT);					//����ģʽ������͸��
	setcolor(BLACK);						//������ɫ����ɫ


	//���ӿ��Լ�����ͼƬ
	loadimage(&imgshovel, "res/Screen/shovel.png");
	loadimage(&imgshovel_frame, "res/Screen/shovelSlot.png", imgshovel_frame.getwidth() * 2, imgshovel_frame.getheight() * 2, true);

	//��ʼ����ͣ��ͼ�꣬û�ҵ����ʵ�ͼƬ�����ջ�ջ�
	loadimage(&imgpause_one, "res/pause/SelectorScreen_Quit1.png");//δ����ʱΪ��ɫ
	loadimage(&imgpause_two, "res/pause/SelectorScreen_Quit2.png");//����ʱΪ��ɫ

	//��ʼ��9����ͣͼƬ
	gameinit_pause();

	//��ʼ��ʳ�˻���ͼƬ
	gameinit_shirenhua();

	//��װһ��ֻ��Ҫ����Ϸ��ʼʱ��������Ʒ�ĺ���������ֻ��С�Ƴ�����ӵĴ�������
	creat_front();

	

	/*loadimage(&imgBulletBlast[i], "res/bullets/bullet_blast.png",
		imgBulletBlast[3].getwidth() * k, imgBulletBlast[3].getheight() * k, true);*/
}

//��Ϸ��ʼ����ʵ��
void startUI() {
	IMAGE imgMenu, imgMenu1, imgMenu2, imgcontinue1, imgcontinue2,imgsmallgame1,imgsmallgame2;
	int	flag1 = 0, flag2 = 0,flag3=0;//��Ϊ��ʼ��Ϸ��ͼ�꣬2Ϊ�����Ͼ���Ϸ��ͼ��
	int flag4 = 0,flag5=0;
	loadimage(&imgMenu, "res/menu.png");	//���ؿ�ʼ����ͼ
	loadimage(&imgMenu1, "res/menu1.png");
	loadimage(&imgMenu2, "res/menu2.png");
	loadimage(&imgcontinue1, "res/Screen/continue1.png");
	loadimage(&imgcontinue2, "res/Screen/continue2.png");
	loadimage(&imgsmallgame1, "res/reanim/SelectorScreen_Survival_highlight.png");
	loadimage(&imgsmallgame2, "res/reanim/SelectorScreen_Survival_button.png");
	//��¼ϵͳ����ͼƬ�ĳ�ʼ��
	init_loadimage();

	while (1)
	{
		BeginBatchDraw();
		//flag1 = 0, flag2 = 0;
		putimage(0, 0, &imgMenu);	//��Ⱦ��ʼ����ͼ��������
		putimagePNG(474, 75, flag1 == 0 ? &imgMenu2 : &imgMenu1);
		putimagePNG(484, 300, flag2 == 0 ? &imgcontinue2 : &imgcontinue1);
		putimagePNG(469, 75+120, flag3 == 0 ? &imgsmallgame1 : &imgsmallgame2);
		putimagePNG(60, 0, imgloadacount[1]);
		putimagePNG(60, 140, flag4 == 1 ? imgloadacount[2] : imgloadacount[3]);
		//putimagePNG(60, 70, imgloadacount[0]);

		setcolor(character_color);						//�˻�������ɫ
		
		outtextxy(110, 86, "Tcww");
		
		setcolor(BLACK);

		ExMessage	msg;
		if (peekmessage(&msg)) {
			if (msg.x > 474 && msg.x < 774 && msg.y>75 && msg.y < 215)flag1 = 1; else flag1 = 0;
			if (msg.x > 484 && msg.x < 484 + 286 && msg.y>300 && msg.y < 300 + 122)flag2 = 1; else flag2 = 0;
			if (msg.x > 469 && msg.x < 469 + 313 && msg.y>195 && msg.y < 195 + 103)flag3 = 1; else flag3 = 0;
			if (msg.x > 60 && msg.x < 60 + 290 && msg.y>140 && msg.y < 140 + 70)flag4 = 1; else flag4 = 0;

			if (msg.message == WM_LBUTTONDOWN &&	//����������		��չ������꾭��ʱҲ���Ը���
				msg.x > 474 && msg.x < 774 && msg.y>75 && msg.y < 215) {
				flag1 = 1;
			}
			else if (msg.message == WM_LBUTTONDOWN &&	//����������		��չ������꾭��ʱҲ���Ը��� 286 122
				msg.x > 484 && msg.x < 484 + 286 && msg.y>300 && msg.y < 300 + 122) {
				flag2 = 1;

			}
			else if (msg.message == WM_LBUTTONDOWN &&	//����������		��չ������꾭��ʱҲ���Ը��� 286 122
				msg.x > 469 && msg.x < 469 + 313 && msg.y>195 && msg.y < 195 + 103) {
				flag3 = 1;
			}
			else if (msg.message == WM_LBUTTONDOWN &&	//��¼���˺Ŵ���
				msg.x > 60 && msg.x < 60 + 290 && msg.y>140 && msg.y < 140 + 70) {
				//putimagePNG(280, 150, imgloadacount[0]);
				flag4 = 1;
				flag5 = 1; 
				
				account_manage();
			}
		}
		//WM_LBUTTONUP
		else if (msg.message == WM_LBUTTONDOWN) {	//������̧���������ò��û������ͻ���򵥸ĳɰ��£����Ż�
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
				flag1 = flag2 = 0;
			}
		}
		EndBatchDraw();
	}
}

//��������Ѳ��ʵ��
void viewScence() {
	int xMin = WIN_WIDTH - imgBg.getwidth();
	vector2 points[9] = { {550,80},{530,160},{630,170},{530,200},{525,270},		//9����ʬվλ
		{565,370},{605,340},{705,280},{690,340} };
	int index[9];
	for (int i = 0; i < 9; i++) {
		index[i] = rand() % 11;
	}
	int count = 0;
	for (int x = 0; x >= xMin; x -= 4) {	//ÿ֡�ƶ�2����
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

	//ͣ��1s
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


	//������
	for (int x = xMin; x <= -112; x += 4) {	//ÿ֡�ƶ�2����
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

//״̬���»�ʵ�ַϰ�
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
//		 putimage(0,0,&imgBg);        //��ͼƬ��Ⱦ����
//		putimagePNG(250, 0, &imgBar);  //����͸����
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
	//���Ʋ�IMAGE imgBar;
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

//��ͼƬ���ص�������(��Ⱦ)
void updateWindow()
{

	BeginBatchDraw();

	//putimage(-112, 0, &imgBg);	//����(��Ⱦ)������
	//putimagePNG(255, 0, &imgBar);	//����(��Ⱦ)״̬��
	putimage(-112, 0, &imgBg);
	putimagePNG(80, -10, &imgBar);

	//for (int i = 0; i < ZHI_WU_COUNT; i++) {	//����(��Ⱦ)ֲ�￨��
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

	//�ڵ�ͼ�ϼ���(��Ⱦ)ֲ��
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (plant_map[i][j].type > 0) {
				//int x = 256 + j * 81;
				//int y = 179 + i * 102 + 14;
				int zhiWuType = plant_map[i][j].type - 1;
				int index = plant_map[i][j].frameIndex;
				//ͼƬ��С���µ�yֵ���ʵ���������Ȼ��Ҳ����ȥ�ü�ͼƬ()
				if (plant_map[i][j].type == SHI_REN_HUA + 1 && plant_map[i][j].eating == true)
					putimagePNG(plant_map[i][j].x, plant_map[i][j].y - 20, imgChomperAttack[index]);
				else if (plant_map[i][j].type == SHI_REN_HUA + 1 && plant_map[i][j].digest == true)
					putimagePNG(plant_map[i][j].x, plant_map[i][j].y - 20, imgChomperDigest[index]);
				else if (plant_map[i][j].type == SHI_REN_HUA + 1)
					putimagePNG(plant_map[i][j].x, plant_map[i][j].y - 20, imgZhiWu[zhiWuType][index]);
				//putimagePNG(x, y, imgZhiWu[zhiWuType][index]);
				else
					putimagePNG(plant_map[i][j].x, plant_map[i][j].y, imgZhiWu[zhiWuType][index]);
			}
		}
	}

	//��Ⱦ����
	if (shovel_front.used == false) {
		putimagePNG(235 + 7 * 65, 0, &imgshovel_frame);
		putimagePNG(235 + 7 * 65, 0, &imgshovel);
	}
	else if (shovel_front.used == true)
	{
		putimagePNG(235 + 7 * 65, 0, &imgshovel_frame);
		putimagePNG(shovel_front.x, shovel_front.y, &imgshovel);
	}

	//����(��Ⱦ)�϶���ֲ��
	if (curZhiWu > 0) {
		IMAGE* img = imgZhiWu[curZhiWu - 1][0];
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, img);
	}

	//for (int i = 0; i < ballMax; i++) {	//��ʱ��Ⱦ�ᱻ��ʬ��ס
	//	if (balls[i].used || balls[i].xoff) {	//����(��Ⱦ)����
	//		IMAGE* img = &imgSunShineBall[balls[i].frameIndex];
	//		putimagePNG(balls[i].x, balls[i].y, img);
	//	}
	//}

	//����(��Ⱦ)����ֵ
	char scoreText[8];
	sprintf_s(scoreText, sizeof(scoreText), "%d", sunShine);	//������ֵת�����ַ�����
	outtextxy(110, 60, scoreText);			//��Ⱦ���	λ�ÿɵ����ɾ���,����ʹ�ù̶�ֵy	255-283 80-108

	//����(��Ⱦ)��ʬ
	for (int i = 0; i < zmMax; i++) {
		if (zms[i].used) {
			//IMAGE* img = &imgZm[zms[i].frameIndex];
			//IMAGE* img = (zms[i].dead) ? imgZmDead : imgZm;

			IMAGE* img = NULL;
			if (zms[i].eating) {
				img = imgZmEat;
			}
			else if (zms[i].dead) {
				img = imgZmDead;
			}
			else {
				img = imgZm;
			}

			img += zms[i].frameIndex;
			putimagePNG(zms[i].x, zms[i].y - img->getheight(), img);
		}
	}

	//����(��Ⱦ)�㶹�ӵ�
	for (int i = 0; i < bulletMax; i++) {
		if (bullets[i].used) {
			if (bullets[i].blast) {	//�㶹�ӵ���ײ��Ⱦ
				IMAGE* img = &imgBulletBlast[bullets[i].frameIndex];
				//IMAGE* img = &imgBulletBlast[];
				putimagePNG(bullets[i].x, bullets[i].y, img);
				FlushBatchDraw();
			}
			else {	//�㶹�ӵ���ͨ��̬��Ⱦ
				putimagePNG(bullets[i].x, bullets[i].y, &imgBulletNormal);
			}
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

	//����(��Ⱦ)����
	for (int i = 0; i < ballMax; i++) {
		if (balls[i].used /* || balls[i].xoff*/) {
			IMAGE* img = &imgSunShineBall[balls[i].frameIndex];
			//putimagePNG(balls[i].x, balls[i].y, img);
			putimagePNG(balls[i].pCur.x, balls[i].pCur.y, img);
		}
	}

	EndBatchDraw();
}

//�û����ʵ��
void userClick() {
	static	int status = 0;
	ExMessage	msg;
	if (peekmessage(&msg)) {	//�ж��û��Ƿ��в���
		useshovel(&msg);
		clickpause(&msg);
		if (msg.message == WM_LBUTTONDOWN) {	//����������
			if (msg.x > 163 && msg.x < 163 + 65 * ZHI_WU_COUNT && msg.y < 96) {	//������Ƶ��¼�
				int index = (msg.x - 163) / 65;

				//�ж�����ֵ�Ƿ��㹻����ֲ��
				//���Ż���Ӧ�����º��ټ���		��ֲ������������ʱ
				if (index == XIANG_RI_KUI) {
					if (sunShine >= 50) {
						status = 1;
						curZhiWu = index + 1;
						//ʹֲ����ʾ�ڵ��λ�ã�������ֲ��������ϴ���ʧλ�õ�Сbug
						curX = msg.x;
						curY = msg.y;
						//sunShine -= 50;
					}
				}
				else if (index == WAN_DAO) {
					if (sunShine >= 100) {
						status = 1;
						curZhiWu = index + 1;
						//ʹֲ����ʾ�ڵ��λ�ã�������ֲ��������ϴ���ʧλ�õ�Сbug
						curX = msg.x;
						curY = msg.y;
						//sunShine -= 100;
					}
				}
				else if (index == SHI_REN_HUA) {
					if (sunShine >= 150) {
						status = 1;
						curZhiWu = index + 1;
						//ʹֲ����ʾ�ڵ��λ�ã�������ֲ��������ϴ���ʧλ�õ�Сbug
						curX = msg.x - 25;//��ͼƬ��С���µ������
						curY = msg.y - 60;
						//sunShine -= 150;
					}
				}
				else if (index == CHERRY)//ӣ��
				{
					if (sunShine >= 150)
					{
						status = 1;
						curZhiWu = index + 1;
						curX = msg.x;
						curY = msg.y;
						//sunShine -= 150;
					}
				}
				else if (index == WALL_NUT)//ӣ��
				{
					if (sunShine >= 50)
					{
						status = 1;
						curZhiWu = index + 1;
						curX = msg.x;
						curY = msg.y;
						//sunShine -= 50;
					}
				}
				//�����Ļ��д�����

				//status = 1;
				//ʹֲ����ʾ�ڵ��λ�ã�������ֲ��������ϴ���ʧλ�õ�Сbug
				//curX = msg.x;
				//curY = msg.y;
			}
			else {	//�ռ������¼�
				collectSunshine(&msg);
			}
		}
		else if (msg.message == WM_MOUSEMOVE && status == 1) {	//����ƶ�
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message == WM_RBUTTONDOWN && status == 1) {	//����Ҽ�����
			if (msg.x > 256 - 112 && msg.x < 900 - 30 && msg.y > 179 && msg.y < 489) {
				int	row = (msg.y - 179) / 102;	//��ȡ��
				int	col = (msg.x - 256 + 112) / 81;	//��ȡ��
				if (plant_map[row][col].type == 0) {
					plant_map[row][col].type = curZhiWu;	//����굱ǰ������ֲ��
					plant_map[row][col].frameIndex = 0;	//��Ⱦֲ���һ֡
					plant_map[row][col].shootTimer = 0;	//��ʼ������ʱ��

					plant_map[row][col].x = 256 - 112 + col * 81;	//ֲ������
					plant_map[row][col].y = 179 + row * 102 + 14;
					plant_map[row][col].eating = false;
					plant_map[row][col].digest = false;
					plant_map[row][col].digest_timer = 0;
					plant_map[row][col].catchzm = -1;
					plant_map[row][col].deadTimer = 0;
					//enum { WAN_DAO, XIANG_RI_KUI, SHI_REN_HUA, CHERRY, WALL_NUT, ZHI_WU_COUNT };	//ֲ��ö��
					//������ֵ������ֲ����
					if (plant_map[row][col].type == WAN_DAO + 1)	sunShine -= 100;
					else if (plant_map[row][col].type == XIANG_RI_KUI + 1)	sunShine -= 50;
					else if (plant_map[row][col].type == SHI_REN_HUA + 1)	sunShine -= 150;
					else if (plant_map[row][col].type == CHERRY + 1)	sunShine -=150;
					else if (plant_map[row][col].type == WALL_NUT + 1)	sunShine -= 50;
				}
			}
			//ʹֲ���ͷ���ʧ
			curZhiWu = 0;
			status = 0;
			//����ֲ�������
			//curX = 1000;
			//curY = 1000;
		}
	}
}

//�ж��ļ��Ƿ����ʵ��
bool fileExist(const char* name) {
	FILE* fp = fopen(name, "r");
	if (fp == NULL) {
		return false;
	}
	else {
		fclose(fp);
		return true;
	}
}

//���¶���ʵ��
void updateGame() {

	//����ֲ�ﶯ��
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (plant_map[i][j].type > 0) {
				if (plant_map[i][j].type == SHI_REN_HUA + 1 && plant_map[i][j].eating == true) chomper_eating(i, j);
				else if (plant_map[i][j].type == SHI_REN_HUA + 1 && plant_map[i][j].eating == false && plant_map[i][j].digest == true)  chomper_digest(i, j);
				else if (plant_map[i][j].type == CHERRY + 1) cherry_boom(i, j);
				else{
					/*static int count = 0;
					count++;
					if (count > 1)
					{
						count = 0;
						
					}*/
					plant_map[i][j].frameIndex++;
					/*plant_map[i][j].frameIndex--;
					plant_map[i][j].frameIndex++;*/

					int	zhiWuType = plant_map[i][j].type - 1;
					int	index = plant_map[i][j].frameIndex;
					if (imgZhiWu[zhiWuType][index] == NULL) {
						plant_map[i][j].frameIndex = 0;
					}

				}
			}
		}
	}

	//���½�ʬ����
	updateZm();

	//С�Ƴ��뽩ʬ����ײ���
	checkcarzm();

	//����С�Ƴ�
	updatecar();

	//������ʬ
	createZm();

	//��������
	createSunShine();

	//�������⶯��
	updateSunShine();

	//�����㶹�ӵ�
	createBullets();	//�㶹����֮��Ƶ�ʲ�һ��

	//�����㶹�ӵ�����
	updateBullets();

	//�㶹�ӵ��뽩ʬ��ײ
	collisionCheck();

	
}

//�����Ϸ�Ƿ����ʵ��
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

//��������ʵ��
void createSunShine() 
{
	static	int	count = 0;
	static	int	fre = 500;
	count++;

	if (count >= fre) {	//�����������ɵ��ٶ�

		fre = 300 + rand() % 150;	//�ڶ������������ʱ�����
		count = 0;

		int i;

		//���������ȡ�����õ�����
		for (i = 0; i < ballMax && balls[i].used; i++);	
		if (i >= ballMax)return;

		balls[i].used = true;
		balls[i].frameIndex = 0;
		//balls[i].x = 260 + rand() % (900 - 320);	//������
		//balls[i].y = 60;
		//balls[i].destY = 200 + (rand() % 4) * 90;	//���ֹͣλ��
		//balls[i].xoff = 0;
		//balls[i].yoff = 0;
		balls[i].timer = 0;

		//�Ż�
		balls[i].status = SUNSHINE_DOWN;
		balls[i].t = 0;
		balls[i].p1 = vector2(260 - 112 + rand() % (900 - 320 + 112), 60);
		balls[i].p4 = vector2(balls[i].p1.x, 200 + (rand() % 4) * 90);
		int off = 2;
		float distance = balls[i].p4.y - balls[i].p1.y;
		balls[i].speed = 1.0 / (distance / off);
	}

	//���տ���������
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (plant_map[i][j].type == XIANG_RI_KUI + 1) {
				plant_map[i][j].timer++;
				if (plant_map[i][j].timer > sunshine_product_speed) {//���Ĵ˴��ĺ궨����Ըı��������ٶ�
					plant_map[i][j].timer = 0;

					int k;
					for (k = 0; k < ballMax && balls[k].used; k++);
					if (k >= ballMax)return;

					balls[k].used = true;

					balls[k].p1 = vector2(plant_map[i][j].x, plant_map[i][j].y);	//���ñ��������ߵĲ���
					int w = (50 + rand() % 51) * (rand() % 2 ? 1 : -1);//�����������������
					balls[k].p4 = vector2(plant_map[i][j].x + w, plant_map[i][j].y + imgZhiWu[XIANG_RI_KUI][0]->getheight()
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

//�������⶯��ʵ��
void updateSunShine() {
	for (int i = 0; i < ballMax; i++) {
		if (balls[i].used) {
			balls[i].frameIndex = (balls[i].frameIndex + 1) % 29;	//��������֡

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
				if (balls[i].timer > 50) {//�����Զ���ʧ���ռ���ʱ��
					//ͨ�����Ĵ˴�����ѡ���Ƿ�����Զ��ռ�����
					{//���⵽ʱ����ʧ
						balls[i].used = false;
						balls[i].timer = 0;
					}
					//�Զ��ռ�
						/*balls[i].timer = 0;
						balls[i].status = SUNSHINE_COLLECT;*/

				}
			}
			else if (balls[i].status == SUNSHINE_COLLECT) {
				struct sunShineBall* sun = &balls[i];
				sun->t += sunshine_collect_speed;//ͨ������sunshine_collect_speed�Ĵ�С�����޸������ռ����ٶ�
				//sun->t += sun->speed;
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
					balls[i].timer = 0;	//���ö�ʱ����������һ��ȡ��ͬ����������һ����ֹͣλ�þ���ʧ
					balls[i].used = false;
					//printf((balls[i].timer));
					//cout << "i=" << i << ":" << balls[i].timer << endl;
				}
			}*/
		}
		/*else if (balls[i].xoff) {	//�ƶ�������
			//�����������ƫ��ֵ
			float	destY = 0;
			float	destX = 262;

			float angle = atan((balls[i].y - destY) / (balls[i].x - destX));	//���Ǻ���

			balls[i].xoff = 4 * cos(angle);
			balls[i].yoff = 4 * sin(angle);

			balls[i].x -= balls[i].xoff;
			balls[i].y -= balls[i].yoff;

			if (balls[i].x < 262 || balls[i].y < 0) {
				//sunShine += 25;	//�����������ֵ���bug
				balls[i].xoff = 0;
				balls[i].yoff = 0;

			}
		}*/
	}
}

//�ռ�����ʵ��
void collectSunshine(ExMessage* msg) {
	int w = imgSunShineBall[0].getwidth();	//����������Ŀ��
	int h = imgSunShineBall[0].getheight();	//����������ĸ߶�
	for (int i = 0; i < ballMax; i++) {
		if (balls[i].used) {	//������ʹ���˲Ž��в���
			int x = balls[i].pCur.x;
			int y = balls[i].pCur.y;
			if (msg->x > x && msg->x<x + w &&		//ֻ�е���������ⷶΧ�ڲŽ��в���
				msg->y>y && msg->y < y + h) {

				//balls[i].used = false;	//��������ʧ

				balls[i].status = SUNSHINE_COLLECT;

				//sunShine += 25;			//����ֵ��25

				//mciSendString("play res/sunshine.mp3", 0, 0, 0);	

				PlaySound("res/sunshine.wav", NULL, SND_FILENAME | SND_ASYNC);	//����������û��Ч��Ҫʹ��WAV��ʽ������

				//�����������ƫ��ֵ
				//float	destY = 0;
				//float	destX = 262;

				//float angle = atan((balls[i].y - destY) / (balls[i].x - destX));	//���Ǻ���

				//balls[i].xoff = 4 * cos(angle);
				//balls[i].yoff = 4 * sin(angle);

				//�Ż�
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

//������ʬʵ��
void createZm() {

	if (zmCount >= ZM_MAX) {
		return;
	}

	static	int	count = 0;
	static	int	zmFre = 300;
	count++;

	if (count >= zmFre) {	//���ƽ�ʬ���ɵ��ٶ�

		zmFre = 100 + rand() % 200;	//�ڶ������ɽ�ʬ��ʱ�����
		count = 0;

		int i;

		//�ӽ�ʬ����ȡ�����õĽ�ʬ
		for (i = 0; i < zmMax && zms[i].used; i++);	//���ʣ��ʾ���һ���¶��巽ʽ����{}һ����������&&!xx
		if (i >= zmMax)return;

		zms[i].used = true;
		//zms[i].frameIndex = 0;
		zms[i].x = WIN_WIDTH;
		zms[i].row = rand() % 3;
		zms[i].y = 172 + (zms[i].row + 1) * 100;
		zms[i].speed = zm_speed;
		zms[i].blood = 100;
		zms[i].dead = false;
		zms[i].eating = false;
		zms[i].catched = -1;//��ʬ��׽����ֲ���ڱ��е�������ֲ��ĺ��������뽩ʬһ��
		//zms[i].boom = false;

		zmCount++;
	}
}

//���½�ʬ����ʵ��
void updateZm() {
	//���½�ʬλ��
	static int count = 0;
	count++;
	if (count > 2) {	//���ƽ�ʬǰ���ٶ�
		count = 0;
		for (int i = 0; i < zmMax; i++) {
			if (zms[i].used) {
				zms[i].x -= zms[i].speed;
				if (zms[i].x < 38) {
					//������Ϸ
					gameStatus = FAIL;
				}
			}
		}
	}

	//���½�ʬ����
	static int count2 = 0;
	count2++;
	if (count2 > 4) {	//���ƽ�ʬ���������ٶȣ��������
		count2 = 0;
		//setjmp(jmpbuffer_three);
		for (int i = 0; i < zmMax; i++) {
			if (zms[i].used) {
				if (zms[i].dead) {
					zms[i].frameIndex++;
					if (zms[i].frameIndex >= 20) {
						zms[i].used = false;
						killZmCount++;
						cout << "ɱ���Ľ�ʬ����" << killZmCount << endl;
						if (killZmCount >= ZM_MAX) {
							gameStatus = WIN;
						}
					}
				}
				else if (zms[i].eating) {
					if (plant_map[zms[i].row][zms[i].catched].type == 0)
					{
						zms[i].eating = false;
						zms[i].speed = zm_speed;
					}
						
					zms[i].frameIndex = (zms[i].frameIndex + 1) % 20;
				}
				else
				{
					zms[i].frameIndex = (zms[i].frameIndex + 1) % 21;	//��ʬ������	-δ���(�������⣬�������10�ͻ����֣�������)
					//cout << "i=" << i << zms[i].frameIndex << endl;	//���ư������ؽ�ʬͼƬʱ���������ش���
				}
			}
		}
	}
}

//�����㶹�ӵ�ʵ��
void createBullets() {
	int lines[3] = { 0 };
	int	dangerX = WIN_WIDTH - imgZm[0].getwidth() + 50;	//���忪ʼ�������
	for (int i = 0; i < zmMax; i++) {
		if (zms[i].used && zms[i].x < dangerX) {
			lines[zms[i].row] = 1;	//��ʬ�����иı��
			//cout <<"��ʬ������" << zms[i].row << endl;
		}
	}
	for (int i = 0; i < 3; i++) {	//3��9�̶�ֵ����չ�Բ���ǽ���궨��
		for (int j = 0; j < 9; j++) {
			if (lines[i] && plant_map[i][j].type == 1) {	//���㶹�ҽ�ʬ�ߵ������Χ
				plant_map[i][j].shootTimer++;
				if (plant_map[i][j].shootTimer > wandou_bullet_product_speed) {
					plant_map[i][j].shootTimer = 0;
					plant_map[i][j].frameIndex = 3;
					int k;
					for (k = 0; k < bulletMax && bullets[k].used; k++);
					if (k >= bulletMax) return;
					bullets[k].used = true;		//��ʼ���㶹�ӵ�
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

//�����㶹�ӵ�����ʵ��
void updateBullets() {
	for (int i = 0; i < bulletMax; i++) {
		if (bullets[i].used) {
			bullets[i].x += bullets[i].speed;
			if (bullets[i].x > WIN_WIDTH) {
				bullets[i].used = false;
			}
			//��ײ���������ʧ
			if (bullets[i].blast) {
				bullets[i].frameIndex++;
				if (bullets[i].frameIndex > 3) {	//����3�ᵼ��״̬����˸
					bullets[i].used = false;
				}
			}
		}
	}
}

//��齩ʬ��ײʵ��
void collisionCheck() {

	//�㶹�ӵ��뽩ʬ����ײ���
	checkBullet2Zm();

	//��ʬ��ֲ�����ײ���
	checkZm2ZhiWu();
}

//�㶹�ӵ��뽩ʬ����ײ���ʵ��
void checkBullet2Zm() {
	for (int i = 0; i < bulletMax; i++) {
		if (bullets[i].used == false || bullets[i].blast)continue;	//����㶹�ӵ�ûʹ�û����Ѿ���ʼ��ײ��������
		for (int j = 0; j < zmMax; j++) {
			if (zms[j].used == false)continue;
			int x1 = zms[j].x + 80;
			int x2 = zms[j].x + 110;
			int x = bullets[i].x;
			if (zms[j].dead == false && bullets[i].row == zms[j].row && x > x1 && x < x2) {	//�㶹�ӵ��뽩ʬ��ײ��
				zms[j].blood -= pea_bullet_damage;//5;//10;//20;
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

//��ʬ��ֲ�����ײ���ʵ��
void checkZm2ZhiWu() {
	for (int i = 0; i < zmMax; i++) {
		if (zms[i].dead)continue;
		int row = zms[i].row;
		for (int k = 0; k < 9; k++) {
			if (plant_map[row][k].type == 0)continue;
			int zhiWuX = 256 - 112 + k * 81;	//���彩ʬ���Է�Χ
			int x1 = zhiWuX + 10;//x1,x2�ֱ�Ϊֲ������ҽ�
			int x2 = zhiWuX + 60;
			int x3 = zms[i].x + 80;//x3Ϊ��ʬ�����

			if (x3 > x1 && x3 < x2) //�����˽�ʬ��ֲ�ﻥ�Եķ�Χ
			{
				zms[i].catched = k;//��¼��ʬ��׽����ֲ������һ�е�����
				if (plant_map[row][k].type != SHI_REN_HUA + 1 && plant_map[row][k].catched == true&& plant_map[row][k].type!=WALL_NUT+1)
				{	//��ʬ�ԵĹ����е�һЩ����
					plant_map[row][k].deadTimer++;
					if (plant_map[row][k].deadTimer > normal_plant_blood)
					{	//��ʬ������-���ò���
						plant_map[row][k].deadTimer = 0;
						plant_map[row][k].type = 0;
						zms[i].eating = false;
						zms[i].frameIndex = 0;
						zms[i].speed = zm_speed;
					}
				}
				else if (plant_map[row][k].type == SHI_REN_HUA + 1)
				{
					if (plant_map[row][k].eating == false)
					{
						if (x3 <= x2)
						{
							plant_map[row][k].catchzm = i;//��¼ÿ��ʳ�˻�Ҫ�ԵĽ�ʬ������
							plant_map[row][k].eating = true;
							/*zms[i].dead = true;
							zms[i].used = false;*/
						}
					}
				}
				else if (plant_map[row][k].type == WALL_NUT + 1)
				{
					plant_map[row][k].catched = true;
					zms[i].eating = true;
					zms[i].speed = 0;
					//zms[i].frameIndex = 0;
					plant_map[row][k].deadTimer++;
					if (plant_map[row][k].deadTimer > wallnut_plant_blood)
					{	//��ʬ������-���ò���
						
						zms[i].eating = false;
						zms[i].frameIndex = 0;
						zms[i].speed = zm_speed;
						//plant_map[row][k].deadTimer = 0;
						plant_map[row][k].type = 0;
					}
				}
				else {	//��ʬ����-���ò���
					plant_map[row][k].catched = true;
					plant_map[row][k].deadTimer = 0;
					zms[i].eating = true;
					zms[i].speed = 0;
					zms[i].frameIndex = 0;
				}
			}
		}
	}
}

//����С�Ƴ�
void creatcar()
{
	//����ֻ�õ�3��С�Ƴ���
	int carmax = sizeof(cars) / sizeof(cars[0]);
	for (int i = 0; i < carmax; i++)
	{
		cars[i].x = 60;
		cars[i].y = 100 + (i + 1) * 100 + 10;
		cars[i].used = true;
		cars[i].move = false;
	}
}

void updatecar()
{

	for (int i = 0; i < carmax; i++)
	{
		if (cars[i].move == true)
		{
			cars[i].x += 10;
			/*��ÿ�����ӵ�xֵ�趨�ù���ʱ���������ƶ���ʱ����ܻ�©�����ֽ�ʬ
			���趨�Ĺ�Сʱ�ֻ�Ӱ��۸У��趨ʱӦ�ۺϿ���������*/
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
			if (zms[j].used && zms[j].dead == false && zms[j].row == i&&cars[i].x <= WIN_WIDTH)
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
					else //С�Ƴ����˶�����������ʬ
					{
						/*if (zms[i].dead == false)
						{
							killZmCount++;
							cout << "ɱ���Ľ�ʬ����" << killZmCount << endl;
						}	*/			
						//��dead��Ϊtrue����updatezm()�����м������Ӽ����Ĳ��裬�����û�б�Ҫ��
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
	//����С�Ƴ�
	//creatcar();
	/*�ư��ˣ�֮ǰsb�߼������ˣ�С�Ƴ�ֻ����Ϸ��ʼʱ����һ�Σ�֮���ٴ�����
	����������updategame()�������棬�᲻�ϴ���С�Ƴ������¶�һ�ξͲ��ٶ����ٶ��ý�ʬ�ٽӴ������޷�ɱ����ʬ*/
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
	int shovel_x2 = (235 + 7 * 65) + 70;
	int shovel_y1 = 0;
	int shovel_y2 = 70;
	/*int x = 343 + i * 65;//ֲ�￨������
				int y = 6;*/
	int x1 = 163 + ZHI_WU_COUNT * 65;//x1��x2Ϊֲ�￨�����ĺ�����
	int x2 = 163;
	int y1 = 6;//y1��y2Ϊֲ�￨������������
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
		shovel_front.x = msg->x - 40;
		shovel_front.y = msg->y - 35;
		if (msg->message == WM_RBUTTONDOWN) //����Ҽ�����
		{
			shovel_front.used = false;
		}
		else if (msg->x > x2&&msg->x<x1&&msg->y>y1&&msg->y < y2&&msg->message == WM_LBUTTONDOWN)
		{
			shovel_front.used = false;
		}
		else
		{
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 8; j++)
				{
					//180,230Ϊֲ���ͼ���Ͻ�
					//plant_map[row][col].x = 256 - 112 + col * 81;	//ֲ������
					//plant_map[row][col].y = 179 + row * 102 + 14;
					int plant_x1 = plant_map[i][j].x;
					int plant_x2 = plant_map[i][j].x + 81;
					int plant_y1 = plant_map[i][j].y;
					int plant_y2 = plant_map[i][j].y + 102;
					if (msg->x > plant_x1&&msg->x<plant_x2&&msg->y>plant_y1&&msg->y < plant_y2&&msg->message == WM_LBUTTONDOWN)
					{
						plant_map[i][j].type = 0;
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

	//��ʼ����ͣ��ͼ�꣬û�ҵ����ʵ�ͼƬ�����ջ�ջ�
	//putimagePNG((235 + 8 * 65), 25, flag == 0 ? imgpause[2] : imgpause[3]);
	int pause_x1 = (235 + 8 * 65);
	int pause_x2 = (235 + 8 * 65) + 247;
	int pause_y1 = 25;
	int pause_y2 = 25 + 46;
	{
		if (msg->message == WM_LBUTTONDOWN &&	//����������		��չ������꾭��ʱҲ���Ը���
			msg->x > pause_x1 && msg->x<pause_x2 && msg->y>pause_y1 && msg->y < pause_y2)
		{
			//flag = 1;
			cout << "��ͣ" << endl;
			bool isRunning = true; // ���һ����������������ѭ����ִ��
			while (isRunning) // ʹ�ò���������Ϊѭ������
			{
				update_pause(pause_type);
				//putimagePNG((235 + 8 * 65), 25, imgpause[3]);//ֱ�Ӹ���ͼƬ�����Ż�  ���˵�����ʱ��ͼƬ

				/*�˴������������msg��꣬Ӧ���Ǹ���ÿ�ε���clickpause����ʱ���ݲŻ���£����˴�ѭ�����Ѿ���������ˣ�
				�����ڴ˴�����һ���µ���������ʵ���˳�whileѭ����ȡ����ͣ*/
				ExMessage msg1;
				if (peekmessage(&msg1))
				{
					pause_click(&msg1, isRunning, pause_type);

				}

			}
		}

		//else if (msg.message == WM_LBUTTONUP && flag == 1) {	//������̧��
		//	EndBatchDraw();
		//	return;
		//}
	}
}

void game_save()
{
	//�Ѽ������ӵ�����ȫ�����룬�ٴ��뼸�������ȫ�ֱ���
	ofstream outfile("preserve.txt");
	if (!fileExist("preserve.txt"))
		return;

	outfile << sunShine << " " << killZmCount << " " << gameStatus << endl;

	//��������
	for (int i = 0; i < ballMax; i++)
	{
		outfile << balls[i].destY << " " << balls[i].frameIndex << " " <<
			balls[i].p1.x << " " << balls[i].p1.y << " " <<
			balls[i].p2.x << " " << balls[i].p2.y << " " <<
			balls[i].p3.x << " " << balls[i].p3.y << " " <<
			balls[i].p4.x << " " << balls[i].p4.y << " " <<
			balls[i].pCur.x << " " << balls[i].pCur.y << " " <<
			balls[i].speed << " " << balls[i].status << " " <<
			balls[i].t << " " << balls[i].timer << " " <<
			balls[i].used << " " << balls[i].x << " " <<
			balls[i].xoff << " " << balls[i].y << " " <<
			balls[i].yoff;
		outfile << endl;
	}

	//ֲ������
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			outfile << plant_map[i][j].catched << " " << plant_map[i][j].deadTimer << " "
				<< plant_map[i][j].frameIndex << " " << plant_map[i][j].shootTimer << " "
				<< plant_map[i][j].timer << " " << plant_map[i][j].type << " "
				<< plant_map[i][j].x << " " << plant_map[i][j].y << " "
				<< plant_map[i][j].eating << " " << plant_map[i][j].digest << " "
				<< plant_map[i][j].digest_timer << " " << plant_map[i][j].catchzm;
			
			outfile << endl;
		}
	}

	//��ʬ����
	for (int i = 0; i < zmMax; i++)
	{
		outfile << zms[i].blood << " " << zms[i].dead << " "
			<< zms[i].eating << " " << zms[i].frameIndex << " "
			<< zms[i].row << " " << zms[i].speed << " "
			<< zms[i].used << " " << zms[i].x << " "
			<< zms[i].y << " "<<zms[i].catched;
		outfile << endl;
	}

	//С�Ƴ�����
	for (int i = 0; i < carmax; i++)
	{
		outfile << cars[i].move << " " << cars[i].used << " "
			<< cars[i].x << " " << cars[i].y << " ";
		outfile << endl;
	}

	//�㶹�ӵ�����
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

	infile >> sunShine >> killZmCount >> gameStatus;

	//��������
	for (int i = 0; i < ballMax; i++)
	{
		infile >> balls[i].destY >> balls[i].frameIndex >>
			balls[i].p1.x >> balls[i].p1.y >>
			balls[i].p2.x >> balls[i].p2.y >>
			balls[i].p3.x >> balls[i].p3.y >>
			balls[i].p4.x >> balls[i].p4.y >>
			balls[i].pCur.x >> balls[i].pCur.y >>
			balls[i].speed >> balls[i].status >>
			balls[i].t >> balls[i].timer >>
			balls[i].used >> balls[i].x >>
			balls[i].xoff >> balls[i].y >>
			balls[i].yoff;
	}

	//ֲ������
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			infile >> plant_map[i][j].catched >> plant_map[i][j].deadTimer
				>> plant_map[i][j].frameIndex >> plant_map[i][j].shootTimer
				>> plant_map[i][j].timer >> plant_map[i][j].type
				>> plant_map[i][j].x >> plant_map[i][j].y
				>> plant_map[i][j].eating >> plant_map[i][j].digest
				>> plant_map[i][j].digest_timer >> plant_map[i][j].catchzm;
			
		}
	}

	//��ʬ����
	for (int i = 0; i < zmMax; i++)
	{
		infile >> zms[i].blood >> zms[i].dead
			>> zms[i].eating >> zms[i].frameIndex
			>> zms[i].row >> zms[i].speed
			>> zms[i].used >> zms[i].x
			>> zms[i].y>>zms[i].catched;

	}

	//С�Ƴ�����
	for (int i = 0; i < carmax; i++)
	{
		infile >> cars[i].move >> cars[i].used
			>> cars[i].x >> cars[i].y;

	}

	//�㶹�ӵ�����
	for (int i = 0; i < bulletMax; i++)
	{
		infile >> bullets[i].blast >> bullets[i].frameIndex
			>> bullets[i].row >> bullets[i].speed
			>> bullets[i].used >> bullets[i].x
			>> bullets[i].y;

	}

	infile.close();
}

void gameinit_pause()
{
	//�ܹ�9�ţ�5��ͼƬ��ÿ����Ϊ��Ӧ�İ�����δ���µ�ͼƬ��һ��ΪĹ��ͼ
	//0.1Ϊ�����˵���ͼƬ��2,3Ϊ���˵���ͼƬ��4,5,Ϊ��������Ϸ��ͼƬ��6,7Ϊ�����¿�ʼ���ؿ���ͼƬ��9ΪĹ��ͼ
	char name[64];
	memset(name, 0, sizeof(name));
	//IMAGE *imgpause[10]
	imgpause[9] = new IMAGE;
	loadimage(imgpause[9], "res/Screen/pause_0.png");
	loadimage(&imgtombstone, "res/Screen/pause_1.png");

	for (int i = 0; i < 2; i++)//���˵�
	{
		sprintf_s(name, sizeof(name), "res/Screen/mainmenu%d.jpg", i + 1);
		if (fileExist(name)) {
			imgpause[i] = new IMAGE;
			loadimage(imgpause[i], name);
		}
		else {
			break;
		}
		//loadimage(imgpause[i], name);
	}
	for (int i = 0; i < 2; i++)//�˵�
	{
		sprintf_s(name, sizeof(name), "res/Screen/pause_menu%d.jpg", i + 1);
		if (fileExist(name)) {
			imgpause[i + 2] = new IMAGE;
			loadimage(imgpause[i + 2], name);
		}
		else {
			break;
		}
		//loadimage(imgpause[i+2], name);
	}
	for (int i = 0; i < 2; i++)//������Ϸ
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
	for (int i = 0; i < 2; i++)//���¿�ʼ�˹ؿ�
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
	//0.1Ϊ�����˵���ͼƬ��2,3Ϊ���˵���ͼƬ��4,5,Ϊ��������Ϸ��ͼƬ��6,7Ϊ�����¿�ʼ���ؿ���ͼƬ��9ΪĹ��ͼ
	putimagePNG((235 + 8 * 65), 25, imgpause[3]);//ֱ�Ӹ���ͼƬ�����Ż�  ���˵�����ʱ��ͼƬ

	//putimagePNG(238, 50, imgpause[9]);//Ĺ���˵� ʹ�������Ⱦ����ʳ�ͻ
	putimagePNG(238, 50, &imgtombstone);//Ĺ���˵�

	putimagePNG(420 - 90, 450 - 125, type[0] ? imgpause[7] : imgpause[6]);

	putimagePNG(420 - 94, 450 - 125 + 50, type[1] ? imgpause[1] : imgpause[0]);

	putimagePNG(360 - 80, 450 - 125 + 100 + 22, type[2] ? imgpause[5] : imgpause[4]);

	EndBatchDraw();
}

void pause_click(ExMessage *msg, bool &isRunning, bool*type)
{
	if (msg->x > 360 - 80 && msg->x < 360 - 80 + 336 && msg->y>450 - 125 + 100 + 22 && msg->y < 450 - 125 + 100 + 22 + 75)//������Ϸ��ͼ��
	{
		type[2] = true;
		if (msg->message == WM_LBUTTONDOWN) {
			game_save();
			//flag = 0;
			isRunning = false; // ��������������Ϊfalse���˳�ѭ��
			cout << "������Ϸ" << endl;
		}
	}
	else type[2] = false;

	if (msg->x > 420 - 94 && msg->x < 420 - 94 + 247 && msg->y>450 - 125 + 50 && msg->y < 450 - 125 + 50 + 46)//�������˵���ͼ��
	{
		type[1] = true;
		if (msg->message == WM_LBUTTONDOWN)
		{
			game_save();
			longjmp(jmpbuffer, 1);
		}
	}
	else type[1] = false;

	if (msg->x > 420 - 90 && msg->x < 420 - 90 + 241 && msg->y>450 - 125 && msg->y < 450 - 125 + 50)//���¿�ʼ�ؿ���ͼ��
	{
		type[0] = true;
		if (msg->message == WM_LBUTTONDOWN)
		{
			read_regame();
			isRunning = false; // ��������������Ϊfalse���˳�ѭ��
			cout << "������Ϸ" << endl;
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
	memset(plant_map, 0, sizeof(plant_map));
	memset(zms, 0, sizeof(zms));
	//memset(cars, 0, sizeof(cars));
	creatcar();
	memset(bullets, 0, sizeof(bullets));
}

void gameinit_shirenhua()
{
	//IMAGE *imgChomperAttack[18];//ʳ�˻��Խ�ʬ18֡ͼƬ
	//IMAGE *imgChomperDigest[18];//ʳ�˻��Խ�ʬ18֡ͼƬ

	char name[64];
	for (int i = 0; i < 19; i++)//ʳ�˻��Խ�ʬ
	{
		sprintf_s(name, sizeof(name), "res/zhiwu/ChomperAttack/ChomperAttack%d.png", i);
		if (fileExist(name)) {
			imgChomperAttack[i] = new IMAGE;
			loadimage(imgChomperAttack[i], name);
		}
		else {
			break;
		}
	}

	for (int i = 0; i < 19; i++)//ʳ�˻�������ʬ
	{
		sprintf_s(name, sizeof(name), "res/zhiwu/ChomperDigest/ChomperDigest%d.png", i);
		if (fileExist(name)) {
			imgChomperDigest[i] = new IMAGE;
			loadimage(imgChomperDigest[i], name);
		}
		else {
			break;
		}
	}
}

void chomper_eating(int x, int y)//��ʱx,y����Ϊʳ�˻���ֲ���ͼ�ϵ�λ��
{
	//static int z;//��¼��ǰʳ�˻�Ҫ�ԵĽ�ʬ�������������⣬��ͬһʱ���ж����������ʱ��z���ܻᱻ�������
	//for (int i = 0; i < zmMax; i++)
	//{
	//	if (zms[i].row == x )
	//	{
	//		int zhiWuX = 256 - 112 + y * 81;	//���彩ʬ���Է�Χ
	//		int x2 = zhiWuX + 60;
	//		int x3 = zms[i].x + 80;//x3Ϊ��ʬ�����
	//		if (x3 <= x2)
	//			z = i;
	//	}
	//}
	Sleep(1);
	plant_map[x][y].frameIndex++;
	int	zhiWuType = plant_map[x][y].type - 1;
	int	index = plant_map[x][y].frameIndex;
	if (imgZhiWu[zhiWuType][index] == NULL) {
		plant_map[x][y].frameIndex = 0;
		plant_map[x][y].eating = false;
		plant_map[x][y].digest = true;
		zms[plant_map[x][y].catchzm].dead = true;
		zms[plant_map[x][y].catchzm].used = false;
		killZmCount++;
		cout << "ɱ���Ľ�ʬ��Ϊ" << killZmCount << endl;
		//z = 0;
	}
}

void chomper_digest(int x, int y)//��ʱx,y����Ϊʳ�˻���ֲ���ͼ�ϵ�λ��
{
	Sleep(1);
	plant_map[x][y].frameIndex++;
	int	zhiWuType = plant_map[x][y].type - 1;
	int	index = plant_map[x][y].frameIndex;
	if (imgZhiWu[zhiWuType][index] == NULL) {
		plant_map[x][y].frameIndex = 0;
		//plant_map[x][y].eating = false; 
		//plant_map[x][y].digest = false;
		plant_map[x][y].digest_timer++;
	}
	if (plant_map[x][y].digest_timer > 5)
	{
		plant_map[x][y].digest_timer = 0;
		plant_map[x][y].digest = false;
	}
}

//ӣ�ұ�ը
void cherry_boom(int x,int y)
{
	static int count = 0;
	count++;
	if (count > 1)
	{
		count = 0;
		plant_map[x][y].frameIndex++;
		int	zhiWuType = plant_map[x][y].type - 1;
		int	index = plant_map[x][y].frameIndex;


		if (imgZhiWu[zhiWuType][index] == NULL) {
			//for (int i = 0; i < zmMax; i++)
			//{
			//		if (zms[i].dead)continue;
			//		int row = zms[i].row;
			//		int zhiWuX = 256 - 112 + y * 81;	//���彩ʬ���Է�Χ
			//		int x1 = zhiWuX + 10;//x1,x2�ֱ�Ϊֲ������ҽ�
			//		int x2 = zhiWuX + 60;
			//		int x3 = zms[i].x + 80;//x3Ϊ��ʬ�����
			//		if (x3 >= x1 && x3 <= x2)
			//		{
			//			//zms[i].blood -= zms[i].blood;
			//			zms[i].used = true;
			//			zms[i].dead = true;//��ʬ�������Ż�
			//			zms[i].speed = 0;
			//			zms[i].frameIndex = 0;
			//			//longjmp(jmpbuffer_three, 1);
			//			plant_map[x][y].type = 0;
			//			/*killZmCount++;
			//			cout << "ɱ���Ľ�ʬ��Ϊ��" << killZmCount << endl;*/
			//		}
			//}
			for (int i = 0; i < zmMax; i++)
			{
				if (zms[i].dead)continue;
				if (zms[i].used)
				{
					int row = zms[i].row;
					int zhiWuX = 256 - 112 + y * 81;	//���彩ʬ���Է�Χ
					int x1 = zhiWuX + 10;//x1,x2�ֱ�Ϊֲ������ҽ�
					int x2 = zhiWuX + 60;
					int x3 = zms[i].x + 80;//x3Ϊ��ʬ�����
					if (x3 >= x1 && x3 <= x2)
					{
						zms[i].dead = true;
						zms[i].frameIndex = 0;
						//��ʬ������bug�����Ż�
						IMAGE* img = NULL;
						img = imgZmDead;
						 //int count1 = 0;
						static int count2 = 0;
						while (zms[i].frameIndex <= 20)
						{
							
							count2++;
							if (count2 > 10) {	//��������
								count2 = 0;
								img += zms[i].frameIndex;
								putimagePNG(zms[i].x, zms[i].y - img->getheight(), img);
								zms[i].frameIndex++;
							}
						}

						zms[i].used = false;
						killZmCount++;
						cout << "ɱ���Ľ�ʬ��Ϊ:" << killZmCount << endl;
					}
				}
			}
			plant_map[x][y].type = 0;
		}
	}
}

void init_loadimage()
{
	for (int i = 0; i < 4; i++)
	{
		imgloadacount[i] = new IMAGE;
	}
	//imgloadacount[0]Ϊ�˺��б�1Ϊ��ǰ�˺ţ�2.3Ϊ�˺Ŵ�����
	//IMAGE *imgloadacount[4];//ϵͳ�ĵ�¼(ûʱ��д�ˣ��׸�Ƥ)
	loadimage(imgloadacount[0], "res/account/selectID.png");
	loadimage(imgloadacount[1], "res/account/SelectorScreen_WoodSign1.png");
	loadimage(imgloadacount[2], "res/account/SelectorScreen_WoodSign2.png");
	loadimage(imgloadacount[3], "res/account/SelectorScreen_WoodSign2_press.png");
}

//�˻��Ĺ���
void account_manage()
{
	ExMessage	msg;
	bool running = true;

	while(running)
	{
			
		BeginBatchDraw();//ѭ������ȾͼƬ��Ҫˢ�»��������ʴ˴���Ҫʹ�øú���
		//putimagePNG(60, 70, imgloadacount[0]);
		//��Ⱦ�����⣬���ںڱ�
			putimagePNG(60, 70, imgloadacount[0]);
		
		if (peekmessage(&msg))
		{
			if (msg.x > 367-80 && msg.x < 560-100 && msg.y>490-20 && msg.y < 535-20 && msg.message == WM_LBUTTONDOWN)
			{
				
				running = false;
			} 
			else if (msg.x > 367 - 80 - 210 && msg.x < 560 - 100 - 210
				&& msg.y>490 - 20 && msg.y < 535 - 20 && msg.message == WM_LBUTTONDOWN)
			{
				running = false;
			}
		}
		EndBatchDraw();
	}



}