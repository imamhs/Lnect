/* 
 * File:   OpenKinect.hpp
 * Author: imam
 *
 * Created on 13 October 2015, 10:49 PM
 */

/*
 * libfreenect class from the scratch to use multiple Kinect devices in OpenCV
 * 
 */

#ifndef OPENKINECT_HPP
#define	OPENKINECT_HPP

#include <thread>
#include <mutex>

#include <opencv2/opencv.hpp>

#include <libfreenect.h>

/* Freenect class for libfreenect library
 * you just need one class object for all Kinect devices
 * once create it is initialised
 * after starting one or more Kinect objects process their request by start member function
 * when all Kinect objects are stoped, you can stop process thread by stop member function
 * 
 * typical application procedure as follows:
 * 
 * - create one Freenect object in your application
 * - make sure Freenect object is initialised
 * - create one or more kinect objects and initialise them
 * - start Kinect objects
 * - start Freenect object
 * - stop Kinect objects
 * - stop Freenect object
 */

class Freenect {
    bool success;
    bool started;
    std::thread *freenect_thread;
public:
    
    freenect_context *context;              // libfreenect context for all Kinect devices
    
    volatile int stop_freenect_thread;
    
    Freenect ();
    ~Freenect ();
    bool isInitialised ();
    void initialise ();
    void start ();
    void stop ();
    int no_of_kinects ();
};

class Kinect {
    bool success;
    bool depth_started;
    bool rgb_started;
public:
    
    freenect_device *device;                // device context
    freenect_frame_mode rgb_mode;
    freenect_video_format rgb_format;
    freenect_resolution rgb_resolution;
    freenect_frame_mode depth_mode;
    freenect_depth_format depth_format;
    freenect_resolution depth_resolution;
    
    cv::Mat depth;                          // depth camera OpenCV image container
    cv::Mat rgb;                            // RGB camera OpenCV image container
    std::mutex depth_mutex;
    std::mutex rgb_mutex;
    
    Kinect (int _index, freenect_context *context);
    ~Kinect ();
    bool isInitialised ();
    void initialise (int _index, freenect_context *context);
    bool depthStarted ();
    bool rgbStarted ();
    void start ();
    void stop ();
};

extern void freenect_thread_func (Freenect *freenect_object);
extern void freenect_depth_callback (freenect_device *device, void *pixel, uint32_t timestamp);
extern void freenect_rgb_callback (freenect_device *device, void *pixel, uint32_t timestamp);

#endif	/* OPENKINECT_HPP */

