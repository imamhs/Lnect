/* 
 * File:   VideoArea.hpp
 * Author: imam
 *
 * Created on 3 October 2015, 1:03 AM
 */

#ifndef VIDEOAREA_HPP
#define	VIDEOAREA_HPP

#include <vector>

#include <gtkmm/drawingarea.h>
#include <cairomm/context.h>

#include <opencv2/opencv.hpp>

#include "OpenKinect.hpp"

#include "dep.hpp"

class VideoArea : public Gtk::DrawingArea
{
    private:
        cv::RNG rng;
    protected:
        virtual bool on_draw (const Cairo::RefPtr<Cairo::Context> &cr);
        bool on_timeout ();
    public:
        VideoArea ();
        virtual ~VideoArea();   
};

extern Kinect *kinect_list[MAX_KINECTS];
extern int no_of_kinects;
extern int kinect_device_no;

#endif	/* VIDEOAREA_HPP */

