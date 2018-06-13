/* 
 * File:   DepthArea.hpp
 * Author: imam
 *
 * Created on 14 October 2015, 11:55 PM
 */

#ifndef DEPTHAREA_HPP
#define	DEPTHAREA_HPP

#include <vector>

#include <gtkmm/drawingarea.h>
#include <cairomm/context.h>

#include <opencv2/opencv.hpp>

#include "OpenKinect.hpp"

#include "dep.hpp"

class DepthArea : public Gtk::DrawingArea
{
    private:
        cv::RNG rng;
    protected:
        virtual bool on_draw (const Cairo::RefPtr<Cairo::Context> &cr);
        bool on_timeout ();
    public:
        DepthArea ();
        virtual ~DepthArea();   
};

extern Kinect *kinect_list[MAX_KINECTS];
extern int no_of_kinects;
extern int kinect_device_no;

#endif	/* DEPTHAREA_HPP */

