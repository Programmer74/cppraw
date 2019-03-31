#include <iostream>
#include "CustomSlider.h"

CustomSlider::~CustomSlider() {

}

double CustomSlider::getValue() {
    return value;
}

int marginLeft = 5;
int marginRight = 5;

int last_width = 0;
int last_height = 0;

bool mouse_moved_over_dragger = false;

int CustomSlider::getDraggerX(double value, int w) {
    return marginLeft + (value - from) / (to - from) * (w - marginLeft - marginRight);
}

double CustomSlider::getValue(int draggerX) {
    return (draggerX - marginLeft) * (to - from) / (last_width - marginLeft - marginRight) + from;
}

void CustomSlider::draw(int x, int y, int w, int h, const Cairo::RefPtr<Cairo::Context> &cr) {
    last_width = w;
    last_height = h;

    cr->set_source_rgb(1, 1, 1);
    cr->rectangle(x, y, w, h);
    cr->fill();

    cr->set_source_rgb(0, 0, 0);
    cr->move_to(marginLeft, y + h / 4);
    cr->text_path(this->caption);

    cr->move_to(w - marginRight - 45, y + h / 4);
    cr->text_path(std::to_string(value));

    cr->move_to(marginLeft, y + h * 3 / 4);
    cr->text_path(std::to_string(from));

    cr->move_to(w - marginRight - 45, y + h * 3 / 4);
    cr->text_path(std::to_string(to));
    cr->fill();

    cr->set_line_width(1.0);
    cr->move_to(marginLeft, y + h / 2);
    cr->line_to(w - marginRight, h / 2);

    cr->move_to(marginLeft, y + h / 2 - 4);
    cr->line_to(marginLeft, y + h / 2 + 4);

    cr->move_to(w - marginRight, y + h / 2 - 4);
    cr->line_to(w - marginRight, y + h / 2 + 4);

    cr->stroke();

    int valX = getDraggerX(value, w);
    if (mouse_moved_over_dragger) {
        cr->set_source_rgb(1, 0, 0);
    } else {
        cr->set_source_rgb(0, 0, 0);
    }
    cr->rectangle(valX - 2, h / 2 - 2, 4, 4);
    cr->fill();
}

int mouseDownX = 0;
int mouseDownY = 0;
bool clickedValueDragger = false;
void CustomSlider::mouseDown(int x, int y) {
    mouseDownX = x;
    mouseDownY = y;
    clickedValueDragger = ((abs(getDraggerX(value, last_width) - x) < 10) &&
            (abs(last_height / 2 - y) < 10));
}

void CustomSlider::mouseDragged(int x, int y) {
    int deltaX = x - mouseDownX;
    int deltaY = y - mouseDownY;

    if (clickedValueDragger) {
        setValue(getValue(x));
    } else {
        setValue(value + deltaX * (to - from) / 700);
    }

    mouseDownX = x;
    mouseDownY = y;
}

void CustomSlider::mouseMoved(int x, int y) {
    mouse_moved_over_dragger = ((abs(getDraggerX(value, last_width) - x) < 10) &&
                           (abs(last_height / 2 - y) < 10));
}

void CustomSlider::setValue(double newvalue) {
    value = newvalue;
    if (value < from) value = from;
    if (value > to) value = to;
    value_changed(value);
}

void CustomSlider::mouseScrolled(int direction) {
    setValue(value + (1 - 2 * direction) * (to - from) / 2000);
}
