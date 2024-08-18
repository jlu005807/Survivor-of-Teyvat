#pragma once
#include<iostream>
#include<easyx.h>
#include<vector>

class Object;//�����ࣨ����ӵ�����ť�Ƚ���Ԫ��

class Widget
{
public:

	//����״̬
	int width_widget;
	int height_widget;

	ExMessage m_msg;

	//ҳ��ͼƬ(������
	IMAGE* page;

	//����Ԫ��
	std::vector<Object*> m_objects;

	//��ʼ������
	void init_widget(int Width,int height,std::vector<Object*> objects,IMAGE* page);

	void close_widget()
	{
		delete page;
		for (int i = 0; i < m_objects.size(); i++)
			delete m_objects[i];
	}

public:

	Widget(int width,int height, std::vector<Object*> objects,IMAGE* page):width_widget(width),height_widget(height)
	{
		init_widget(width,height,objects,page);
	}


	~Widget()
	{
		close_widget();
	}


	//���������߼�
	void run_widget();//����

	void draw_widget();//ͼ��

	//չʾ����
	void show_widget()
	{		
		    DWORD start_time = GetTickCount();

			run_widget();
			/*system("Pause");*/
			draw_widget();

			DWORD end_time = GetTickCount();
			DWORD gap_time = end_time - start_time;

			if (gap_time < 1000 /144)
				Sleep(1000 / 144 - gap_time);
	}

};
