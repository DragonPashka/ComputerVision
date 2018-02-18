#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>
#include <iterator>
#include <numeric>   
#include "opencv2/core/core.hpp"
#include <iostream>
#include <string.h>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include <iostream>

#include <vector>
#include <fstream>

#include <cstdlib>
#include <windows.h>
using std::vector;
using std::exit;
using std::cout;
using std::cerr;
using std::wcout;
using std::endl;
using std::string;
using std::wstring;
using std::ofstream;
using std::wofstream;
using std::ifstream;
using std::wifstream;

wstring string_to_wstring(string& s);

string wstring_to_string(wstring s);

vector<string> scandir(string p);



using namespace cv;
using namespace std;
struct doublePoint
{
	 double x = 0;
	 double y = 0;
};

struct Cross
{
	Point p1;
	Point p2;
	Point p3;
	Point p4;
};

struct param
{
	CvPoint* h1;
	CvPoint* h2;	
	CvPoint* g1;
	CvPoint* g2;	
	bool f = false;
};
string wndname = "Shape Detection";
static double angle(Point pt1, Point pt2, Point pt0);
static double angle1(doublePoint pt1, doublePoint pt2);
Cross crossingRomb(param R);

doublePoint paramLine(CvPoint* line);
bool kindOfTrilange(const vector<vector<Point> >& squares);
bool kindOfRectangle(const vector<vector<Point> >& squares);
Cross crossingRect(param R);

doublePoint Colision(doublePoint p1, doublePoint p2);
static void findShapes(const Mat& image, vector<vector<Point> >& rects, vector<vector<Point> >& trilanges, vector<vector<Point> >& circles);
static void drawSquares(Mat& image, const vector<vector<Point> >& squares);
Cross crossingTrilagles(param R, param T);
bool compareSquare(Cross R, vector<vector<Point> >& circles, int radius);
bool compareSquareTrilange(vector<vector<Point> >& rects, vector<vector<Point> >& t);
void compareRombFromTrilange(vector<vector<Point> >& rects, vector<vector<Point> >& t);
double matchShapes(vector<vector<Point> >& shapeTest, vector<vector<Point> >& shapeResult);



int main(int argc, char* argv[])
{
	IplImage* image = 0;
	IplImage* imageTest = 0;

	IplImage* dst = 0, *dstTest;
	IplImage* dst1 = 0;
	IplImage* dst2 = 0, *dstTest2;
	IplImage* color_dst = 0;
	IplImage* edges = 0;
	IplImage* src, *srcTest;
	IplImage* src1;
	IplImage* gray = 0;
	vector<string> v, v1;
	int o = 0; 
	v = scandir("D:\\imgs\\*");
	v1 = scandir("D:\\imgsTest\\*");


for (int t = o; t < o + 100; t++)
{
	v[t] = "D:\\imgs\\" + v[t];
	v1[t] = "D:\\imgsTest\\" + v1[t];

	char* filename = &v[t][0u];
	char* filename1 = &v1[t][0u];
	vector<vector<Point>> rects, circles, trilangles, shapes;
	vector<vector<Point>> rectsTest, circlesTest, trilanglesTest;

	CvMemStorage* storage = cvCreateMemStorage(0);
	CvMemStorage* storageCircle = cvCreateMemStorage(0);
	CvSeq* lines = 0;
	src = cvLoadImage(filename, 0);
	src1 = cvLoadImage(filename, 1);
	srcTest = cvLoadImage(filename1, 1);
	param R, Romb, T;
	R.g1 = NULL; R.h2 = NULL; R.g2 = NULL;
	R.h1 = NULL;
	T.g1 = NULL; T.h2 = NULL; T.g2 = NULL;
	T.h1 = NULL;

	Romb.g1 = NULL; Romb.h2 = NULL; Romb.g2 = NULL;
	Romb.h1 = NULL;

	Mat image1 = cvarrToMat(src1);
	image = cvLoadImage(filename, CV_LOAD_IMAGE_COLOR);
	Mat image2 = cvarrToMat(image);
	Mat imageTest;
	imageTest.convertTo(imageTest, CV_8U);
	// получаем картинку
	dst = cvCreateImage(cvGetSize(image), IPL_DEPTH_16S, image->nChannels);
	dst2 = cvCreateImage(cvGetSize(image), image->depth, image->nChannels);
	dst1 = cvCreateImage(cvGetSize(image), IPL_DEPTH_16S, image->nChannels);
	dstTest = cvCreateImage(cvGetSize(srcTest), IPL_DEPTH_16S, srcTest->nChannels);
	dstTest2 = cvCreateImage(cvGetSize(srcTest), srcTest->depth, srcTest->nChannels);

	// окно для отображения картинки
	IplImage* src1 = cvLoadImage(filename);

	int aperture = 5;
	//применяем оператор Лаплас
	cvLaplace(image, dst, aperture);
	cvLaplace(srcTest, dstTest, aperture);

	//преобразуем изображение к 8-битному
	cvConvertScale(dst, dst2);
	cvConvertScale(dstTest, dstTest2);

	imageTest= cvarrToMat(dstTest2);
	image1 = cvarrToMat(dst2);
	findShapes(image1, rects, trilangles, circles);
	findShapes(imageTest, rectsTest, trilanglesTest, circlesTest);
	if (!rectsTest.empty())
		shapes = rectsTest;
	else if (!trilanglesTest.empty())
		shapes = trilanglesTest;
	else if (!circles.empty())
		shapes = circles;

	if (circles.empty())
	{
		if (trilangles.empty())
		{
			if (!rects.empty())
			{
				if (kindOfRectangle(rects))
				{
					cout << filename << " Simple rectangle:" << ((matchShapes(rects, rectsTest) != 1) && kindOfRectangle(rectsTest)) << ", matches = " << matchShapes(shapes, rects) << endl;

				}
				else
				{
					compareRombFromTrilange(rects, trilangles);
					if (!trilangles.empty())
					{
						if (kindOfTrilange(trilangles))
						{
							cout << filename << " Simple trilange:" << ((matchShapes(trilangles, trilanglesTest) != 1) && kindOfTrilange(trilanglesTest)) << ", matches = " << matchShapes(shapes, trilangles) << endl;
							rects.clear();

						}
						else if (!trilangles.empty())
						{
							cout << filename << " Inverted trilange:" << ((matchShapes(trilangles, trilanglesTest) != 1) && !kindOfTrilange(trilanglesTest)) << ", matches = " << matchShapes(shapes, trilangles) << endl;
							rects.clear();

						}
					}
					else
						cout << filename << " Romb:" << ((matchShapes(rects, rectsTest) != 1) && !kindOfRectangle(rectsTest)) << ", matches = " << matchShapes(shapes, rects) << endl;
				}
			}

		}
		else
		{
			if (!rects.empty() && !kindOfRectangle(rects))
			{
				compareRombFromTrilange(rects, trilangles);

				if (compareSquareTrilange(rects, trilangles))
				{
					cout << filename << " Romb:" << ((matchShapes(rects, rectsTest) != 1) && !kindOfRectangle(rectsTest)) << ", matches = " << matchShapes(shapes, rects) << endl;
					trilangles.clear();

				}
				else if (kindOfTrilange(trilangles))
				{
					cout << filename << " Simple trilange:" << ((matchShapes(trilangles, trilanglesTest) != 1) && kindOfTrilange(trilanglesTest)) << ", matches = " << matchShapes(shapes, trilangles) << endl;
					rects.clear();

				}
				else if (!trilangles.empty())
				{
					cout << filename << " Inverted trilange:" << ((matchShapes(trilangles, trilanglesTest) != 1) && !kindOfTrilange(trilanglesTest)) << ", matches = " << matchShapes(shapes, trilangles) << endl;
					rects.clear();

				}
			}
			else
			{
				if (kindOfTrilange(trilangles))
				{
					cout << filename << " Simple trilange:" << ((matchShapes(trilangles, trilanglesTest) != 1) && kindOfTrilange(trilanglesTest)) << ", matches = " << matchShapes(shapes, trilangles) << endl;
				}
				else if (!trilangles.empty())
					cout << filename << " Inverted trilange:" << ((matchShapes(trilangles, trilanglesTest) != 1) && !kindOfTrilange(trilanglesTest)) << ", matches = " << matchShapes(shapes, trilangles) << endl;

				rects.clear();

			}
		}
	}
	else
	{
		rects.clear();
		trilangles.clear();
	}

	// сглаживаем изображение
	cvSmooth(src, src, CV_GAUSSIAN, 5, 5);
	// поиск кругов
	CvSeq* results = cvHoughCircles(src, storageCircle, CV_HOUGH_GRADIENT, 3, src->width / 2, 100, 100, 26, 44);

	if (rects.empty() && trilangles.empty())
	{
		int i = 0;
		color_dst = cvCreateImage(cvGetSize(image), 8, 3);
		edges = cvCreateImage(cvGetSize(image), 8, 1);
		// детектирование границ
		gray = cvCreateImage(cvGetSize(dst2), IPL_DEPTH_8U, 1);
		cvCvtColor(src1, gray, CV_RGB2GRAY);
		cvCanny(gray, edges, 50, 200, 3);
		// конвертируем в цветное изображение
		cvCvtColor(edges, color_dst, CV_GRAY2BGR);
		// нахождение линий
		lines = cvHoughLines2(edges, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, 50, 50, 10);

		for (i = 0; i < lines->total; i++)
		{
			//	cout << lines->total<<endl;
			CvPoint* line = (CvPoint*)cvGetSeqElem(lines, i);
			doublePoint l1, l2, l3, p1, p2;
			Point H;

			int count = 0;
			l2.x = 0; l2.y = 0;
			p2.x = 1208; p2.y = 0;

			l1 = paramLine(line);
			p1 = Colision(l1, l2);

			if (l1.y == -1000 || angle1(l1, l2) < 0.3)
			{
				if (((line[0].x >= 20 && line[0].x <= 35) || (line[0].x >= 88 && line[0].x <= 95)) && (R.h1 == NULL || R.h2 == NULL))
				{
					if ((line[0].x >= 20 && line[0].x <= 35) && R.h1 == NULL)
					{
						line[0].y = 0;
						line[1].y = 128;
						R.h1 = line;
					}
					else if ((line[0].x >= 88 && line[0].x <= 95) && R.h2 == NULL)
					{
						line[0].y = 0;
						line[1].y = 128;
						R.h2 = line;
					}
				}
			}
			else if ((p1.x == -1000) || angle1(l1, l2) >= 0.97)
			{
				if ((line[0].y >= 20 && line[0].y <= 35) || (line[0].y >= 88 && line[0].y <= 95) && (R.g1 == NULL || R.g2 == NULL))
				{
					if ((line[0].y >= 20 && line[0].y <= 35) && R.g1 == NULL)
					{
						line[0].x = 0;
						line[1].x = 128;
						R.g1 = line;
					}
					else if ((line[0].y >= 88 && line[0].y <= 95) && R.g2 == NULL)
					{
						line[0].x = 0;
						line[1].x = 128;
						R.g2 = line;
					}
				}
			}
			else
				if (angle1(l1, l2) >0.6 && angle1(l1, l2) <0.8)
				{
					double meanX = (double)(line[1].x + line[0].x) / 2;
					double meanY = (double)(line[1].y + line[0].y) / 2;
					line[0].x = 0;
					line[0].y = l1.y;
					line[1].x = 128;
					line[1].y = 128 * l1.x + l1.y;

					if (meanX>64 && meanY<64 && Romb.h1 == NULL)
					{
						Romb.h1 = line;
					}
					else
						if (meanX>64 && meanY>64 && Romb.h2 == NULL)
						{
							Romb.h2 = line;
						}
						else
							if (meanX<64 && meanY>64 && Romb.g1 == NULL)
							{
								Romb.g1 = line;
							}
							else
								if (meanX<64 && meanY<64 && Romb.g2 == NULL)
								{
									Romb.g2 = line;
								}
				}
				else
				{
					if (angle1(l1, l2) >0.4 && angle1(l1, l2) <0.6)
					{

						if ((T.h1 == NULL))
						{
							T.h1 = line;
						}
						else
						{
							l3 = paramLine(T.h1);
							if ((angle1(l1, l3) > 0.4 && angle1(l1, l2) < 0.6) && T.h2 == NULL)
							{
								T.h2 = line;
							}
						}
					}
				}
		}

		if (results->total == 1 || !circles.empty())
		{
			Cross p, p0, p2;
			p = crossingRect(R);
			p0 = crossingRomb(Romb);
			p2 = crossingTrilagles(R, T);
			if (p.p1.x != -1000 && p0.p1.x == -1000)
			{
				float* p1 = (float*)cvGetSeqElem(results, 0);

				if (compareSquare(p, circles, cvRound(p1[1])))
				{
					cout << filename << " The sign \"STOP\" ,matches = " << matchShapes(shapes, circles) << endl;
				}
				else
				{
					if (circles.empty())
					{
						float* p1 = (float*)cvGetSeqElem(results, 0);
						CvPoint pt = cvPoint(cvRound(p1[0]), cvRound(p1[1]));
						if (p2.p1.x == -1000)
						{
							cvCircle(image, pt, cvRound(p1[2]), CV_RGB(0, 0xff, 0));
							cout << filename << " Circle" << endl;
						}
						else
						{
							vector<Point> tr;
							tr.push_back(p2.p3);
							tr.push_back(p2.p1);
							tr.push_back(p2.p2);
							trilangles.push_back(tr);
							if (kindOfTrilange(trilangles))
							{
								cout << filename << " Simple trilange:" << ((matchShapes(trilangles, trilanglesTest) != 1) && kindOfTrilange(trilanglesTest)) << ", matches = " << matchShapes(shapes, trilangles) << endl;
							}else
								cout << filename << " Inverted trilange:" << ((matchShapes(trilangles, trilanglesTest) != 1) && !kindOfTrilange(trilanglesTest)) << ", matches = " << matchShapes(shapes, trilangles) << endl;

							cvLine(image, p2.p1, p2.p2, CV_RGB(255, 0, 0), 3, CV_AA, 0);
							cvLine(image, p2.p1, p2.p3, CV_RGB(255, 0, 0), 3, CV_AA, 0);
							cvLine(image, p2.p2, p2.p3, CV_RGB(255, 0, 0), 3, CV_AA, 0);
							trilangles.clear();
						}

					}
					else
						cout << filename << " Circle:" << (matchShapes(circles, circlesTest) != 1) << ", matches = " << matchShapes(shapes, circles) << endl;
				}
			}
			else
			{
				if (!circles.empty())
				{
					cout << filename << " Circle:" << (matchShapes(circles, circlesTest) != 1) << ", matches = " << matchShapes(shapes, circles) << endl;

				}
				else
					if (p0.p1.x != -1000)
					{
						Cross p;
						p = crossingRomb(Romb);
						if (p.p1.x != -1000 && (p.p2.x != 0 && p.p3.x != 0))
						{
							vector<Point> re;
							re.push_back(p.p1);
							re.push_back(p.p2);
							re.push_back(p.p3);
							re.push_back(p.p4);

							rects.push_back(re);
							cout << filename << " Romb:" << (matchShapes(rects, rectsTest) != 1) << ", matches = " << matchShapes(shapes, rects) << endl;

							rects.clear();
							cvLine(image, p.p1, p.p2, CV_RGB(0, 255, 0), 3, CV_AA, 0);
							cvLine(image, p.p2, p.p3, CV_RGB(0, 255, 0), 3, CV_AA, 0);
							cvLine(image, p.p3, p.p4, CV_RGB(0, 255, 0), 3, CV_AA, 0);
							cvLine(image, p.p4, p.p1, CV_RGB(0, 255, 0), 3, CV_AA, 0);
						}

					}
					else
					{
						cout << filename << " Circle" << endl;
						float* p1 = (float*)cvGetSeqElem(results, 0);
						CvPoint pt = cvPoint(cvRound(p1[0]), cvRound(p1[1]));
						cvCircle(image, pt, cvRound(p1[2]), CV_RGB(0, 0xff, 0));
					}
			}
		}
		else
		{
			Cross p, p0;
			p0 = crossingRomb(Romb);
			p = crossingRect(R);
			if (p.p1.x != -1000 && p0.p1.x == -1000)
			{
				vector<Point> re;
				re.push_back(p.p1);
				re.push_back(p.p2);
				re.push_back(p.p4);
				re.push_back(p.p3);
				rects.push_back(re);
				cout << filename << " Simple rectangle:"<< (matchShapes(rects, rectsTest)!=1)<<", matches = " << matchShapes(shapes, rects) << endl;
				rects.clear();
				cvLine(image, p.p1, p.p2, CV_RGB(255, 0, 0), 3, CV_AA, 0);
				cvLine(image, p.p3, p.p4, CV_RGB(255, 0, 0), 3, CV_AA, 0);
				cvLine(image, p.p1, p.p3, CV_RGB(255, 0, 0), 3, CV_AA, 0);
				cvLine(image, p.p2, p.p4, CV_RGB(255, 0, 0), 3, CV_AA, 0);
			}
			else
			{
				Cross p;
				p = crossingRomb(Romb);
				if (p.p1.x != -1000 && (p.p2.x != 0 && p.p3.x != 0))
				{
					vector<Point> re;
					re.push_back(p.p1);
					re.push_back(p.p2);
					re.push_back(p.p3);
					re.push_back(p.p4);
					rects.push_back(re);

					cout << filename << " Romb:" << (matchShapes(rects, rectsTest) != 1) << ", matches = " << matchShapes(shapes, rects) << endl;
					rects.clear();					
					cvLine(image, p.p1, p.p2, CV_RGB(0, 255, 0), 3, CV_AA, 0);
					cvLine(image, p.p2, p.p3, CV_RGB(0, 255, 0), 3, CV_AA, 0);
					cvLine(image, p.p3, p.p4, CV_RGB(0, 255, 0), 3, CV_AA, 0);
					cvLine(image, p.p4, p.p1, CV_RGB(0, 255, 0), 3, CV_AA, 0);
				}
				else
				{
					if (rects.empty() && trilangles.empty() && circles.empty())
					{
						findShapes(image2, rects, trilangles, circles);
						if (circles.empty())
						{
							if (trilangles.empty())
							{
								if (!rects.empty())
								{
									if (kindOfRectangle(rects))
									{
										cout << filename << " Simple rectangle:" <<( (matchShapes(rects, rectsTest) != 1)&& kindOfRectangle(rectsTest)) << ", matches = " << matchShapes(shapes, rects) << endl;
									}
									else
									{
										compareRombFromTrilange(rects, trilangles);

										if (!trilangles.empty())
										{
											if (kindOfTrilange(trilangles))
											{
												cout << filename << " Simple trilange:" << ((matchShapes(trilangles, trilanglesTest) != 1) && kindOfTrilange(trilanglesTest)) << ", matches = " << matchShapes(shapes, trilangles) << endl;
												rects.clear();

											}
											else if (!trilangles.empty())
											{
												cout << filename << " Inverted trilange:" << ((matchShapes(trilangles, trilanglesTest) != 1) && !kindOfTrilange(trilanglesTest)) << ", matches = " << matchShapes(shapes, trilangles) << endl;
												rects.clear();

											}
										}
										else
											cout << filename << " Romb:" << ((matchShapes(rects, rectsTest) != 1) && !kindOfRectangle(rectsTest)) << ", matches = " << matchShapes(shapes, rects) << endl;


									}
								}
								else cout << " NonType" << endl;

							}
							else
							{
								if (!rects.empty() && !kindOfRectangle(rects))
								{
									compareRombFromTrilange(rects, trilangles);

									if (compareSquareTrilange(rects, trilangles))
									{
										cout << filename << " Romb:" << ((matchShapes(rects, rectsTest) != 1) && !kindOfRectangle(rectsTest)) << ", matches = " << matchShapes(shapes, rects) << endl;
									}
									else if (kindOfTrilange(trilangles))
									{
										cout << filename << " Simple trilange:" << ((matchShapes(trilangles, trilanglesTest) != 1) && kindOfTrilange(trilanglesTest)) << ", matches = " << matchShapes(shapes, trilangles) << endl;
										rects.clear();

									}
									else if (!trilangles.empty())
									{
										cout << filename << " Inverted trilange:" << ((matchShapes(trilangles, trilanglesTest) != 1) && !kindOfTrilange(trilanglesTest)) << ", matches = " << matchShapes(shapes, trilangles) << endl;

										rects.clear();

									}
								}
								else
								{
									if (kindOfTrilange(trilangles))
									{
										cout << filename << " Simple trilange:" << ((matchShapes(trilangles, trilanglesTest) != 1) && kindOfTrilange(trilanglesTest)) << ", matches = " << matchShapes(shapes, trilangles) << endl;
									}
									else if (!trilangles.empty())
										cout << filename << " Inverted trilange:" << ((matchShapes(trilangles, trilanglesTest) != 1) && !kindOfTrilange(trilanglesTest)) << ", matches = " << matchShapes(shapes, trilangles) << endl;
									rects.clear();

								}
							}
						}

						else
						{
							cout << filename << " Circle:" << (matchShapes(circles, circlesTest) != 1)  << ", matches = " << matchShapes(shapes, circles) << endl;

							rects.clear();
							trilangles.clear();
						}
					}

				}
			}

		}

	}


	drawSquares(image2, rects);
	drawSquares(image2, circles);
	drawSquares(image2, trilangles);
	// показываем картинку
	cvShowImage(filename, image);
}

	cvWaitKey(0);


	cvReleaseImage(&gray);
	cvReleaseImage(&src);
	//cvReleaseImage(&src1);

	cvReleaseImage(&image);
	cvReleaseImage(&dst);
	cvReleaseImage(&dst2);
	cvReleaseImage(&dstTest);
	cvReleaseImage(&srcTest);
	cvReleaseImage(&color_dst);
	cvReleaseImage(&edges);
	// удаляем окна
	cvDestroyAllWindows();
	// удаляем окна
	cvDestroyAllWindows();
	return 0;
}


double matchShapes(vector<vector<Point> >& shapeTest, vector<vector<Point> >& shapeResult)
{
	if (shapeResult.empty() || shapeTest.empty())
		return 1;

	double max = -20;
	double m;
	for(int i=0; i<shapeResult.size(); i++)
		for (int j = 0; j < shapeTest.size(); j++)
		{
			m = matchShapes(shapeResult[i], shapeTest[j], CV_CONTOURS_MATCH_I1, 0);
			if (max < m)
				max = m;
		}
	return max;
}


Cross crossingTrilagles(param R, param T)
{
	Cross P;
	int b = 0;
	int side = 0;
	doublePoint l1, l2, l3, l4, p1, p2;
	if (T.h1 != NULL)
		b++;
	if (T.h2 != NULL)
		b++;
	if (R.g1 != NULL)
		b++;
	if (R.g2 != NULL)
		b++;
	if (b == 4)
	{
		l1 = paramLine(T.h1);
		l2 = paramLine(T.h2);
		p1 = Colision(l1, l2);
		if (!(((p1.y >= 20 && p1.y <= 35) || (p1.y >= 88 && p1.y <= 95)) && (p1.x >= 40) && (p1.x <= 80)))
		{
			P.p1.x = -1000;
			return P;
		}
		side = (p1.y >= 20 && p1.y <= 35) ? 2: 1;		
		switch (side)
		{
			case 2:
				P.p1.x = p1.x; P.p1.y = p1.y;
				l3 = paramLine(R.g2);
				p2= Colision(l1, l3);
				P.p2.x = p2.x; P.p2.y = p2.y;
				p1= Colision(l3, l2);
				P.p3.x = p1.x; P.p3.y = p1.y;
				break;
			case 1:
				P.p1.x = p1.x; P.p1.y = p1.y;
				l3 = paramLine(R.g1);
				p2 = Colision(l1, l3);
				P.p2.x = p2.x; P.p2.y = p2.y;
				p1 = Colision(l3, l2);
				P.p3.x = p1.x; P.p3.y = p1.y;
				break;	
		}
	}	
	if (b == 3)
	{
		if (R.g1 != NULL && R.g2 != NULL)
		{
			P.p1.x = -1000;
			return P;
		}
		l1 = paramLine(T.h1);
		l2 = paramLine(T.h2);
		p1 = Colision(l1, l2);

		if (!(((p1.y >= 20 && p1.y <= 35) || (p1.y >= 88 && p1.y <= 95)) && (p1.x >= 40) && (p1.x <= 80)))
		{
			P.p1.x = -1000;
			return P;
		}
		side = (p1.y >= 20 && p1.y <= 35) ? 2 : 1;
		if (R.g1 != NULL && side == 1)
		{
			l3 = paramLine(R.g1);
			P.p1.x = p1.x; P.p1.y = p1.y;
			l3 = paramLine(R.g1);
			p2 = Colision(l1, l3);
			P.p2.x = p2.x; P.p2.y = p2.y;
			p1 = Colision(l3, l2);
			P.p3.x = p1.x; P.p3.y = p1.y;
		}
		else if (R.g2 != NULL && side == 2)
		{
			P.p1.x = p1.x; P.p1.y = p1.y;
			l3 = paramLine(R.g2);
			p2 = Colision(l1, l3);
			P.p2.x = p2.x; P.p2.y = p2.y;
			p1 = Colision(l3, l2);
			P.p3.x = p1.x; P.p3.y = p1.y;
		}
		else b = 2;
	}
	if (b == 2)
	{
		if (R.g1 != NULL && R.g2 != NULL)
		{
			P.p1.x = -1000;
			return P;
		}
		if ((T.h1 != NULL) && (T.h2 != NULL))
		{
			l1 = paramLine(T.h1);
			l2 = paramLine(T.h2);
			p1 = Colision(l1, l2);
			if (!(((p1.y >= 20 && p1.y <= 35) || (p1.y >= 88 && p1.y <= 95)) && (p1.x >= 40) && (p1.x <= 80)))
			{
				P.p1.x = -1000;
				return P;
			}
			if ((p1.y >= 20 && p1.y <= 35))
			{
				P.p1.x = p1.x; P.p1.y = p1.y;
				P.p2.x = p1.x - 20; P.p2.y = l1.x*P.p2.x + l1.y;
				P.p3.x = p1.x + 20; P.p3.y = l2.x*P.p3.x + l2.y;
				if (P.p2.y < p1.y)
				{
					P.p2.y = l2.x*P.p2.x + l2.y;
					P.p3.y = l1.x*P.p3.x + l1.y;
				}	
			}
			else			
			{
				P.p1.x = p1.x; P.p1.y = p1.y;
				P.p2.x = p1.x - 20; P.p2.y = l1.x*P.p2.x + l1.y;
				P.p3.x = p1.x + 20; P.p3.y = l2.x*P.p3.x + l2.y;
				if (P.p2.y > p1.y)
				{
					P.p2.y = l2.x*P.p2.x + l2.y;
					P.p3.y = l1.x*P.p3.x + l1.y;
				}
			}
		}
		b = (R.g2 == NULL) ? 1 : 2;
		side = (T.h2 == NULL) ? 1: 2;
		switch (b)
		{
		case 1:
			l1 = paramLine(R.g1);
			if (side==1)
			l2 = paramLine(T.h1);
			else l2 = paramLine(T.h2);
			p1 = Colision(l1, l2);
			if (!(((p1.x >= 20) && (p1.x <= 35))|| ((p1.x >= 80) && (p1.x <= 95))))
			{
				P.p1.x = -1000;
				return P;
			}
			if ((p1.x >= 20) && (p1.x <= 35))
			{
				P.p1.x = p1.x; P.p1.y = p1.y;
				P.p2.x = p1.x + 20; P.p2.y = l2.x*P.p2.x + l2.y;
				P.p3.x = p1.x + 40; P.p3.y = l1.y;
			}
			else
			{
				P.p1.x = p1.x; P.p1.y = p1.y;
				P.p2.x = p1.x - 20; P.p2.y = l2.x*P.p2.x + l2.y;
				P.p3.x = p1.x - 40; P.p3.y = l1.y;
			}

			break;
		case 2:
			l1 = paramLine(R.g2);
			if (side == 1)
				l2 = paramLine(T.h1);
			else l2 = paramLine(T.h2);
			p1 = Colision(l1, l2);
			if (!(((p1.x >= 20) && (p1.x <= 35)) || ((p1.x >= 80) && (p1.x <= 95))))
			{
				P.p1.x = -1000;
				return P;
			}
			if ((p1.x >= 20) && (p1.x <= 35))
			{
				P.p1.x = p1.x; P.p1.y = p1.y;
				P.p2.x = p1.x + 20; P.p2.y = l2.x*P.p2.x + l2.y;
				P.p3.x = p1.x + 40; P.p3.y = l1.y;
			}
			else
			{
				P.p1.x = p1.x; P.p1.y = p1.y;
				P.p2.x = p1.x - 20; P.p2.y = l2.x*P.p2.x + l2.y;
				P.p3.x = p1.x - 40; P.p3.y = l1.y;
			}

			break;
		}
		
	}else 	P.p1.x = -1000;


		return P;
}

Cross crossingRomb(param R)
{
	Cross P;
	int b = 0;
	int side = 0;
	doublePoint l1, l2, l3, l4, p1, p2;
	if (R.h1 != NULL)
		b++;
	if (R.h2 != NULL)
		b++;
	if (R.g1 != NULL)
		b++;
	if (R.g2 != NULL)
		b++;
	if (b == 3)
	{
		if ((R.h1 == NULL) || (R.h2 == NULL))
			side = (R.h1 == NULL) ? 1 : 2;
		else
			side = (R.g1 == NULL) ? 3 : 4;
		switch (side)
		{
		case 1:
			l1 = paramLine(R.h2);
			l2 = paramLine(R.g1);
			p1 = Colision(l1, l2);
			P.p3.x = p1.x; P.p3.y = p1.y;		
			l3 = paramLine(R.g2);
			p1 = Colision(l3, l2);
			P.p4.x = p1.x; P.p4.y = p1.y;

			P.p1.x = P.p3.x; P.p1.y = l3.x*P.p1.x + l3.y;
			P.p2.x = P.p3.x+(P.p3.x - P.p4.x); P.p2.y = l1.x*P.p2.x+ l1.y;
			break;
		case 2:
			l1 = paramLine(R.g2);
			l2 = paramLine(R.h1);
			p1 = Colision(l1, l2);
			P.p1.x = p1.x; P.p1.y = p1.y;
			l3 = paramLine(R.g1);
			p1 = Colision(l3, l1);
			P.p4.x = p1.x; P.p4.y = p1.y;

			P.p3.x = P.p1.x; P.p1.y = l3.x*P.p3.x + l3.y;
			P.p2.x = P.p3.x + (P.p3.x - P.p4.x); P.p2.y = l2.x*P.p2.x + l2.y;
			break;
		case 3:
			l1 = paramLine(R.h1);
			l2 = paramLine(R.h2);
			p1 = Colision(l1, l2);
			P.p2.x = p1.x; P.p2.y = p1.y;
			l3 = paramLine(R.g2);
			p1 = Colision(l3, l1);
			P.p1.x = p1.x; P.p1.y = p1.y;

			P.p3.x = P.p1.x; P.p3.y = l2.x*P.p3.x + l2.y;
			P.p4.x = P.p3.x - (P.p2.x - P.p3.x); P.p4.y = l3.x*P.p4.x + l3.y;
			break;
		case 4:
			l1 = paramLine(R.h1);
			l2 = paramLine(R.h2);
			p1 = Colision(l1, l2);
			P.p2.x = p1.x; P.p2.y = p1.y;
			l3 = paramLine(R.g1);
			p1 = Colision(l3, l2);
			P.p3.x = p1.x; P.p3.y = p1.y;

			P.p1.x = P.p3.x; P.p1.y = l1.x*P.p1.x + l1.y;
			P.p4.x = P.p3.x - (P.p2.x - P.p3.x); P.p4.y = l3.x*P.p4.x + l3.y;
			break;
		}

	}
	else
		if (b == 4)
		{
			l1 = paramLine(R.g2);
			l2 = paramLine(R.h1);
			p1 = Colision(l1, l2);
			P.p1.x = p1.x; P.p1.y = p1.y;

			l1 = paramLine(R.h2);
			p1 = Colision(l1, l2);
			P.p2.x = p1.x; P.p2.y = p1.y;

			l2 = paramLine(R.g1);
			p1 = Colision(l1, l2);
			P.p3.x = p1.x; P.p3.y = p1.y;

			l1 = paramLine(R.g2);
			p1 = Colision(l1, l2);
			P.p4.x = p1.x; P.p4.y = p1.y;
		}
		else
			if (b == 2)
			{
				if (((R.h1 != NULL) && (R.g1 != NULL))|| ((R.h1 == NULL) && (R.g1 == NULL)))
					P.p1.x = -1000;
				else
				{
					b = (R.h1 != NULL) ? 1 : 3;
					side = (R.h2 != NULL) ? 2 : 4;
					switch (b)
					{
					case 1:
						if (side == 2)
						{
							l1 = paramLine(R.h1);
							l2 = paramLine(R.h2);
							p1 = Colision(l1, l2);
							P.p2.x = p1.x; P.p2.y = p1.y;
							P.p4.x = P.p2.x- 90; P.p4.y = p1.y;
							P.p3.x = P.p2.x-(P.p2.x -P.p4.x)/2; P.p3.y = l2.x*P.p3.x+l2.y;
							P.p1.x = P.p3.x; P.p1.y = l1.x*P.p1.x + l1.y;
						}else
						if (side == 4)
						{
							l1 = paramLine(R.h1);
							l2 = paramLine(R.g2);
							p1 = Colision(l1, l2);
							P.p1.x = p1.x; P.p1.y = p1.y;
							P.p3.x = P.p1.x; P.p3.y = p1.y+ 90;

							P.p4.y = P.p3.y - (P.p3.y - P.p1.y) / 2; P.p4.x = (P.p4.y - l2.y) / l2.x;
							P.p2.y = P.p4.y; P.p2.x = (P.p2.y - l1.y) / l1.x;
						}					
						break;
					case 3:
						if (side == 4)
						{
							l1 = paramLine(R.g1);
							l2 = paramLine(R.g2);
							p1 = Colision(l1, l2);
							P.p4.x = p1.x; P.p4.y = p1.y;

							P.p2.x = P.p4.x + 90; P.p2.y = p1.y;

							P.p3.x = P.p2.x - (P.p2.x - P.p4.x) / 2; P.p3.y = l1.x*P.p3.x + l1.y;
							P.p1.x = P.p3.x; P.p1.y = l2.x*P.p1.x + l2.y;
						}
						else
							if (side == 2)
							{
								l1 = paramLine(R.g1);
								l2 = paramLine(R.h2);
								p1 = Colision(l1, l2);
								P.p3.x = p1.x; P.p3.y = p1.y;

								P.p1.x = P.p3.x; P.p1.y = p1.y - 90;
								P.p4.y = P.p3.y - (P.p3.y - P.p1.y) / 2; P.p4.x = (P.p4.y - l1.y) / l1.x;
								P.p2.y = P.p4.y; P.p2.x = (P.p2.y - l2.y) / l2.x;
							}
						break;
					}
				}
			}		
			else P.p1.x = -1000;
	return P;
}

Cross crossingRect(param R)
{
	Cross P;
	int b = 0;
	int side = 0;
	if (R.h1 != NULL)
		b++;
	if (R.h2 != NULL)
		b++;
	if (R.g1 != NULL)
		b++;
	if (R.g2 != NULL)
		b++;
	if (b == 3)
	{
		if ((R.h1 == NULL) || (R.h2 == NULL))
			side = (R.h1 == NULL) ? 1 : 2;	
		else
			side = (R.g1 == NULL) ? 3 : 4;
		switch (side)
		{
		case 1:
			P.p2.x = R.h2[0].x; P.p2.y = R.g1[0].y;
			P.p4.x = P.p2.x; P.p4.y = R.g2[0].y;
			P.p1.x = P.p2.x-(P.p4.y- P.p2.y); P.p1.y = P.p2.y;
			P.p3.x = P.p1.x; P.p3.y = P.p4.y;
			break;
		case 2:
			P.p1.x = R.h1[0].x; P.p1.y = R.g1[0].y;
			P.p3.x = P.p1.x; P.p3.y = R.g2[0].y;
			P.p2.x = P.p1.x+(P.p3.y - P.p1.y ); P.p2.y = P.p1.y;
			P.p4.x = P.p2.x; P.p4.y =  P.p3.y;
			break;
		case 3:
			P.p3.x = R.h1[0].x; P.p3.y = R.g2[0].y;
			P.p4.x = R.h2[0].x; P.p4.y = P.p3.y;

			P.p1.x = P.p3.x; P.p1.y = P.p3.y-(P.p4.x- P.p3.x);
			P.p2.x = P.p4.x; P.p2.y = P.p1.y;
			break;
		case 4:
			P.p1.x = R.h1[0].x; P.p1.y = R.g1[0].y;
			P.p2.x = R.h2[0].x; P.p2.y = P.p1.y;

			P.p3.x = P.p1.x; P.p3.y = P.p1.y + (P.p2.x - P.p1.x);
			P.p4.x = P.p2.x; P.p4.y = P.p3.y;
			break;
		}
		

		
	}
	else
		if (b == 4)
		{
			P.p1.x = R.h1[0].x; P.p1.y = R.g1[0].y;
			P.p2.x = R.h2[0].x; P.p2.y = P.p1.y;
			P.p3.x = P.p1.x; P.p3.y = R.g2[0].y;
			P.p4.x = P.p2.x; P.p4.y = P.p3.y;
		}
		else
			P.p1.x = -1000;
	return P;

}

bool compareSquare(Cross R, vector<vector<Point> >& circles, int radius)
{
	double s1 = 0;
	double s2 = 0;
	s1 = sqrt((R.p1.x - R.p2.x)*(R.p1.x - R.p2.x) + (R.p1.y - R.p2.y)*(R.p1.y - R.p2.y))*sqrt((R.p4.x - R.p2.x)*(R.p4.x - R.p2.x) + (R.p4.y - R.p2.y)*(R.p4.y - R.p2.y));
	if(!circles.empty())
	for (int i = 0; i < circles.size(); i++)
	{
		if (s2 < fabs(contourArea(Mat(circles[i]))))
		{
			s2 = fabs(contourArea(Mat(circles[i])));
		}
	}
	else
	{
		s2 = CV_PI*radius*radius;
	}
	
	if(fabs(s2-s1)<=1000)
	return true;
	else return false;

}

void compareRombFromTrilange(vector<vector<Point> >& rects, vector<vector<Point> >& t)
{
	Point minY, maxY;
	for (size_t i = 0; i < rects.size(); i++)
	{
		minY.y = 10000;
		maxY.y = -100;
		const Point* p = &rects[i][0];
		int n = (int)rects[i].size();

		for (int j = 0; j < n; j++)
		{
			if (minY.y > p[j].y)
			{
				minY.y = p[j].y;
				minY.x = j;
			}
			if (maxY.y < p[j].y)
			{
				maxY.y = p[j].y;
				maxY.x = j;
			}
		}
		int k = 0;
		int b = 0;
		for (int j = 0; j < n; j++)
		{
			if (minY.x!=j && fabs(minY.y - p[j].y)<=13 && fabs(p[j].x-p[minY.x].x)>=20)
			{
				k++;
			}
			if (maxY.x != j && fabs(maxY.y - p[j].y) <= 13 && fabs(p[j].x - p[maxY.x].x) >= 20)
			{
				b++;
			}
		}

		
		if (b>0 || k>0)
		{
			t.push_back(rects[i]);
			rects.erase(rects.begin() + i);
		}

	
	}
}


bool compareSquareTrilange(vector<vector<Point> >& rects, vector<vector<Point> >& t)
{
	double s1 = 0;
	double s2 = 0;
	
	for (int i = 0; i < rects.size(); i++)
	{
		if (s1 < fabs(contourArea(Mat(rects[i]))))
		{
			s1 = fabs(contourArea(Mat(rects[i])));
		}
	}
	if (!t.empty())
		for (int i = 0; i < t.size(); i++)
		{
			if (s2 < fabs(contourArea(Mat(t[i]))))
			{
				s2 = fabs(contourArea(Mat(t[i])));
			}
		}
	else
	{
		return true;
	}

	if (s1 > s2)
	{
		t.clear();
		return true;		
	}
	else
	{
		rects.clear();
		return false;
	}

}


doublePoint paramLine(CvPoint* line)
{
	doublePoint p;
	if (line[1].x != line[0].x)
	{
		p.x = (double)(line[1].y - line[0].y) / (line[1].x - line[0].x);
		p.y = line[1].y-p.x*line[1].x;
	}
	else
	{
		p.x = -1000;
		p.y = -1000;
	}
	return p;
}



doublePoint Colision(doublePoint p1, doublePoint p2)
{
	doublePoint p;
	if (p2.x != p1.x)
	{
		p.x = (double)(p1.y - p2.y) / (p2.x - p1.x);
		p.y = p1.x*p.x + p1.y;
	}
	else
	{
		p.x = -1000;
		p.y = -1000;
	}

	return p;
}
int thresh = 50, N = 11;
static double angle1(doublePoint pt1, doublePoint pt2)
{	
	return fabs(pt1.x*pt2.x + 1) / (sqrt(1 + pt1.x*pt1.x)*sqrt(1 + pt2.x*pt2.x));
}


static double angle(Point pt1, Point pt2, Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}


static void findShapes(const Mat& image, vector<vector<Point> >& rects, vector<vector<Point> >& trilanges, vector<vector<Point> >& circles)
{
	rects.clear();
	trilanges.clear();
	circles.clear();

	Mat pyr, timg, gray0(image.size(), CV_8U), gray;

	pyrDown(image, pyr, Size(image.cols / 2, image.rows / 2));
	pyrUp(pyr, timg, image.size());
	vector<vector<Point> > contours;

	for (int c = 0; c < 3; c++)
	{
		int ch[] = { c, 0 };
		mixChannels(&timg, 1, &gray0, 1, ch, 1);

		for (int l = 0; l < N; l++)
		{
			if (l == 0)
			{
				Canny(gray0, gray, 0, thresh, 5);
				//imshow("df", gray);

				dilate(gray, gray, Mat(), Point(-1, -1));

				
			}
			else
			{
				gray = gray0 >= (l + 1) * 255 / N;
			}

			findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

			vector<Point> approx;
			double maxCosine;
			for (size_t i = 0; i < contours.size(); i++)
			{
				approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);
				if (approx.size() >= 3 && fabs(contourArea(Mat(approx))) > 1000 && fabs(contourArea(Mat(approx))) <= 10000 && isContourConvex(Mat(approx)))
				{
					maxCosine = 0;

					for (int j = 2; j < approx.size() + 1; j++)
					{
						double cosine = fabs(angle(approx[j % approx.size()], approx[j - 2], approx[j - 1]));
						maxCosine = MAX(maxCosine, cosine);
					}
					
					switch (approx.size())
					{
					case 4:
						if (maxCosine < 0.3)
							rects.push_back(approx);
						break;
					case 3:
						if (maxCosine >= 0.3 && maxCosine <= 0.64)
						{
							trilanges.push_back(approx);
						}
						break;
					default:
						if (approx.size() >= 7 && maxCosine >= 0.76)
							circles.push_back(approx);
						if (approx.size() >= 5 && approx.size() <= 7 && maxCosine >= 0.76)
							rects.push_back(approx);
						break;
						
					}
					
				}
			}
		}
	}
}

bool kindOfRectangle(const vector<vector<Point> >& squares)
{
	int y1 = -1000, y2 = -1000, y3 = -1000;
	Point minY, maxY;
	
	bool f = false;
	bool g = false;
	for (size_t i = 0; i < squares.size(); i++)
	{
		 f = false;
		 g = false;
		 minY.y = 10000;
		 maxY.y = -100;
		const Point* p = &squares[i][0];
		int n = (int)squares[i].size();
		
		for (int j = 0; j < n; j++)
		{
			if (minY.y > p[j].y)
			{
				minY.y = p[j].y;
				minY.x = j;
			}
			if (maxY.y <p[j].y)
			{
				maxY.y = p[j].y;
				maxY.x = j;
			}			
		}
		int k = 0;
		for (int j = 0; j < n; j++)
		{
			if (maxY.y > 64 && (maxY.x != j && fabs(p[j].y - maxY.y) <= 13))
			{
				f = true;
				k++;
			}
			if (minY.y < 64 && (minY.x != j && fabs(p[j].y - minY.y) <= 13))
			{
				g = true;
				k++;
			}
		
		}
		if (k)
			continue;
		if (!(f & g))
			return false;

	}
	if (f & g)
		return true;
	else return false;


}


bool kindOfTrilange(const vector<vector<Point> >& squares)
{
	int y1 = -1000, y2 = -1000, y3 = -1000;
	bool f=true;
	Point minY, maxY;
	for (size_t i = 0; i < squares.size(); i++)
	{
		minY.y = 10000;
		maxY.y = -100;
		const Point* p = &squares[i][0];
		int n = (int)squares[i].size();
		if (n == 3)
		{
			if (fabs(p[0].y - p[1].y) <= 10)

				if (p[0].y < p[2].y)
					f = false;
				else
					f = true;
			else 	if (fabs(p[2].y - p[1].y) < 10)
				if (p[2].y < p[0].y)
					f = false;
				else
					f = true;
			else
				if (p[2].y < p[1].y)
					f = false;
				else f = true;
		}
		else
		{
			for (int j = 0; j < n; j++)
			{
				if (minY.y > p[j].y)
				{
					minY.y = p[j].y;
					minY.x = j;
				}
				if (maxY.y < p[j].y)
				{
					maxY.y = p[j].y;
					maxY.x = j;
				}
			}
			int k = 0;
			int b = 0;
			for (int j = 0; j < n; j++)
			{
				if (minY.x != j && fabs(minY.y - p[j].y) <= 13 && fabs(p[j].x - p[minY.x].x) >= 20)
				{
					k++;
				}
				if (maxY.x != j && fabs(maxY.y - p[j].y) <= 13 && fabs(p[j].x - p[maxY.x].x) >= 20)
				{
					b++;
				}
			}

			if (b>0)
			{
				f = true;
			}
			else
				f = false;
		}
		
	}
	return f;
}

// the function draws all the squares in the image
static void drawSquares(Mat& image, const vector<vector<Point> >& squares)
{

	if (squares.size() == 0)
		return;
	const Point* p = &squares[squares.size()-1][0];
	int n = (int)squares[squares.size() - 1].size();
	polylines(image, &p, &n, 1, true, Scalar(0, 255, 0), 3, LINE_AA);
	

}

wstring string_to_wstring(string& s)
{
	ofstream ofs("temp.txt", ofstream::out);
	if (!ofs) { cerr << "don't open file: 'temp.txt1' " << endl; exit(1); }
	ofs << s;
	ofs.close();

	wstring s1;
	wifstream wifs("temp.txt", wifstream::in);
	if (!wifs) { cerr << "don't open file: 'temp.txt2'" << endl; exit(1); }
	wifs >> s1;
	wifs.close();
	return s1;
}

string wstring_to_string(wstring s)
{
	wofstream wofs("temp.txt", wofstream::out);
	if (!wofs) { cerr << "don't open file: 'temp.txt3'" << endl; exit(1); }
	wofs << s;
	wofs.close();

	string s1;
	ifstream ifs("temp.txt", ifstream::in);
	if (!ifs) { cerr << "don't open file: 'temp.txt3'" << endl; exit(1); }
	ifs >> s1;
	ifs.close();
	return s1;
}

vector<string> scandir(string p)
{
	wstring Path = string_to_wstring(p);

	WIN32_FIND_DATA FindFileData;
	HANDLE hf;

	hf = FindFirstFile((string_to_wstring(p)).c_str(), &FindFileData);

	vector<string> v;
	if (hf != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
				v.push_back(wstring_to_string(FindFileData.cFileName));
		} while (FindNextFile(hf, &FindFileData) != 0);
	}

	return v;
}
