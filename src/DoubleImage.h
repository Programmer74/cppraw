#ifndef CPPRAW_DOUBLEIMAGE_H
#define CPPRAW_DOUBLEIMAGE_H

#include <stdint.h>
#include <iostream>

class DoubleImage {
public:
    DoubleImage(int _width, int _height);

    int image_width = 0;

    int image_height = 0;

    void set_color(int x, int y, double r, double g, double b);

    void get_color(int x, int y, double *r, double *g, double *b);

    void get_color_adjusted(int x, int y, double *r, double *g, double *b);

    void set_exposure(double value);

    void setForceCenteringImage(bool forceCenteringImage);

    void setForceAutoscalingImage(bool forceAutoscalingImage);

    void setImagePaneOffsetX(int imagePaneOffsetX);

    void setImagePaneOffsetY(int imagePaneOffsetY);

    void setImagePaneScale(double imagePaneScale);

    void setImagePaneMouseX(int imagePaneMouseX);

    void setImagePaneMouseY(int imagePaneMouseY);

    void setImagePaneMouseDownX(int imagePaneMouseDownX);

    void setImagePaneMouseDownY(int imagePaneMouseDownY);

    int getImagePaneMouseDownX() const;

    int getImagePaneMouseDownY() const;

    int getImagePaneOffsetX() const;

    int getImagePaneOffsetY() const;

    double getImagePaneScale() const;

    void paintOnBuf(uint8_t *static_image_buf, int pane_width, int pane_height);

private:
    int image_pane_offset_x = 0;
    int image_pane_offset_y = 0;
    double image_pane_scale = 0;
    bool autoscale_performed = false;
    bool force_centering_image = true;
    bool force_autoscaling_image = true;

    int image_pane_mouse_x = -1;
    int image_pane_mouse_y = -1;

    int image_pane_mouse_down_x = -1;
    int image_pane_mouse_down_y = -1;

};


#endif //CPPRAW_DOUBLEIMAGE_H
