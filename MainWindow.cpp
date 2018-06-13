#include "MainWindow.hpp"

#include <iostream>

#include <cstdio>
#include <cstdlib>

#include <libfreenect.h>

MainWindow::MainWindow()
{
    set_title ("Lnect");
    set_size_request (1300, 600);
    set_border_width (10);
    
    video_frame.set_label ("Kinect Capture");
    video_frame.set_label_align (Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER);
    video_frame.set_shadow_type (Gtk::SHADOW_OUT);
    video_frame.set_size_request (1100, 500);
    video_frame.set_border_width (5);

    video_area.set_tooltip_text ("RGB Video");
    video_area.set_size_request (660, 480);
    depth_area.set_tooltip_text ("Depth Video");
    depth_area.set_size_request (340, 250);
    
    video_area_box.set_orientation (Gtk::ORIENTATION_HORIZONTAL);
    video_area_box.set_homogeneous (false);
    video_area_box.set_border_width (10);
    video_area_box.pack_start (video_area, true, true, 5);
    video_area_box.pack_end (depth_area, true, true, 5);
    
    video_frame.add (video_area_box);
    
    main_box.set_orientation (Gtk::ORIENTATION_HORIZONTAL);
    main_box.set_homogeneous (false);
    main_box.pack_start (video_frame, true, true, 10);
    
    number_of_devices_caption_label.set_text ("No of Kinects: ");
    select_kinect_caption.set_text ("Select Kinect: ");
    
    tilt_up_button.set_label ("Tilt up");
    tilt_up_button.signal_clicked().connect (sigc::mem_fun (*this, &MainWindow::on_tilt_up_button_clicked));
    level_button.set_label ("Level");
    level_button.signal_clicked().connect (sigc::mem_fun (*this, &MainWindow::on_level_button_clicked));
    tilt_down_button.set_label ("Tilt down");
    tilt_down_button.signal_clicked().connect (sigc::mem_fun (*this, &MainWindow::on_tilt_down_button_clicked));
    
    if (no_of_kinects > 0) {
        char buff1[12];
        sprintf (buff1, "%d", no_of_kinects);
        number_of_devices_label.set_text (buff1);
        kinect_device_no = 1;
        
        for (int c = 0; c < no_of_kinects; c++) {
            char buff2[12];
            sprintf (buff2, "%d", c+1);
            kinect_list_combo.append (buff2);
            
            if (freenect_set_led (kinect_list[c]->device, LED_GREEN) != 0) std::cerr << "Cannot set led" << std::endl;
            tilt_angle[c] = 0;
            if (freenect_set_tilt_degs (kinect_list[c]->device, tilt_angle[c]) != 0) std::cerr << "Cannot change tilt" << std::endl;
        }
        kinect_list_combo.set_active (0);
        kinect_list_combo.signal_changed().connect (sigc::mem_fun (*this, &MainWindow::on_kinect_list_combo_changed));  
    }
    else {
        number_of_devices_label.set_text ("0");
        kinect_list_combo.set_sensitive (false);
        tilt_up_button.set_sensitive (false);
        level_button.set_sensitive (false);
        tilt_down_button.set_sensitive (false);
    }
    
    control_box.set_orientation (Gtk::ORIENTATION_VERTICAL);
    control_box.set_homogeneous (false);
    control_box.pack_start (number_of_devices_caption_label, false, false, 2);
    control_box.pack_start (number_of_devices_label, false, false, 2);
    control_box.pack_start (select_kinect_caption, false, false, 2);
    control_box.pack_start (kinect_list_combo, false, false, 2);
    control_box.pack_start (tilt_up_button, false, false, 2);
    control_box.pack_start (level_button, false, false, 2);
    control_box.pack_start (tilt_down_button, false, false, 2);
    
    main_box.pack_end (control_box, true, true, 10);
    
    add (main_box);
    
    show_all();
}

MainWindow::~MainWindow()
{
    
}

void MainWindow::on_kinect_list_combo_changed()
{
  Glib::ustring text = kinect_list_combo.get_active_text();
  if(!(text.empty())) {
      kinect_device_no = atoi (text.c_str());
      for (int c = 0; c < no_of_kinects; c++) {
          if (c != (kinect_device_no-1)) if (freenect_set_led (kinect_list[c]->device, LED_BLINK_GREEN) != 0) std::cerr << "Cannot set led" << std::endl;
      }
      if (freenect_set_led (kinect_list[kinect_device_no-1]->device, LED_GREEN) != 0) std::cerr << "Cannot set led" << std::endl;
  }
}

void MainWindow::on_tilt_up_button_clicked()
{
    if (tilt_angle[kinect_device_no-1] < 30) {
        tilt_angle[kinect_device_no-1] += 5;
        if (freenect_set_tilt_degs (kinect_list[kinect_device_no-1]->device, tilt_angle[kinect_device_no-1]) != 0) std::cerr << "Cannot change tilt" << std::endl;
    }
}

void MainWindow::on_level_button_clicked()
{
    tilt_angle[kinect_device_no-1] = 0;
    if (freenect_set_tilt_degs (kinect_list[kinect_device_no-1]->device, tilt_angle[kinect_device_no-1]) != 0) std::cerr << "Cannot change tilt" << std::endl;
}

void MainWindow::on_tilt_down_button_clicked()
{
    if (tilt_angle[kinect_device_no-1] > -30) {
        tilt_angle[kinect_device_no-1] -= 5;
        if (freenect_set_tilt_degs (kinect_list[kinect_device_no-1]->device, tilt_angle[kinect_device_no-1]) != 0) std::cerr << "Cannot change tilt" << std::endl;
    }
}