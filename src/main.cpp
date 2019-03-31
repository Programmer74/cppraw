#include <gtkmm.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdint.h>
#include <vector>
#include "DoubleImage.h"
#include "DoubleImageLoader.h"
#include "CustomSlider.h"

Gtk::ApplicationWindow *pWindow = nullptr;

Gtk::DrawingArea *gImage = nullptr;
Glib::RefPtr<Gdk::Pixbuf> pixbuf;
int old_gimage_width = -1;
int old_gimage_height = -1;

Gtk::DrawingArea *gCustomSlidersPane = nullptr;
Gtk::CheckButton *chbAutoCenter = nullptr;
Gtk::ScaleButton *sldExposure = nullptr;
DoubleImage *doubleImage = nullptr;

uint8_t *static_image_buf_for_preview = new uint8_t[1920 * 1080 * 3];

std::vector<CustomSlider> sliders;
int slider_height = 40;
int mouse_down_slider_index = -1;

bool on_sliderspane_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
    Gtk::Allocation allocation = gCustomSlidersPane->get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();

    // coordinates for the center of the window
    int xc, yc;
    xc = width / 2;
    yc = height / 2;

    cr->set_line_width(10.0);

    // draw red lines out from the center of the window
    cr->set_source_rgb(0.8, 0.0, 0.0);
    cr->move_to(0, 0);
    cr->line_to(xc, yc);
    cr->line_to(0, height);
    cr->move_to(xc, yc);
    cr->line_to(width, yc);
    cr->stroke();

    int y = 0;
    int h = slider_height;
    for (CustomSlider slider : sliders) {
        slider.draw(0, y, width, h, cr);
        y += h;
    }

    return true;
}

bool on_sliderspane_press_event(GdkEventButton *e) {
    int index = (int)(e->y) / slider_height;
    if (index < sliders.size()) {
        mouse_down_slider_index = index;
        Gtk::Allocation allocation = gCustomSlidersPane->get_allocation();
        const int width = allocation.get_width();
        sliders[index].mouseDown(e->x, (int)(e->y) % slider_height, width, slider_height);
    }
    gCustomSlidersPane->queue_draw();
    return true;
}

bool on_sliderspane_release_event(GdkEventButton *e) {
    mouse_down_slider_index = -1;
    return true;
}

bool on_sliderpane_motion(GdkEventMotion *e) {
    int index = (int)(e->y) / slider_height;
    if (index < sliders.size()) {
        if (index != mouse_down_slider_index) {
            mouse_down_slider_index = -1;
        }
        Gtk::Allocation allocation = gCustomSlidersPane->get_allocation();
        const int width = allocation.get_width();
        if (mouse_down_slider_index != -1) {
            sliders[index].mouseDragged(e->x, (int)(e->y) % slider_height, width, slider_height);
        } else {
            sliders[index].mouseMoved(e->x, (int)(e->y) % slider_height, width, slider_height);
        }
    }
    gCustomSlidersPane->queue_draw();
    return true;
}

bool on_sliderpane_scroll(GdkEventScroll *e) {
    int index = (int)(e->y) / slider_height;
    if (index < sliders.size()) {
        sliders[index].mouseScrolled(e->direction);
    }
    gCustomSlidersPane->queue_draw();
    return true;
}

bool on_imagepane_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
    Gtk::Allocation allocation = gImage->get_allocation();
    const int pane_width = allocation.get_width();
    const int pane_height = allocation.get_height();

    doubleImage->paintOnBuf(static_image_buf_for_preview, pane_width, pane_height);

    if ((old_gimage_width != pane_width) || (old_gimage_height != pane_height)) {
        pixbuf = Gdk::Pixbuf::create_from_data(static_image_buf_for_preview,
                                               Gdk::COLORSPACE_RGB, false, 8, pane_width,
                                               pane_height,
                                               pane_width * 3);
    }

    Gdk::Cairo::set_source_pixbuf(cr, pixbuf, 0, 0);
    cr->rectangle(0, 0, pixbuf->get_width(), pixbuf->get_height());
    cr->fill();

    return true;
}

bool on_imagepane_scroll(GdkEventScroll *e) {
    doubleImage->setForceAutoscalingImage(false);
    double image_pane_scale = doubleImage->getImagePaneScale();
    if (e->direction == GDK_SCROLL_UP) {
        image_pane_scale += 0.1;
        if (image_pane_scale > 10) image_pane_scale = 10;
    } else if (e->direction == GDK_SCROLL_DOWN) {
        image_pane_scale -= 0.1;
        if (image_pane_scale < 0.5) image_pane_scale = 0.5;
    }
    doubleImage->setImagePaneScale(image_pane_scale);
    gImage->queue_draw();
    std::cout << "Scale: " << image_pane_scale << std::endl;

    return true;
}

bool on_imagepane_motion(GdkEventMotion *e) {
    double image_pane_mouse_x = e->x;
    double image_pane_mouse_y = e->y;

    int ox = (doubleImage->getImagePaneMouseDownX() - image_pane_mouse_x);
    int oy = (doubleImage->getImagePaneMouseDownY() - image_pane_mouse_y);

    doubleImage->setImagePaneOffsetX(doubleImage->getImagePaneOffsetX() - ox);
    doubleImage->setImagePaneOffsetY(doubleImage->getImagePaneOffsetY() - oy);

    doubleImage->setImagePaneMouseDownX(image_pane_mouse_x);
    doubleImage->setImagePaneMouseDownY(image_pane_mouse_y);

    //std::cout << "offsets " << image_pane_offset_x << " " << image_pane_offset_y << std::endl;

    doubleImage->setImagePaneMouseX(image_pane_mouse_x);
    doubleImage->setImagePaneMouseY(image_pane_mouse_y);

    gImage->queue_draw();
    return true;
}

bool on_imagepane_press_event(GdkEventButton *e) {
    if ((e->type == GDK_BUTTON_PRESS) && (e->button == 1)) {
        doubleImage->setImagePaneMouseDownX(e->x);
        doubleImage->setImagePaneMouseDownY(e->y);
    }
    return true;
}

void on_autocenter_pressed() {
    std::cout << "Autocenter clicked" << std::endl;
    doubleImage->setForceCenteringImage(chbAutoCenter->get_active());
    doubleImage->setForceAutoscalingImage(chbAutoCenter->get_active());
    gImage->queue_draw();
}

void on_slider_changed(double val) {
    double exposureStop = (val - 50.0) / 25.0;
    std::cout << "exposureStop is " << exposureStop << std::endl;
    doubleImage->set_exposure(exposureStop);
    gImage->queue_draw();
}

int main(int argc, char **argv) {

    doubleImage = DoubleImageLoader().load_image();

    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

    auto refBuilder = Gtk::Builder::create();
    try {
        refBuilder->add_from_file("../../test.glade");
    }
    catch (const Glib::FileError &ex) {
        std::cerr << "FileError: " << ex.what() << std::endl;
        return 1;
    }
    catch (const Glib::MarkupError &ex) {
        std::cerr << "MarkupError: " << ex.what() << std::endl;
        return 1;
    }
    catch (const Gtk::BuilderError &ex) {
        std::cerr << "BuilderError: " << ex.what() << std::endl;
        return 1;
    }

    refBuilder->get_widget("applicationwindow1", pWindow);
    if (pWindow) {
        //Get the GtkBuilder-instantiated Button, and connect a signal handler:

        refBuilder->get_widget("gImage", gImage);
        if (gImage) {
            std::cout << "lel" << std::endl;
            gImage->signal_draw().connect(sigc::ptr_fun(on_imagepane_draw));
            gImage->add_events(Gdk::SCROLL_MASK | Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON_PRESS_MASK |
                               Gdk::POINTER_MOTION_HINT_MASK);
            gImage->signal_scroll_event().connect(sigc::ptr_fun(on_imagepane_scroll));
            gImage->signal_motion_notify_event().connect(sigc::ptr_fun(on_imagepane_motion));
            gImage->signal_button_press_event().connect(sigc::ptr_fun(on_imagepane_press_event));
        }

        refBuilder->get_widget("chbAutoCenter", chbAutoCenter);
        if (chbAutoCenter) {
            chbAutoCenter->signal_toggled().connect(sigc::ptr_fun(on_autocenter_pressed));
        }

        refBuilder->get_widget("sldExposure", sldExposure);
        sldExposure->signal_value_changed().connect(sigc::ptr_fun(on_slider_changed));

        sliders.emplace_back(CustomSlider("Exposure", -2.0, 2.0, 0.0, [&](double val) {
            doubleImage->set_exposure(val);
            gImage->queue_draw();
        }));

        sliders.emplace_back(CustomSlider("Brightness", -1.0, 1.0, 0.0, [&](double val) {
            doubleImage->set_brightness(val);
            gImage->queue_draw();
        }));

        sliders.emplace_back(CustomSlider("Contrast", 0.0, 2.0, 1.0, [&](double val) {
            doubleImage->set_contrast(val);
            gImage->queue_draw();
        }));

        refBuilder->get_widget("gCustomSlidersPane", gCustomSlidersPane);
        if (gCustomSlidersPane) {
            std::cout << "kex" << std::endl;

            gCustomSlidersPane->add_events(Gdk::ALL_EVENTS_MASK);

            gCustomSlidersPane->signal_draw().connect(sigc::ptr_fun(on_sliderspane_draw));
            gCustomSlidersPane->signal_scroll_event().connect(sigc::ptr_fun(on_sliderpane_scroll));
            gCustomSlidersPane->signal_button_press_event().connect(sigc::ptr_fun(on_sliderspane_press_event));
            gCustomSlidersPane->signal_button_release_event().connect(sigc::ptr_fun(on_sliderspane_release_event));
            gCustomSlidersPane->signal_motion_notify_event().connect(sigc::ptr_fun(on_sliderpane_motion));
        }
        app->run(*pWindow);
    }

    delete pWindow;

    return 0;
}
