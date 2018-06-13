#include "DepthArea.hpp"

#include <gdkmm/pixbuf.h>
#include <gdkmm/general.h>
#include <glibmm/main.h>

DepthArea::DepthArea()
{
    Glib::signal_timeout().connect (sigc::mem_fun (*this, &DepthArea::on_timeout), 50);    
}

DepthArea::~DepthArea()
{

}

bool DepthArea::on_timeout()
{
    Glib::RefPtr<Gdk::Window> win = get_window();
    if (win)
    {
        Gdk::Rectangle r (0, 0, get_allocation().get_width(), get_allocation().get_height());
        win->invalidate_rect (r, false);
    }
    return true;
}

bool DepthArea::on_draw (const Cairo::RefPtr<Cairo::Context> &cr)
{

    cv::Mat cv_frame, cv_frame1, result;
    
    if (kinect_device_no < 1)
    {
        cv::Mat temp = cv::Mat (480, 640, CV_8UC1);
        for( int y = 0; y < temp.rows; y++ )
        {
            for( int x = 0; x < temp.cols; x++ )
            {
                temp.at<uchar>(y,x) = cv::saturate_cast<uchar>(rng.uniform (0, 255));
            }
        }
        cv::copyMakeBorder (temp, cv_frame, 10, 10, 20, 20, cv::BORDER_CONSTANT, cv::Scalar (10, 10, 10));
    }
    else
    {
        cv::Mat temp;
        if (kinect_list[kinect_device_no-1]->depthStarted())
        {
            kinect_list[kinect_device_no-1]->depth_mutex.lock();
            kinect_list[kinect_device_no-1]->depth.copyTo (temp);
            kinect_list[kinect_device_no-1]->depth_mutex.unlock();
        }
        
        cv::copyMakeBorder (temp, cv_frame, 10, 10, 20, 20, cv::BORDER_CONSTANT, cv::Scalar (10, 10, 10));
    }
    
    if (cv_frame.empty()) return false;
    
    cv::resize (cv_frame, cv_frame1, cv::Size (0, 0), 0.5, 0.5);
    
    cv::cvtColor (cv_frame1, result, CV_GRAY2RGB);
    
    if (result.empty()) return false;
        
    Gdk::Cairo::set_source_pixbuf (cr, Gdk::Pixbuf::create_from_data (result.data, Gdk::COLORSPACE_RGB, false, 8, result.cols, result.rows, result.step));
    
    cr->paint();
        
    return true;
}
