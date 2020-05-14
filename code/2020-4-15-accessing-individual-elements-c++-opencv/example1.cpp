#include <opencv2/opencv.hpp>

int main (int arg, char** argv)
{
    int size[] {10, 10, 10};
    cv::Mat m(3, size, CV_32F);

    m.at<float>(1, 2, 3);

    /////////////////////////////////////////7
    cv::Mat m1(10, 10, CV_32FC3);

    m1.at<cv::Vec3f>(2, 2);

    //////////////////////////////////////////
    cv::Mat m2(20, 20, CV_8UC3);

    m2.at<cv::Vec3i>(cv::Point2i(10,10));

    //////////////////////////////////////////
    int size3[] {10, 10, 10, 10, 10};
    cv::Mat m3(5,size3, CV_8UC3, cv::Scalar(0));

    int idx[] {1, 2, 3, 4, 5};
    m3.at<uchar>(idx);

    /////////////////////////////////////////
    int size4[] {100, 100, 100};
    cv::Mat m4(3, size4, CV_32F, cv::Scalar(0));

    m4.isContinuous();

    for ( std::size_t i = 0; i != m4.size[0]; i++)
    {
        float* p = m4.ptr<float>(i);
        for (std::size_t j = 0; j != m4.size[1]*m4.size[2]; j++)
        {
            *(p + j);
        }
    }

    ////////////////////////////////////////////////////
    cv::Mat m5(100, 100, CV_32FC3, cv::Scalar(0));

    std::cout << m5.isContinuous() << std::endl;

    for ( std::size_t i = 0; i != m4.size[0]; i++)
    {
        cv::Vec3f* p = m5.ptr<cv::Vec3f>(i);
        for (std::size_t j = 0; j != m5.size[1]; j++)
        {
            *(p + j);
        }
    }

    /////////////////////////////////////////////////////
    int size5[] {100, 100, 100};
    cv::Mat m6(3, size5, CV_32FC3, cv::Scalar(0));

    cv::MatConstIterator_<cv::Vec3f> it = m6.begin<cv::Vec3f>();

    while( it != m6.end<cv::Vec3f>())
    {
        *it
    }

}