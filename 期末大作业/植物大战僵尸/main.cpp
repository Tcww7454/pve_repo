#include"game.h"

int main() {

	gameInit();	//��Ϸ��ʼ��

	setjmp(jmpbuffer);

	startUI();	//������Ϸ��ʼ����

	setjmp(jmpbuffer_two);
	//viewScence();	//����Ѳ��

	//barsDown();	//״̬���»�


	//˫���壬�Ƚ�Ҫ���Ƶ�����һ���Ի�����ͼƬ�ϣ��ٰ�ͼƬ��������ⲻ�ϴ��ڴ��ȡ���ݶ����µ���Ļ��˸
	//��Ҫ��BeginBatchDraw()��EndBatchDraw()��FlushBatchDraw()���
	//BeginBatchDraw();		˫����ӵ�����Ῠ״̬��bug-���ڵļ�����ʱ�������

	int timer = 0;
	bool flag = true;

	while (1)
	{
		userClick();	//��ȡ�û�����¼�

		timer += getDelay();	//��ȡ���ʱ��
		if (timer > 20) {	//��������ֲ����Ⱦʱ��
			timer = 0;
			flag = true;
		}
		if (flag) {

			flag = false;

			updateWindow();	//������Ϸ���ڣ���Ⱦ��

			updateGame();	//���¶���

			if (checkOver())break;	//�����Ϸ�Ƿ����
		}
		//ˢ��ͼƬ���������
		//FlushBatchDraw();
	}

	//EndBatchDraw();

	system("pause");

	return 0;
}