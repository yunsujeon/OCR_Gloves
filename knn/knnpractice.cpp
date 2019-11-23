#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/ml/ml.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
#include <fcntl.h>

#define BUF_MAX 512

using namespace cv;
using namespace std;

const int MIN_CONTOUR_AREA = 100;
const int RESIZED_IMAGE_WIDTH = 20;
const int RESIZED_IMAGE_HEIGHT = 30;

class ContourWithData //ContourWithData 클래스 생성
{
	public:
		vector<Point> ptContour;
		Rect boundingRect;
		float fltArea;

		bool checkIfContourIsValid()
		{                            
			if (fltArea < MIN_CONTOUR_AREA) return false;  //컨투어의 영역이 일정부분 이상 이하인 것을 인식한다. 
			return true;    
		}

		static bool sortByBoundingRectXPosition(const ContourWithData& cwdLeft, const ContourWithData& cwdRight)
		{      
			return(cwdLeft.boundingRect.x < cwdRight.boundingRect.x);  //컨투어는 좌측에서부터 우측으로
		}
};

Mat structuring_element(10,10,CV_8U,Scalar(1));

int process(Mat img_input, Mat& img_result)
{
	cvtColor(img_input, img_result, COLOR_RGBA2GRAY);
	return (0);
}

int process2(Mat img_result, Mat& img_binary)
{
	threshold(img_result, img_binary, 240, 255, THRESH_BINARY_INV | THRESH_OTSU);
}

int process3(Mat img_binary, Mat& img_dilated)
{
	dilate(img_binary,img_dilated,structuring_element);
}

int main(int, char**)
{
	int papaindex=0;
	int m=0;
	int index=0;
	int no_white=0;
	int fd,i=0;
	char tmp,read_byte=0;
	char buf[256];
	char fub[256];
	
	int a, b, c, d;
	char buffer[BUF_MAX];
	struct termios newtio;

	fd = open( "/dev/ttyUSB0", O_RDWR | O_NOCTTY ); // /dev/ttyACM0 사용하기 위>해 오픈해준다.

	if(fd<0)
	{
		fprintf(stderr,"ERR\n"); exit(-1);
	} // 안열리면 나가기

	memset( &newtio, 0, sizeof(newtio) ); // 시리얼 통신환경 설정을 위한 구조체 변수값을 0바이트로 초기화

	newtio.c_cflag = B9600; //통신 속도 9600
	newtio.c_cflag |= CS8; //데이터 비트 8비트
	newtio.c_cflag |= CLOCAL; //내부 통신포트 사용, 외부모뎀 없이
	newtio.c_cflag |= CREAD; //쓰기 기본, 읽기 가능하게하기
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 0;

	tcflush (fd, TCIFLUSH );
	tcsetattr(fd, TCSANOW, &newtio ); //포트에 대한 통신 환경설정

	//웹캡으로 부터 데이터 읽어오기 위해 준비 
	VideoCapture cap(1);

	if (!cap.isOpened())
	{
		printf("카메라를 열수 없습니다. \n");
	}

	Mat frame;
	Mat frame2;
	
	namedWindow("camera1", 1);
	namedWindow("camera2", 1);

	for (;;)
	{
		//웹캡으로부터 한 프레임을 읽어옴 
		cap >> frame;
		cap >> frame2;

//use reflect mirror		
/*		flip(frame,frame,-1);
		flip(frame,frame,1);
		flip(frame2,frame2,-1);
		flip(frame2,frame2,1);
*/
		//그레이 스케일로 변경하는 함수 실행
		Mat img_result;
		process( frame, img_result );

		//이진 스케일로 변경하는 함수 실행
		Mat img_binary;
		process2( img_result, img_binary);

		//글자 개별 크기 확대하는 함수 실행
		Mat img_dilated;
		process3( img_binary, img_dilated);

		Mat img_labels, stats, centroids, img_cut;

		int numOfLables = connectedComponentsWithStats(img_dilated, img_labels, stats, centroids, 8,CV_32S);

		//라벨링 된 이미지에 각각 직사각형으로 둘러싸기
		
		for (int j = 1; j < numOfLables; j++)
		{
			int area = stats.at<int>(j, CC_STAT_AREA);
			int left = stats.at<int>(j, CC_STAT_LEFT);
			int top  = stats.at<int>(j, CC_STAT_TOP);
			int width = stats.at<int>(j, CC_STAT_WIDTH);
			int height = stats.at<int>(j, CC_STAT_HEIGHT);

			rectangle(frame, Point(left,top), Point(left+width,top+height), Scalar(0,255,0),1 );

			line(frame, Point(350,240), Point(350,280), Scalar(0,255,0), 2,8,0);

			if(m == 0 && top+height < 280 && top+height > 240 && left < 350 && left+width >350)
			{
				a = left;
				b = top;
				c = width;
				d = height;
				m = 1;
			}

			Range cols(350,351);
			Range rows(200,300);
			img_cut =  img_dilated(rows,cols);

			no_white = countNonZero(img_cut);

			if(m == 2 && top+height < 280 && top+height > 240 && no_white <1 )
			{
				m = 0;
			}
		
		}
		if(m == 1 && b+d < 280 && b+d > 240 && a < 350 && a+c >350)
		{		
			papaindex=index;
			Rect rect(a,b,c,d);
			Mat subimage = frame2(rect);
			sprintf(buf,"/home/jeon/OCR_Gloves/knn/imgsaves/testimg_%06d.png",index);
			cout<<buf<<endl;
			imwrite(buf,subimage);
			index++;

			if(index==99)
			{
				index=0;
			}
			m = 2;

			vector<ContourWithData> allContoursWithData;           // 벡터 선언,
			vector<ContourWithData> validContoursWithData;        

			Mat matClassificationInts;      //classification numbers 를 읽은 후 이 값에 저장 할 것이다.

			FileStorage fsClassifications("classifications.xml", FileStorage::READ);        // 학습한  classifications 파일을 연다

			if (fsClassifications.isOpened() == false)
			{                                                
				cout << "error, unable to open training classifications file, exiting program\n\n";   
				return(0);                                                                            
			}

			fsClassifications["classifications"] >> matClassificationInts;      // classifications section을 읽는다.
			fsClassifications.release();  // classifications 파일을 닫는다.

			Mat matTrainingImagesAsFlattenedFloats;         // 여러개의 이미지를 읽어들이는 변수

			FileStorage fsTrainingImages("images.xml", FileStorage::READ);          // training된 이미지 파일을 연다.

			if (fsTrainingImages.isOpened() == false)
			{                                                 
				cout << "error, unable to open training images file, exiting program\n\n";
				return(0);                                                                
			}

			fsTrainingImages["images"] >> matTrainingImagesAsFlattenedFloats;           // 이미지 섹션을 Mat  훈련 이미지 변수로 읽는다.
			fsTrainingImages.release();                                                 // 이미지 파일을 닫는다.

			// train 

			Ptr<ml::KNearest>  kNearest(ml::KNearest::create());            // KNN object를 나타낸다

			// 훈련을 요청한다. 두 매개변수는 모두 Mat 형식이여야 한다.
			// 실제로는  multiple images / numbers 이지만..
			kNearest->train(matTrainingImagesAsFlattenedFloats, ml::ROW_SAMPLE, matClassificationInts);


			//Mat matTestingNumbers = imread("/home/dotheart/OCR_Gloves/knn/imgsaves/testimg_%6d.png",papaindex);            // read in the test numbers image
			Mat matTestingNumbers = imread(buf,IMREAD_COLOR);   // test numbers image를 읽어온다.

			if (matTestingNumbers.empty())
			{                              
				std::cout << "error: image not read from file\n\n";       
				return(0);                                                
			}

			Mat matGrayscale;           
			Mat matBlurred;             
			Mat matThresh;              
			Mat matThreshCopy;         

			cvtColor(matTestingNumbers, matGrayscale, COLOR_RGB2GRAY);         // grayscale로 변환한다.

			GaussianBlur(matGrayscale,matBlurred,Size(5, 5),0);     //Blur 처리한다.

			adaptiveThreshold(matBlurred,matThresh,255,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY_INV,11,2);   //바이너리 이미지로 변환한다.

			matThreshCopy = matThresh.clone();              // 카피 이미지를 만든다.

			vector<vector<Point> > ptContours;        // 컨투어를 위한 벡터를 정의해준다.
			vector<Vec4i> v4iHierarchy;               // 컨투어의 hierarchy를 정의해준다. (we won't use this in this program but this may be helpful for reference)

			findContours(matThreshCopy,ptContours,v4iHierarchy,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);

			for (int i = 0; i < ptContours.size(); i++)
			{               
				ContourWithData contourWithData;                                                    // instantiate a contour 데이터 object를 나타낸다.
				contourWithData.ptContour = ptContours[i];                                          // 데이터로 윤곽선 지정
				contourWithData.boundingRect = cv::boundingRect(contourWithData.ptContour);         // bounding rect 추출 (생성됨)
				contourWithData.fltArea = contourArea(contourWithData.ptContour);               // contour area 를 계산한다.
				allContoursWithData.push_back(contourWithData);                                     // 데이터가있는 모든 contour  목록에 contour data 개체 추가
			}

			for (int i = 0; i < allContoursWithData.size(); i++)
			{                      
				if (allContoursWithData[i].checkIfContourIsValid())
				{                  
					validContoursWithData.push_back(allContoursWithData[i]); // 유효한 contour list 검출
				}
			}

			// 유효한 컨투어들을 좌측에서부터 정렬한다.
			sort(validContoursWithData.begin(), validContoursWithData.end(), ContourWithData::sortByBoundingRectXPosition);

			string strFinalString;         //최종 문자열을 선언합니다. 이것은 프로그램 끝까지 최종 숫자 시퀀스를 갖습니다.

			for (int i = 0; i < validContoursWithData.size(); i++)
			{            

				// 초록 사각형을 둘러서 그린다.
				rectangle(matTestingNumbers,validContoursWithData[i].boundingRect,Scalar(0, 255, 0),2);

				Mat matROI = matThresh(validContoursWithData[i].boundingRect);          // ROI image 를 컨투어 영역으로부터 따온다. (관심영역)

				Mat matROIResized;
				resize(matROI, matROIResized, Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT));     // 이미지 resize (인식과 저장에 용이)

				Mat matROIFloat;
				matROIResized.convertTo(matROIFloat, CV_32FC1);             // Mat 형에서 float형으로 변환, find_nearest를 부르는데 용이하다.

				Mat matROIFlattenedFloat = matROIFloat.reshape(1, 1);

				Mat matCurrentChar(0, 0, CV_32F);

				kNearest->findNearest(matROIFlattenedFloat, 1, matCurrentChar);     // 데이터 중  find_nearest 를 찾아 불러올 수 있다. (인식할 수 있게 된 것)

				float fltCurrentChar = (float)matCurrentChar.at<float>(0, 0);

				strFinalString = strFinalString + char(int(fltCurrentChar)); 

			}


			cout << "\n" << "numbers read = " << strFinalString << "\n";       // show the full string

			imshow("camera2", matTestingNumbers);

			write(fd,strFinalString.c_str(),strFinalString.length());
			i=read(fd,buffer,BUF_MAX);
			buffer[i]='\0';

			
/*
			ofstream fout;

			sprintf(fub,"/home/dotheart/OCR_Gloves/knn/string_text/text_%06d.txt",papaindex);

			fout.open(fub);		

			fout << strFinalString << '\n'; //2진으로 변환되기 전 텍스트 형식의 알파벳 출력

			char cstr[20];

			strcpy(cstr,strFinalString.c_str());

			for(int i=0; i<cstr[i] != 0; i++)//cstr을 한글자씩 불러온다.
			{
				string asdf = conversion(cstr[i]); //function name : conversion

				fout << asdf  ; //2진으로 변환 된 후 텍스트 형식의 숫자 출력

			}

			fout.close();
*/

		}
		imshow("camera1", frame) ;
		if(waitKey(20)>=0) break;

	}

	close(fd);
	return 0;

}

