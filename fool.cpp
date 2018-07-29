#include<opencv2/opencv.hpp>
using namespace cv;
using namespace std;
Mat foreground, img_Minus, dst_Open, dst_Close; //����������뱳����������������ͼ
Mat imgROI;	//֮���ס������Ҷȣ���ɫ�����������ǲ���
int ThredholdValue = 50;	//��ǰ��ֵ����ֵ
int gray_Value();	//����ѡ����Ҷ�ֵ����ɫ���ص�������
int count_Num();	//��������
void on_Threshold(int, void*); //����ֵ

							   //������
int main()
{
	VideoCapture capture(0);	//����ͷ�汾
	//VideoCapture capture("F:\\Desktop\\my_1\\6B\\��Ƶ\\����ͨ��6B.avi");	//��Ƶ�汾
	Mat frame, background; //������ǰ֡������֡
	int nfrmNum = 0;  //��ʼ֡����һ֡��

	while (true)
	{
		capture >> frame;
		if (nfrmNum == 0)
		{
			if (frame.empty())	//��δ��ȡ�����ݣ��������
			{
				printf("��ȡ����"); break;
			}
			background = frame.clone(); //����֡����һ֡��
										//imshow("background", background);
			nfrmNum++;
		}
		//imshow("��Ƶ", frame);
		nfrmNum++;
		absdiff(frame, background, img_Minus); //maybe������֣�������֡���һ֡����
											   //imshow("�����", img_Minus);

		GaussianBlur(img_Minus, img_Minus, Size(3, 3), 0, 0); //��˹�˲�
		Mat element = getStructuringElement(MORPH_RECT, Size(7, 7));
		morphologyEx(img_Minus, dst_Open, MORPH_OPEN, element);//������
		morphologyEx(dst_Open, dst_Close, MORPH_CLOSE, element);//������������
		cvtColor(dst_Close, dst_Close, COLOR_BGR2GRAY);	//��Ϊ�Ҷ�ͼ��
		namedWindow("������������֮��Ķ�ֵ��ͼ��", WINDOW_AUTOSIZE); //��������
		createTrackbar("��ֵ", "������������֮��Ķ�ֵ��ͼ��", &ThredholdValue, 255, on_Threshold);//��������
		on_Threshold(0, 0);
		imgROI = foreground(Rect(foreground.cols*0.7, 0.0, 50.0, foreground.rows*1.0));//ѡȡ��Ҫ��Ҷȵ�����
		gray_Value();	//������ҶȺ���		
		count_Num();	//��������
		waitKey(30);
	}
	return 0;
}

//�����任��ֵ
void on_Threshold(int, void*)
{
	threshold(dst_Close, foreground, ThredholdValue, 255, 0);
	imshow("������������֮��Ķ�ֵ��ͼ��", foreground);
}
//������ҶȺ���
int gray_Value()
{
	int i, j;
	int w = imgROI.cols; //��
	int h = imgROI.rows; //��
						 //printf("%d %d", w, h);
	uchar *p;
	int intensity, s = 0;
	int num = 0;
	Mat block;
	imgROI.copyTo(block); //�������������������ROI���Ƹ�block
	imshow("��ȡ����ͼ", block);
	for (i = 0; i < h; i++)
	{
		p = block.ptr<uchar>(i);
		for (j = 0; j < w; j++) //����ÿһ������ֵ���ڵ�Ϊ0���׵�Ϊ255���������
		{
			intensity = p[j];
			if (intensity > 0)
				s += 255;
			else
				s += 0;
		}
	}
	s /= 255;  //��ƽ����Ҳ���ǰ�ɫ����ֵ�ĸ���
	return s;
}
//��������һ����־����Ϊ011��ʱ�������һ
//���Լ���һ����ɫ���ظ�������ֵ��������ֵ������������һ֡Ϊ1��С����ֵΪ0��
//011��ʾ���޵��У������ֵ������Ҫ�Ժü��Σ�
int count_Num()
{
	int value = 200; //���ֵ���Լ���ģ�����İ�ɫ���ظ���
	int gray_Num = gray_Value();
	//printf("����Ҷ�ֵ����ɫ����������%d\n", gray_Num);
	int white_Num;
	static int count = 0;
	static int flags[] = { 0,0,0 };	//����һ������{0,0,0}
	if (gray_Num > value) //������ֵ����������һ֡Ϊ1
		white_Num = 1;
	else				  //С����ֵ����������һ֡Ϊ0
		white_Num = 0;

	for (int i = 0; i < 2; i++)	 //������ѭ������һλ��ĩλΪ���µĵ�ǰ֡�ı�־��0��1��
		flags[i] = flags[i + 1];
	flags[2] = white_Num;

	//printf("flag %d%d%d\n", flags[0], flags[1],flags[2]);

	if (flags[0] == 0 && flags[1] == 1 && flags[2] == 1) //������һ��־Ϊ011
	{
		count += 1;
		printf("��ǰͨ����������Ϊ��%d\n", count);
	}
	//printf("��ǰͨ����������Ϊ��%d\n", count);
	return count;
}