// C++ includes
#include <iostream>

// GTKmm (User interface) includes
#include <gtkmm/application.h>

// Main Window class include
#include "MainWindow.hpp"

// libfreenect (Kinect device) include 
#include "OpenKinect.hpp"

#include "dep.hpp"

int kinect_device_no = 0;           // for selecting a kinect for capturing
int no_of_kinects = 0;              // total number of kinects found
Freenect *freenect = nullptr;

Kinect *kinect_list[MAX_KINECTS];
int tilt_angle[MAX_KINECTS];

int main (int argc, char** argv)
{
    freenect = new(std::nothrow) Freenect();
    
    if (freenect)
    {
        if (freenect->isInitialised())
        {
            no_of_kinects = freenect->no_of_kinects();
            if (no_of_kinects > 0) {
                for (int c = 0; c < no_of_kinects; c++)
                {
                    tilt_angle[c] = 0;
                    kinect_list[c] = nullptr;
                    kinect_list[c] = new(std::nothrow) Kinect (c, freenect->context);
                }
            }
        }    
    }
    
    for (int c = 0; kinect_list[c] != nullptr; c++) {
        if (kinect_list[c]->isInitialised()) kinect_list[c]->start();
    }
    
    if (no_of_kinects > 0)
    {
        for (int c = 0; c < no_of_kinects; c++)
        {
            if (kinect_list[c]->rgbStarted() || kinect_list[c]->depthStarted())
            {
                freenect->start();
                break;
            }
        }
    }
    
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create (argc, argv);
    
    MainWindow mwindow;
    
    app->run (mwindow);
        
    for (int c = 0; c < no_of_kinects; c++) {
        if (kinect_list[c]->isInitialised()) kinect_list[c]->stop();
        delete kinect_list[c];
    }
    
    if (freenect) {
        if (freenect->isInitialised()) {
            freenect->stop();
        }
        delete freenect;
    }
    
    return 0;
}

