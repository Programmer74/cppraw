#include <cmath>
#include <vector>
#include "DoubleImage.h"

typedef struct {
    double r;
    double g;
    double b;
} pixel_t;

pixel_t *image;

double exposureStop = 0;

void DoubleImage::set_color(int x, int y, double r, double g, double b) {
    image[x * image_height + y].r = r;
    image[x * image_height + y].g = g;
    image[x * image_height + y].b = b;
}

void DoubleImage::get_color(int x, int y, double *r, double *g, double *b) {
    *r = image[x * image_height + y].r;
    *g = image[x * image_height + y].g;
    *b = image[x * image_height + y].b;
}

double adjust_value(double* original) {
    *original = *original * std::pow(2, exposureStop);
}

void DoubleImage::get_color_adjusted(int x, int y, double *r, double *g, double *b) {
    get_color(x, y, r, g, b);
    adjust_value(r);
    adjust_value(g);
    adjust_value(b);
}

DoubleImage::DoubleImage(int _width, int _height) {
    image_width = _width;
    image_height = _height;
    image = (pixel_t *) std::malloc(image_width * image_height * 3 * sizeof(pixel_t));
}

void DoubleImage::set_exposure(double value) {
    exposureStop = value;
}

void DoubleImage::setForceCenteringImage(bool forceCenteringImage) {
    force_centering_image = forceCenteringImage;
}

void DoubleImage::setForceAutoscalingImage(bool forceAutoscalingImage) {
    force_autoscaling_image = forceAutoscalingImage;
}

void DoubleImage::setImagePaneOffsetX(int imagePaneOffsetX) {
    image_pane_offset_x = imagePaneOffsetX;
}

void DoubleImage::setImagePaneOffsetY(int imagePaneOffsetY) {
    image_pane_offset_y = imagePaneOffsetY;
}

void DoubleImage::setImagePaneScale(double imagePaneScale) {
    image_pane_scale = imagePaneScale;
}

void DoubleImage::setImagePaneMouseX(int imagePaneMouseX) {
    image_pane_mouse_x = imagePaneMouseX;
}

void DoubleImage::setImagePaneMouseY(int imagePaneMouseY) {
    image_pane_mouse_y = imagePaneMouseY;
}

void DoubleImage::setImagePaneMouseDownX(int imagePaneMouseDownX) {
    image_pane_mouse_down_x = imagePaneMouseDownX;
}

void DoubleImage::setImagePaneMouseDownY(int imagePaneMouseDownY) {
    image_pane_mouse_down_y = imagePaneMouseDownY;
}

int DoubleImage::getImagePaneMouseDownX() const {
    return image_pane_mouse_down_x;
}

int DoubleImage::getImagePaneMouseDownY() const {
    return image_pane_mouse_down_y;
}

int DoubleImage::getImagePaneOffsetX() const {
    return image_pane_offset_x;
}

int DoubleImage::getImagePaneOffsetY() const {
    return image_pane_offset_y;
}

double DoubleImage::getImagePaneScale() const {
    return image_pane_scale;
}

uint8_t get_safe_val(double original) {
    int val = original * 255.0;
    if (val > 255) val = 255;
    if (val < 0) val = 0;
    return val;
}

void DoubleImage::paintOnBuf(uint8_t* static_image_buf, int pane_width, int pane_height) {
    if (!autoscale_performed || force_autoscaling_image) {
        double kx = image_width * 1.0 / pane_width;
        double ky = image_height * 1.0 / pane_height;

        image_pane_scale = std::max(kx, ky);

        autoscale_performed = true;
    }
    if (force_centering_image) {
        int nw = image_width / image_pane_scale;
        int nh = image_height / image_pane_scale;

        image_pane_offset_x = (pane_width - nw) / 2;
        image_pane_offset_y = (pane_height - nh) / 2;

    }

    for (int x = 0; x < pane_width; x++) {
        for (int y = 0; y < pane_height; y++) {
            double r, g, b;
            int sx = (x - image_pane_offset_x) * image_pane_scale;
            int sy = (y - image_pane_offset_y) * image_pane_scale;
            if ((sx < 0) || (sy < 0) || (sx >= image_width) || (sy >= image_height)) {
                r = 0;
                g = 0;
                b = 0;
            } else {
                get_color_adjusted(sx, sy, &r, &g, &b);
            }
            static_image_buf[y * pane_width * 3 + x * 3] = get_safe_val(r);
            static_image_buf[y * pane_width * 3 + x * 3 + 1] = get_safe_val(g);
            static_image_buf[y * pane_width * 3 + x * 3 + 2] = get_safe_val(b);
        }
    }

}
