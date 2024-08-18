#pragma once
#include<iostream>
#include<easyx.h>
#include<vector>

class Atlas;//ͼ����

//����������
class Object
{
public:
	//����(���Ͻ�
	int x;
	int y;

	//��С
	int m_width;
	int m_height;

	
	//���������߼�

	Object(int x, int y, int width, int height) :x(x), y(y), m_width(width), m_height(height)
	{
	}

	virtual void load_object(const ExMessage& msg) = 0;

	virtual void draw_object(int delta) = 0;

};