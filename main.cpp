#include <graphics.h>		// ���� EasyX ��ͼ��ͷ�ļ�

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	initgraph(640, 480);	// ������ͼ���ڣ��ֱ��� 640x480
	circle(320, 240, 100);	// ��Բ��Բ�� (320, 240)���뾶 100
	Sleep(5000);			// ��ʱ 5000 ����
	closegraph();			// �ر�ͼ�δ���
	return 0;
}


