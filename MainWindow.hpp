/* 
 * File:   MainWindow.hpp
 * Author: imam
 *
 * Created on 25 September 2015, 7:52 PM
 */

#ifndef MAINWINDOW_HPP
#define	MAINWINDOW_HPP

// Gtkmm includes
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/button.h>

#include "VideoArea.hpp"
#include "DepthArea.hpp"

#include "dep.hpp"

class MainWindow : public Gtk::Window
{
    protected:
        
        Gtk::Box control_box, main_box, video_area_box;
        Gtk::Frame video_frame;
        VideoArea video_area;
        DepthArea depth_area;
        Gtk::Label number_of_devices_caption_label, number_of_devices_label, select_kinect_caption;
        Gtk::ComboBoxText kinect_list_combo;
        Gtk::Button tilt_up_button, tilt_down_button, level_button;
        
        void on_kinect_list_combo_changed();
        void on_tilt_up_button_clicked();
        void on_level_button_clicked();
        void on_tilt_down_button_clicked();
    public:
        MainWindow ();
        virtual ~MainWindow();
};

extern Kinect *kinect_list[MAX_KINECTS];
extern int tilt_angle[MAX_KINECTS];
extern int kinect_device_no;
extern int no_of_kinects;

#endif	/* MAINWINDOW_HPP */

