#include<opencv2/opencv.hpp>
using namespace cv;
using namespace std;
Mat foreground, img_Minus, dst_Open, dst_Close; //声明输出、与背景相减、开闭运算的图
Mat imgROI;	//之后框住求区域灰度（白色像素数）的那部分
int ThredholdValue = 50;	//当前阈值数阈值
int gray_Value();	//求所选区域灰度值（白色像素的数量）
int count_Num();	//计数函数
void on_Threshold(int, void*); //求阈值

							   //主函数
int main()
{
	VideoCapture capture(0);	//摄像头版本
	//VideoCapture capture("F:\\Desktop\\my_1\\6B\\视频\\看见通信6B.avi");	//视频版本
	Mat frame, background; //声明当前帧、背景帧
	int nfrmNum = 0;  //初始帧（第一帧）

	while (true)
	{
		capture >> frame;
		if (nfrmNum == 0)
		{
			if (frame.empty())	//若未读取到数据，输出错误
			{
				printf("读取错误"); break;
			}
			background = frame.clone(); //背景帧（第一帧）
										//imshow("background", background);
			nfrmNum++;
		}
		//imshow("视频", frame);
		nfrmNum++;
		absdiff(frame, background, img_Minus); //maybe背景差分？拿其他帧与第一帧做差
											   //imshow("相减后", img_Minus);

		GaussianBlur(img_Minus, img_Minus, Size(3, 3), 0, 0); //高斯滤波
		Mat element = getStructuringElement(MORPH_RECT, Size(7, 7));
		morphologyEx(img_Minus, dst_Open, MORPH_OPEN, element);//开运算
		morphologyEx(dst_Open, dst_Close, MORPH_CLOSE, element);//开运算后闭运算
		cvtColor(dst_Close, dst_Close, COLOR_BGR2GRAY);	//化为灰度图像
		namedWindow("开运算后闭运算之后的二值化图像", WINDOW_AUTOSIZE); //创建窗口
		createTrackbar("阈值", "开运算后闭运算之后的二值化图像", &ThredholdValue, 255, on_Threshold);//创建滑条
		on_Threshold(0, 0);
		imgROI = foreground(Rect(foreground.cols*0.7, 0.0, 50.0, foreground.rows*1.0));//选取将要求灰度的区域
		gray_Value();	//求区域灰度函数		
		count_Num();	//技术函数
		waitKey(30);
	}
	return 0;
}

//滑条变换阈值
void on_Threshold(int, void*)
{
	threshold(dst_Close, foreground, ThredholdValue, 255, 0);
	imshow("开运算后闭运算之后的二值化图像", foreground);
}
//求区域灰度函数
int gray_Value()
{
	int i, j;
	int w = imgROI.cols; //宽
	int h = imgROI.rows; //高
						 //printf("%d %d", w, h);
	uchar *p;
	int intensity, s = 0;
	int num = 0;
	Mat block;
	imgROI.copyTo(block); //把主函数里面求的区域ROI复制给block
	imshow("所取区域图", block);
	for (i = 0; i < h; i++)
	{
		p = block.ptr<uchar>(i);
		for (j = 0; j < w; j++) //遍历每一个像素值（黑的为0，白的为255），并相加
		{
			intensity = p[j];
			if (intensity > 0)
				s += 255;
			else
				s += 0;
		}
	}
	s /= 255;  //求平均，也就是白色像素值的个数
	return s;
}
//计数，设一个标志，当为011的时候计数加一
//（自己设一个白色像素个数的阈值，大于阈值，这个区域的这一帧为1，小于阈值为0）
//011表示从无到有（这个阈值可能需要试好几次）
int count_Num()
{
	int value = 200; //这个值是自己设的，区域的白色像素个数
	int gray_Num = gray_Value();
	//printf("区域灰度值（白色像素数）：%d\n", gray_Num);
	int white_Num;
	static int count = 0;
	static int flags[] = { 0,0,0 };	//定义一个数组{0,0,0}
	if (gray_Num > value) //大于阈值这个区域的这一帧为1
		white_Num = 1;
	else				  //小于阈值这个区域的这一帧为0
		white_Num = 0;

	for (int i = 0; i < 2; i++)	 //将数组循环左移一位，末位为更新的当前帧的标志（0或1）
		flags[i] = flags[i + 1];
	flags[2] = white_Num;

	//printf("flag %d%d%d\n", flags[0], flags[1],flags[2]);

	if (flags[0] == 0 && flags[1] == 1 && flags[2] == 1) //计数加一标志为011
	{
		count += 1;
		printf("当前通过物体数量为：%d\n", count);
	}
	//printf("当前通过物体数量为：%d\n", count);
	return count;
}