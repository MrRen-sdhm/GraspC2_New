//
// Created by hj on 18-12-29.
//

#ifndef MAGICCUBE_GRAPHICSCUBE_H
#define MAGICCUBE_GRAPHICSCUBE_H
#include <opencv2/opencv.hpp>
#include <QString>
#include <vision/Calibration/interface/CobotCalibrationData.h>
#include <vision/Calibration/interface/CalToolkit.h>

#include <pcl/io/pcd_io.h>

// GPD
//#include <gpd/util/cloud.h>
//#include <gpd/grasp_detector_pointnet.h>

// YOLO
#include "YoloLibtorchDetector.h"

namespace {
    //矩阵转化为笛卡尔坐标
    auto ConvertTransform2XYZRPY = [](const ::Eigen::Isometry3d &transform) {

        Eigen::Vector3d translation = transform.translation();
        Eigen::Matrix3d rotation = transform.rotation();
        Eigen::Vector3d rpy = rotation.eulerAngles(2, 1, 0);

        std::vector<double> xyzRpy = {translation[0], translation[1], translation[2],
                                      rpy[2], rpy[1], rpy[0]};

        return xyzRpy;
    };

    //笛卡尔坐标转化为矩阵
    auto ConvertXYZRPY2Transform = [](std::vector<double> xyzRpy) {

        Eigen::Vector3d translation(xyzRpy[0], xyzRpy[1], xyzRpy[2]);

        Eigen::Matrix3d rotation = Eigen::AngleAxisd(xyzRpy[5], Eigen::Vector3d::UnitZ()) *
                                   Eigen::AngleAxisd(xyzRpy[4], Eigen::Vector3d::UnitY()) *
                                   Eigen::AngleAxisd(xyzRpy[3], Eigen::Vector3d::UnitX()).matrix();

        Eigen::Isometry3d transform = Eigen::Isometry3d::Identity();
        transform.rotate(rotation);
        transform.pretranslate(translation);

        return transform;
    };
}

template <class Type>
Type stringToNum(const string& str)
{
    istringstream iss(str);
    Type num;
    iss >> num;
    return num;
}

class GraphicsGrasp {
public:
    GraphicsGrasp();

    std::pair<std::vector<cv::RotatedRect>, std::vector<int>> detectGraspYolo(cv::Mat &image, bool show);

    /**
     * calcRealCoor  输入kinect点云图像抓取姿态，依据手眼标定结果，输出手臂基坐标系下抓取姿态
     * @param rotMatrix  相机坐标系下抓取姿态的旋转矩阵
     * @param position  相机坐标系下抓取姿态的平移向量
     * @param leftOrRight  0: 左臂 1：右臂
     * @return 手臂基坐标系下抓取姿态(Eigen::Isometry3d格式)
     */
    static std::vector<double> calcRealCoor(const Eigen::Matrix3d& rotMatrix, const Eigen::Vector3d& translation, int leftOrRight);

    /// 获取物体姿态和ID
    std::vector<double> getObjPose(const cv::RotatedRect& RotRect,
                                                  const pcl::PointCloud<pcl::PointXYZRGBA>::Ptr& cloud, int leftOrRight);

    /**
     * \brief Detect grasp poses in a point cloud.
     * \return the list of grasp poses
    */
//    std::vector<std::unique_ptr<gpd::candidate::Hand>> detectGraspPoses(pcl::PointCloud<pcl::PointXYZRGBA>::Ptr &cloud); //FIXME

    /// 创建点云 缩小一倍 color(1920*1080) depth(1920*1080) cloud(960*540)
    void createPointCloud(cv::Mat &color, cv::Mat &depth, const pcl::PointCloud<pcl::PointXYZRGBA>::Ptr& cloud);

    void createLookup(size_t width, size_t height);

    static void readCameraInfo(cv::Mat &cameraMatrix);

private:
    /// GPD FIXME
//    gpd::GraspDetectorPointNet* grasp_detector_; ///< used to run the GPD algorithm
//    gpd::util::Cloud* cloud_camera_; ///< stores point cloud with (optional) camera information and surface normals

    /// YOLO
    YoloLibtorchDetector* yoloDetector;

    // 点云生成相关参数
    cv::Mat lookupX, lookupY; // 像素坐标转换到相机坐标
};

#endif //MAGICCUBE_GRAPHICSCUBE_H
