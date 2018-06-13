#include "OpenKinect.hpp"

#include <iostream>
#include <cstring>

Freenect::Freenect() : success(false), started(false), context(nullptr), freenect_thread(nullptr), stop_freenect_thread(0) {
    if (freenect_init (&context, NULL) < 0) {
        std::cerr << "Cannot initialize libfreenect library" << std::endl;
    }
    else {
        // select camera for opening
        freenect_select_subdevices (context, static_cast<freenect_device_flags>(FREENECT_DEVICE_CAMERA | FREENECT_DEVICE_MOTOR));
        success = true;
    }
}

Freenect::~Freenect()
{
    if (started) {
        if (freenect_thread) {
            freenect_thread->join();
            delete freenect_thread;
        }
    }
    if (success) {
        if (freenect_shutdown (context) < 0) {
            std::cerr << "Can not shutdown libfreenect" << std::endl;
        }
    }
}

bool Freenect::isInitialised()
{
    return success;
}

void Freenect::initialise()
{
    if (!success) {
        if (freenect_init (&context, NULL) < 0) {
            std::cerr << "Cannot initialize libfreenect library" << std::endl;
        }
        else {
            // select camera for opening
            freenect_select_subdevices (context, static_cast<freenect_device_flags>(FREENECT_DEVICE_CAMERA));
            success = true;
        }
    }
}

void Freenect::start()
{
    freenect_thread = new(std::nothrow) std::thread (freenect_thread_func, this);
    started = true;
}

void Freenect::stop()
{
    stop_freenect_thread = 1;
}

int Freenect::no_of_kinects()
{
    return freenect_num_devices (context);
}

Kinect::Kinect(int _index, freenect_context *context) : success(false), depth_started(false), rgb_started(false), device(nullptr)
{
    if (freenect_open_device (context, &device, _index) < 0) {
        std::cerr << "Cannot open Kinect" << std::endl;
    }
    else {
        freenect_set_user (device, this);
        
        depth = cv::Mat (480, 640, CV_8UC1); // assign depth buffer
        rgb = cv::Mat (480, 640, CV_8UC3);   // assign RGB buffer
        
        rgb_format = FREENECT_VIDEO_RGB;
        rgb_resolution = FREENECT_RESOLUTION_MEDIUM;
        rgb_mode = freenect_find_video_mode (rgb_resolution, rgb_format);
    
        if (!rgb_mode.is_valid) {
            std::cerr << "Cannot find video mode"  << std::endl;
            if (freenect_close_device (device) < 0) std::cerr << "Can not close Kinect" << std::endl;
            return;
        }
    
        if (freenect_set_video_mode (device, rgb_mode) < 0) {
            std::cerr << "Cannot set video format"  << std::endl;
            if (freenect_close_device (device) < 0) std::cerr << "Can not close Kinect" << std::endl;
            return;
        }
    
        depth_format = FREENECT_DEPTH_11BIT;
        depth_resolution = FREENECT_RESOLUTION_MEDIUM;
        depth_mode = freenect_find_depth_mode (depth_resolution, depth_format);
    
        if (!depth_mode.is_valid) {
            std::cerr << "Cannot find depth mode"  << std::endl;
            if (freenect_close_device (device) < 0) std::cerr << "Can not close Kinect" << std::endl;
            return;
        }
    
        if (freenect_set_depth_mode (device, depth_mode) < 0) {
            std::cerr << "Cannot set depth format"  << std::endl;
            if (freenect_close_device (device) < 0) std::cerr << "Can not close Kinect" << std::endl;
            return;
        }
    
        freenect_set_depth_callback (device, freenect_depth_callback);
        freenect_set_video_callback (device, freenect_rgb_callback);
        success = true;
    }
}

Kinect::~Kinect()
{
    if (success) {
        if (freenect_close_device (device) < 0) std::cerr << "Can not close Kinect" << std::endl;
    }
}

bool Kinect::isInitialised()
{
    return success;
}

void Kinect::initialise (int _index, freenect_context *context)
{
    if (!success) {
        if (freenect_open_device (context, &device, _index) < 0) {
            std::cerr << "Cannot open Kinect" << std::endl;
        }
        else {
            freenect_set_user (device, this);

            depth = cv::Mat (480, 640, CV_8UC1); // assign depth buffer
            rgb = cv::Mat (480, 640, CV_8UC3);   // assign RGB buffer
            
            rgb_format = FREENECT_VIDEO_RGB;
            rgb_resolution = FREENECT_RESOLUTION_MEDIUM;
            rgb_mode = freenect_find_video_mode (rgb_resolution, rgb_format);

            if (!rgb_mode.is_valid) {
                std::cerr << "Cannot find video mode"  << std::endl;
                if (freenect_close_device (device) < 0) std::cerr << "Can not close Kinect" << std::endl;
                return;
            }

            if (freenect_set_video_mode (device, rgb_mode) < 0) {
                std::cerr << "Cannot set video format"  << std::endl;
                if (freenect_close_device (device) < 0) std::cerr << "Can not close Kinect" << std::endl;
                return;
            }

            depth_format = FREENECT_DEPTH_MM;
            depth_resolution = FREENECT_RESOLUTION_MEDIUM;
            depth_mode = freenect_find_depth_mode (depth_resolution, depth_format);

            if (!depth_mode.is_valid) {
                std::cerr << "Cannot find depth mode"  << std::endl;
                if (freenect_close_device (device) < 0) std::cerr << "Can not close Kinect" << std::endl;
                return;
            }

            if (freenect_set_depth_mode (device, depth_mode) < 0) {
                std::cerr << "Cannot set depth format"  << std::endl;
                if (freenect_close_device (device) < 0) std::cerr << "Can not close Kinect" << std::endl;
                return;
            }

            freenect_set_depth_callback (device, freenect_depth_callback);
            freenect_set_video_callback (device, freenect_rgb_callback);
            success = true;
        }
    }
}

void Kinect::start()
{
    if (freenect_start_depth (device) < 0) std::cerr << "Cannot start depth callback" << std::endl;
    else depth_started = true;
    if (freenect_start_video (device) < 0) std::cerr << "Cannot start RGB callback" << std::endl;
    else rgb_started = true;
}

void Kinect::stop()
{
    if (depth_started) {
        if (freenect_stop_depth (device) < 0) std::cerr << "Cannot stop depth callback" << std::endl;        
    }
    if (rgb_started) {
        if (freenect_stop_video (device) < 0) std::cerr << "Cannot stop RGB callback" << std::endl;
    }
}

bool Kinect::depthStarted()
{
    return depth_started;
}

bool Kinect::rgbStarted()
{
    return rgb_started;
}

void freenect_thread_func (Freenect *freenect_object)
{
    while (!freenect_object->stop_freenect_thread && freenect_process_events (freenect_object->context) >= 0);
}

void freenect_depth_callback (freenect_device *device, void *pixel, uint32_t timestamp)
{
    Kinect *kinect_object = static_cast<Kinect*>(freenect_get_user (device));
    cv::Mat depth8;
    cv::Mat mydepth (480, 640, CV_16UC1, pixel);

    mydepth.convertTo (depth8, CV_8UC1, 1.0/4.0);
    kinect_object->depth_mutex.lock();
    memcpy (kinect_object->depth.data, depth8.data,  640*480);
    kinect_object->depth_mutex.unlock();
}

void freenect_rgb_callback (freenect_device *device, void *pixel, uint32_t timestamp)
{
    Kinect *kinect_object = static_cast<Kinect*>(freenect_get_user (device));
    kinect_object->rgb_mutex.lock();
    memcpy (kinect_object->rgb.data, pixel, kinect_object->rgb_mode.bytes);
    kinect_object->rgb_mutex.unlock();
}