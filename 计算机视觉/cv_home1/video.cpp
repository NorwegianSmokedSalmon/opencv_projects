#include <dirent.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <string>
#include <sys/io.h>
#include <sys/types.h>
#include <vector>

using namespace std;
using namespace cv;


bool GradientEffect = false;//实现图片切换的渐变效果
bool GridEffect = true;

vector<Mat> images_mrege;

string path0 = "../data";
string result_path = "../result";
vector<string> addresses;
vector<string> images_address;
vector<string> video_address;
vector<Mat> images;
String title = "CV-HW1 by Wang Lijie";
String sub_title = "3210101760 Wang Lijie";
Point bottom_start;
Point middle_start;
double title_scale = 2.0;
double subtitle_scale = 1.5;
int fontFace = FONT_HERSHEY_SCRIPT_COMPLEX;
Scalar black = Scalar(0, 0, 0);
Scalar white = Scalar(255, 255, 255);
int title_thickness = 3;
int subtitle_thickness = 2;
int lineType = LINE_AA;
bool bottomLeftStart = false;
int grid_width = 100;
// linux 获取图像数据
vector<string> getFiles(string path)
{
    vector<string> address;
    DIR *pDir;
    struct dirent *ptr;
    if(!(pDir = opendir(path.c_str()))){
        cout<<"Folder doesn't Exist!"<<endl;
    }
    while ((ptr = readdir(pDir)) != 0)
    {
        if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0)
        {
            address.push_back(path + "/" + ptr->d_name);
        }
    }
    closedir(pDir);
    return address;
}
Mat getGridImage(Mat image, int gridWidth){
    int h = image.rows;
    int w = image.cols;
    Mat change = image;
    for(int row = 0; row < h; row++){
        for(int col = 0; col < w; col++){
            if(row%100<=gridWidth){
                change.at<Vec3b>(row, col)[0] = 0;
				change.at<Vec3b>(row, col)[1] = 0;
				change.at<Vec3b>(row, col)[2] = 0;              
            }
        }
    }
    return change;
}
Mat getGridImage_2(Mat image, int gridWidth){
    int h = image.rows;
    int w = image.cols;
    Mat change = image;
    for(int row = 0; row < h; row++){
        for(int col = 0; col < w; col++){
            if(row%100>gridWidth){
                change.at<Vec3b>(row, col)[0] = 0;
				change.at<Vec3b>(row, col)[1] = 0;
				change.at<Vec3b>(row, col)[2] = 0;              
            }
        }
    }
    return change;
}
Mat getGridImage_1(Mat image, int realWidth){
    int h = image.rows;
    int w = image.cols;
    Mat record = image;
    Mat black = image;
    for(int row = 0; row < h; row++){
        for(int col = 0; col < w; col++){
            black.at<Vec3b>(row, col)[0] = 0;
			black.at<Vec3b>(row, col)[1] = 0;
			black.at<Vec3b>(row, col)[2] = 0;
        }
    }
    for(int row = 0; row < h; row++){
        for(int col = 0; col < w; col++){
            if(row%100>realWidth){
               black.at<Vec3b>(row, col) = image.at<Vec3b>(row, col);
			   //black.at<Vec3b>(row, col)[1] = record.at<Vec3b>(row, col)[1];
			   //black.at<Vec3b>(row, col)[2] = record.at<Vec3b>(row, col)[2];
            }
        }
    }
    return black;
}
Mat Gradient_effect(Mat image1, Mat image2){
   double alpha = 0.5;
   double beta = 1 - alpha;
   double gamma = 0;
   Mat merge;
   addWeighted(image1, alpha, image2, beta, gamma, merge, -1);
   return merge;
}

int main()
{
    // load images and video, 先全部存储在addresses中，再进行划分，分别存储在images_address,  video_address
    int index = 0;
    addresses = getFiles(path0);
    for (int i = 0; i < addresses.size(); i++)
    {
        cout << addresses[i] << endl;

        if (addresses[i].find(".mp4") != -1)
        {
            index = i;
            video_address.push_back(addresses[i]);
        }
        else
        {
            images_address.push_back(addresses[i]);
        }
    }   
   
    
    VideoCapture video_capture(video_address[0]);
    double height = video_capture.get(CAP_PROP_FRAME_HEIGHT);
    double width = video_capture.get(CAP_PROP_FRAME_WIDTH);
    double fps = video_capture.get(CAP_PROP_FPS);
    bottom_start = Point(width * 0.6 - sub_title.length() * subtitle_scale * 8, height - subtitle_scale * 40);
    middle_start = Point(width * 0.6 - title.length() * title_scale * 8, height * 0.6);
    Size size = Size(width, height);
    cout<<"fps = "<<fps<<endl;

    // resize image
    for (int i = 0; i < images_address.size(); i++)
    {
        Mat image = imread(images_address[i]);
        resize(image, image, size);
        putText(image, sub_title, bottom_start, fontFace, subtitle_scale, black, subtitle_thickness, lineType, bottomLeftStart);
        images.push_back(image);
    }
    
    //VideoWriter Writer(result_path + "/output_gradient.avi", VideoWriter::fourcc('D', 'I', 'V', 'X'), fps, size, true);
    VideoWriter Writer(result_path + "/output_grid.avi", VideoWriter::fourcc('D', 'I', 'V', 'X'), fps, size, true);
    // 写入文字
    Mat title_img(height, width, CV_8UC3, black);
    putText(title_img, title, middle_start, fontFace, title_scale, white, title_thickness, lineType, bottomLeftStart);
    for (int i = 0; i < 2 * fps; i++)
    {
        Writer.write(title_img);
    }

    // 写入图像
    for (int i = 0; i < images.size() - 1; i++)
    {   

        int height_num = images[i].rows;
        int width_num = images[i].cols;
        //写入给定的图像
        for (int k = 0; k < 2 * fps; k++)
        {
            Writer.write(images[i]);
        }

        if(GradientEffect){//实现渐变效果，用写入过渡图像来实现
           for (int k = 0; k <  2 * fps; k++){
            Mat merge;
            addWeighted(images[i], 1 - 0.02 * k, images[i + 1], 0.02 * k, 3, merge);
            Writer.write(merge);
           }
        }
        if(GridEffect){
           Mat trans1 = images[i];
        //Mat trans2 = images[i+1];
        //Mat trans = trans2;
        //vector<Mat> trans;
        for(int m = 0; m < grid_width; m++){
            trans1 = getGridImage(trans1,m);
            //trans2 = getGridImage(images[i+1], 100-m);
            Writer.write(trans1);
            //Writer.write(trans2);
        }
        
            

        }
        // for(int m = 0; m < grid_width; m++){
        //     trans2 = getGridImage_1(trans,m);
        //     //trans.push_back(trans2);
        //     imshow("a", trans2);
        //     waitKey();
        //     //Writer.write(trans2);
        // }
        
        
    }


    //写入最后一张图像
    for (int i = 0; i < 2 * fps; i++)
    {
        Writer.write(images[images.size() - 1]);
    }

    Mat video_frame;
    video_capture >> video_frame;
    

    
    do
    {
        putText(video_frame, sub_title, bottom_start, fontFace, subtitle_scale, black, subtitle_thickness, lineType, bottomLeftStart);
        Writer.write(video_frame);
        video_capture >> video_frame;
    } while (!video_frame.empty());

    if(GradientEffect){
       cout << "输出的合成视频位于：" << result_path << "/output_gradient.avi" << endl;
    }
    if(GridEffect){
       cout << "输出的合成视频位于：" << result_path << "/output_grid.avi" << endl;
    }

    video_capture.release();

    Writer.release();
    return 0;
}
