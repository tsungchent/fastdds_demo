#include "transport_test.h"
#include "message_publisher.h"
#include "message_subscriber.h"

#include <fastrtps/Domain.h>
#include <fastrtps/log/Log.h>

#include <image.h>
#include <opencv2/opencv.hpp>

#include <chrono>
#include <thread>

std::shared_ptr<image> toImageMsg(const cv::Mat &cvimage, long long timestamp)
{
    std::shared_ptr<image> image_msg = std::make_shared<image>();
    image_msg->chn(cvimage.channels());
    image_msg->timestamp(timestamp);
    image_msg->width(cvimage.cols);
    image_msg->height(cvimage.rows);
    std::vector<uchar> imagedata(cvimage.rows * cvimage.cols * cvimage.channels());
    std::copy(cvimage.data, cvimage.data + imagedata.size(), imagedata.data());
    image_msg->data(imagedata);
    return image_msg;
}

cv::Mat toCvImage(const std::shared_ptr<image> &image_msg)
{
    cv::Mat cvimage(cv::Size(image_msg->width(), image_msg->height()), CV_MAKE_TYPE(CV_8U, image_msg->chn()));
    std::copy(image_msg->data().begin(), image_msg->data().end(), cvimage.data);
    return cvimage;
}

void send_image_test(const std::string load_videopath, double fps)
{
    cv::VideoCapture capture(load_videopath);

    int frame_cout = capture.get(cv::CAP_PROP_FRAME_COUNT);
    int frame_width = capture.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    int frame_chn = capture.get(cv::CAP_PROP_CHANNEL);

    std::vector<std::shared_ptr<image>> images_msgs;

    // preload image
    std::cout << "preload image.";
    int count = 0;
    int max_count = 30 * 10;

    while (count++ < max_count)
    {
        cv::Mat image;
        capture >> image;

        if (image.empty())
            break;

        images_msgs.push_back(toImageMsg(image, count));
    }

    // send image
    std::cout << "test send images, total size:  " << images_msgs.size() << std::endl;
    MessagePublisher pub;
    std::chrono::system_clock clock;
    std::chrono::system_clock::time_point time_base = clock.now();

    if (pub.init(frame_width, frame_height, frame_chn))
    {

        // publish an empty image to build connect
        std::shared_ptr<image> empty_image = std::make_shared<image>();
        pub.publish(empty_image);

        while (!pub.is_connected())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        long long base_time = std::chrono::duration_cast<std::chrono::microseconds>(time_base.time_since_epoch()).count();
        long long delay = (1. / fps) * 1e6;
        // sleep
        for (size_t i = 0; i < images_msgs.size(); i++)
        {
            auto image = images_msgs[i];
            auto timestamp = std::chrono::duration_cast<std::chrono::microseconds>(clock.now() - time_base).count();
            image->timestamp_base(base_time);
            image->timestamp(timestamp);
            std::cout << timestamp << " publish image: " << image->data().size() << std::endl; 
            pub.publish(image);
            std::this_thread::sleep_for(std::chrono::microseconds(delay));
        }
    }
}

class ReceivedImageSeq
{
public:
    bool process_msg(const std::shared_ptr<image> &image)
    {
        if (image->data().size() > 0)
        {
            std::chrono::system_clock clock;
            std::chrono::system_clock::time_point time_base = clock.now();
            long long now_time = std::chrono::duration_cast<std::chrono::microseconds>(time_base.time_since_epoch()).count();
            long long delay = now_time - image->timestamp_base() - image->timestamp();
            cv::Mat cvimage = toCvImage(image);
            image_list.push_back(cvimage);
            image_cout++;
            // std::cout << ".";
            std::cout << reinterpret_cast<long long>(this) <<  " received image : "<< image_list.size() << " delay time: " << delay << std::endl;
            total_delay += delay;
            return true;
        } else {
            std::cout << image->timestamp() << ": received empty image." << std::endl;
        }
        return false;
    }
    const std::vector<cv::Mat> &get_image_list() const
    {
        return image_list;
    }

private:
    std::vector<cv::Mat> image_list;

    long long total_delay = 0;
    int image_cout = 0;
};

void receive_image_test(const std::string &safe_videopath, int fps)
{

    ReceivedImageSeq imageSeq;
    MessageSubscriber sub;

    std::cout << "receive image sequence." << std::endl;
    sub.set_callback(std::bind(&ReceivedImageSeq::process_msg, &imageSeq, std::placeholders::_1));
    if (sub.init())
    {
        sub.run();
    }
    
    const std::vector<cv::Mat> &images = imageSeq.get_image_list();
    std::cout << "write image sequence: size = " << images.size() << std::endl;
    std::cout << "address imageseq: " << reinterpret_cast<long long>(&imageSeq) << std::endl;
    if (images.size() > 0)
    {
        int width = images[0].cols;
        int height = images[1].rows;
        cv::VideoWriter writer(safe_videopath, cv::VideoWriter::fourcc('F', 'M', 'P', 'G'), fps, cv::Size(width, height));
        for (size_t ii = 0; ii < images.size(); ii++)
        {
            writer << images[ii];
        }
    }
}