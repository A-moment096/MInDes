/*
This file is a part of the microstructure intelligent design software project.

Created:     Qi Huang 2023.04

Modified:    Qi Huang 2023.04;

Copyright (c) 2019-2023 Science center for phase diagram, phase transition, material intelligent design and manufacture, Central South University, China

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free
	Software Foundation, either version 3 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
	FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#pragma once
#include "../../solvers/base/sysTool.h"
using namespace std;
namespace pf {
	class BMP24reader
	{
	public:
		BMP24reader() {};
		~BMP24reader() {
			delete bmpbuf;
			bmpbuf = nullptr;
		};
		vector<int> getRGB(int x, int y) {
			vector<int> rgb;
			rgb.push_back(bmpbuf[y * linebyte + x * 3 + 2]);
			rgb.push_back(bmpbuf[y * linebyte + x * 3 + 1]);
			rgb.push_back(bmpbuf[y * linebyte + x * 3 + 0]);
			return rgb;
		}
		double getGrayPercentage(int x, int y) {
			vector<int> rgb;
			rgb.push_back(bmpbuf[y * linebyte + x * 3 + 2]);
			rgb.push_back(bmpbuf[y * linebyte + x * 3 + 1]);
			rgb.push_back(bmpbuf[y * linebyte + x * 3 + 0]);
			return (rgb[0] * 0.299 + rgb[1] * 0.587 + rgb[2] * 0.114) / 255;
		}
		void changeRGB(int x, int y, int R, int G, int B) {
			bmpbuf[y * linebyte + x * 3 + 2] = R;
			bmpbuf[y * linebyte + x * 3 + 1] = G;
			bmpbuf[y * linebyte + x * 3 + 0] = B;
		}
		void GrayTransfer() {
			for(int x = 0; x < bmp_width; x++)
				for (int y = 0; y < bmp_height; y++) {
					vector<int> rgb = getRGB(x, y);
					int gray = double_to_int(rgb[0] * 0.299 + rgb[1] * 0.587 + rgb[2] * 0.114);
					changeRGB(x, y, gray, gray, gray);
				}
		}
		//���򿪵��ļ�·���Ƿ���ȷ���Ƿ���bmp�ļ�
		int safe(string fileName) 
		{
			const char* bmpname = fileName.c_str();
			BITMAPINFOHEADER head;  //BITMAPINFOHEADERΪ����ϵͳ�Դ��Ľṹ�壬��ֱ��ʹ��
			FILE* file = fopen(bmpname, "rb");//���ļ��������Զ����Ʒ�ʽ��ȡ
			if (!file)               //�ļ��Ƿ��ܴ�
			{
				printf("file dont exist, please check !.\n");
				return 0;//0Ĭ��Ϊ�򿪵��ļ�����ȷ
			}
			if (!(fgetc(file) == 'B' && fgetc(file) == 'M'))//�򿪵��Ƿ���bmpͼƬ
			{
				printf("file isnt in bmp format, please check.\n");
				return 0;
			}//����Ķ�Ĭ��Ϊ��ȷ��bmp�ļ�
			fseek(file, sizeof(BITMAPFILEHEADER), 0);//����λͼ�ļ�ͷ
			fread(&head, sizeof(BITMAPINFOHEADER), 1, file);//��λͼ��Ϣͷ����head�ṹ���С�
			if (head.biBitCount != 24)
			{
				printf("The program now only supports true color BMP files\n");
				return 0;
			}
			fclose(file);
			return 1;//���е���һ�����ļ�Ĭ��Ϊ24λbmp�ļ�
		}
		//�����ݶ���
		int read(string fileName)
		{
			const char* bmpname = fileName.c_str();
			BITMAPINFOHEADER head;//headΪλͼ��Ϣͷ�ṹ��
			FILE* file = fopen(bmpname, "rb");//�Զ����Ʒ�ʽ��
			fseek(file, sizeof(BITMAPFILEHEADER), 0);//����λͼ�ļ�ͷ
			fread(&head, sizeof(BITMAPINFOHEADER), 1, file);//��λͼ��Ϣͷ����head
			bmp_width = head.biWidth;//ͼƬ��
			bmp_height = head.biHeight;//ͼƬ��
			bibitcount = head.biBitCount;//ͼƬ��ɫ��
			linebyte = (bmp_width * bibitcount + 31) / 32 * 4;//ÿ��������ռ���ֽ�����
		//Windows�涨һ��ɨ������ռ���ֽ���������4�ı���������unsigned intΪ��λ�����������0��䣬0����ÿһ�е����
		//���λͼ������ռ�õ��ֽ�����������ͼ��ĸ߶ȳ���ͼ�����ٳ���ÿ������ռ�ֽ��������ˣ���ȷ���㷨Ӧ����ǰ��ʾ
			bmpbuf = (unsigned char*)malloc(linebyte * bmp_height);//���ٿռ����Ա���
			fread(bmpbuf, 1, linebyte * bmp_height, file);//��ȡλͼ������
			fclose(file);
			return 1;
		}
		//���޸ĺ��ͼƬд����һ���ط�
		int save(string newfileName)
		{
			const char* newbmpname = newfileName.c_str();
			int colortablesize = 0;//��ɫ���С����ɫͼ��Ϊ0
			BITMAPFILEHEADER filehead;//λͼ�ļ�ͷ
			BITMAPINFOHEADER datehead;//λͼ��Ϣͷ
			FILE* file = fopen(newbmpname, "wb");//���ļ����Զ����Ʒ�ʽд��
			if (!file) return 0;//�򿪲��ɹ�
			filehead.bfType = 0x4D42;//�ļ�Ϊbmp��ʽ
			filehead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colortablesize + linebyte * bmp_height;//λͼ�ļ���С
			filehead.bfReserved1 = 0;//λͼ�ļ������֣�����Ϊ0
			filehead.bfReserved2 = 0;//λͼ�ļ������֣�����Ϊ0
			filehead.bfOffBits = 54 + colortablesize;//λͼ���ݵ���ʼλ��
			fwrite(&filehead, sizeof(BITMAPFILEHEADER), 1, file);//д��λͼ�ļ�ͷ
			datehead.biBitCount = bibitcount;//��������λ��
			datehead.biClrImportant = 0;//λͼ��ʾ��������Ҫ����ɫ��0��ʾȫ������Ҫ
			datehead.biClrUsed = 0;//��ɫ������ɫ��
			datehead.biCompression = 0;//ѹ�����ͣ�������0
			datehead.biHeight = bmp_height;//λͼ�߶�
			datehead.biPlanes = 1;//Ŀ���豸����
			datehead.biSize = 40;//�ýṹ���ֽ���
			datehead.biSizeImage = linebyte * bmp_height;//λͼ��С
			datehead.biWidth = bmp_width;//λͼ���
			datehead.biXPelsPerMeter = 0;//λͼˮƽ�ֱ���
			datehead.biYPelsPerMeter = 0;//λͼ��ֱ�ֱ���
			fwrite(&datehead, sizeof(BITMAPINFOHEADER), 1, file);//д��λͼ��Ϣͷ
			fwrite(bmpbuf, linebyte * bmp_height, 1, file);//д��λͼ����
			fclose(file);//д�����
			return 1;
		}
		int bmp_width;               //��
		int bmp_height;              //��
	private:
		/*���б�������ʹ�ã���дΪȫ�ֱ���*/
		int linebyte;               //����ÿ����ռ���ֽ���
		int bibitcount;             //��ɫ��
		unsigned char* bmpbuf;      //λͼͼ�������
	};
}