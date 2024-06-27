#include"game.h"

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

	int timer = 0;
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