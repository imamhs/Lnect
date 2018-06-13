#include "VideoArea.hpp"

#include <gdkmm/pixbuf.h>
#include <gdkmm/general.h>
#include <glibmm/main.h>

VideoArea::VideoArea()
{
    Glib::signal_timeout().connect (sigc::mem_fun (*this, &VideoArea::on_timeout), 50);    
}

VideoArea::~VideoArea()
{

}

bool VideoArea::on_timeout()
{
    Glib::RefPtr<Gdk::Window> win = get_window();
    if (win)
    {
        Gdk::Rectangle r (0, 0, get_allocation().get_width(), get_allocation().get_height());
        win->invalidate_rect (r, false);
    }
    return true;
}

bool VideoArea::on_draw (const Cairo::RefPtr<Cairo::Context> &cr)
{

    cv::Mat result;
    
    if (kinect_device_no < 1)
    {
        cv::Mat temp = cv::Mat (480, 640, CV_8UC3);
        for( int y = 0; y < temp.rows; y++ )
        {
            for( int x = 0; x < temp.cols; x++ )
            {
                for( int c = 0; c < 3; c++ ) temp.at<cv::Vec3b>(y,x)[c] = cv::saturate_cast<uchar>(rng.uniform (0, 255));
            }
        }
        cv::copyMakeBorder (temp, result, 10, 10, 20, 20, cv::BORDER_CONSTANT, cv::Scalar (10, 10, 10));
    }
    else
    {
        cv::Mat temp;
        if (kinect_list[kinect_device_no-1]->rgbStarted())
        {
            kinect_list[kinect_device_no-1]->rgb_mutex.lock();
            kinect_list[kinect_device_no-1]->rgb.copyTo (temp);
            kinect_list[kinect_device_no-1]->rgb_mutex.unlock();
        }
        cv::copyMakeBorder (temp, result, 10, 10, 20, 20, cv::BORDER_CONSTANT, cv::Scalar (10, 10, 10));
    }
              
    if (result.empty()) return false;

    Gdk::Cairo::set_source_pixbuf (cr, Gdk::Pixbuf::create_from_data(result.data, Gdk::COLORSPACE_RGB, false, 8, result.cols, result.rows, result.step));
    
    cr->paint();
        
    return true;
}