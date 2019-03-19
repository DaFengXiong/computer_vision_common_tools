#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <pcl/io/ply_io.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <fstream>


using namespace cv;
using namespace std;


double fx = 540.6860;
double fy = 540.6860;
double cx = 479.7500;
double cy = 269.7500;

const double camera_factor = 1000;

string rgb_dir_name = "rgb\\";
string depth_dir_name= "depth\\";
string pointcloud_dir_name = "pointcloud\\";

typedef pcl::PointXYZRGB PointT;
typedef pcl::PointCloud<PointT> PointCloud;


void generate_pointcloud(char filepath[], string rgbfilename, string depthfilename) {


	string file_rgb = filepath + rgb_dir_name +rgbfilename;
	string file_depth = filepath + depth_dir_name + depthfilename;

	cout << file_rgb << endl;
	cout << file_depth << endl;

	cv::Mat rgb = imread(file_rgb);
	cv::Mat depth = imread(file_depth, 2);

	PointCloud cloud;
	// �������ͼ


	double xxx, yyy, zzz;
	for (int m = 0; m < depth.rows; m++)
		for (int n = 0; n < depth.cols; n++)
		{
			// ��ȡ���ͼ��(m,n)����ֵ

			//ushort d = depth.at<ushort>(m, n);

			ushort d = depth.ptr<ushort>(m)[n];
			// d ����û��ֵ������ˣ������˵�
			if (d == 0)
				continue;
			// d ����ֵ�������������һ����
			PointT p;

			// ���������Ŀռ�����

			xxx = double(d) / camera_factor;
			yyy = (n - cx) * xxx / fx;
			zzz = (m - cy) * xxx / fy;


			p.x = xxx;
			p.z = -zzz;
			p.y = -yyy;




			// ��rgbͼ���л�ȡ������ɫ
			// rgb����ͨ����BGR��ʽͼ�����԰������˳���ȡ��ɫ
			p.b = rgb.ptr<uchar>(m)[n * 3];
			p.g = rgb.ptr<uchar>(m)[n * 3 + 1];
			p.r = rgb.ptr<uchar>(m)[n * 3 + 2];

			// ��p���뵽������
			cloud.points.push_back(p);
			//cout << cloud->points.size() << endl;
		}
	// ���ò��������
	cloud.height = 1;
	cloud.width = cloud.points.size();
	//cout << "point cloud size = " << cloud->points.size() << endl;
	cloud.is_dense = false;

	string plyfilename = rgbfilename.substr(0, rgbfilename.length() - 4) + ".ply";

	cout << filepath + pointcloud_dir_name + plyfilename << endl;
	try {
		//�������ͼ
		pcl::io::savePLYFile(filepath + pointcloud_dir_name + plyfilename, cloud);
	}
	catch (pcl::IOException &e) {
		cout << e.what() << endl;
	}

	
}



int main(int argc, char** argv)
{

	string rgb_depth_filename = "rgb_depth_filename.txt";

	

	ifstream rgb_depth_file(argv[1]+ rgb_depth_filename, ios::in);

	if (rgb_depth_file.is_open())
	{
		cout << argv[1] << " open successed! " << endl;	
	}
	else
	{
		cout << argv[1] << " open failed! " << endl;
		return 0;
	}


	char buffer[20];
	string rgb_filename;
	string depth_filename;

	while (!rgb_depth_file.eof())
	{
		rgb_depth_file.getline(buffer, sizeof(buffer));
		stringstream filenames(buffer);

		filenames >> rgb_filename;
		filenames >> depth_filename;

		cout << rgb_filename << " " << depth_filename << endl;

		generate_pointcloud(argv[1], rgb_filename, depth_filename);



	}



	rgb_depth_file.close();
	return 0;
}