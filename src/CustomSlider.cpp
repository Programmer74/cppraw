#include <iostream>
#include "CustomSlider.h"

const int marginLeft = 5;
const int marginRight = 5;

CustomSlider::~CustomSlider() {

}

double CustomSlider::getValue() {
    return value;
}

int CustomSlider::getDraggerX(double value, int w) {
    return marginLeft + (value - from) / (to - from) * (w - marginLeft - marginRight);
}

inline double CustomSlider::getValue(int draggerX, int w) {
    return getValue(draggerX, w, from, to);
}

inline double CustomSlider::getValue(int draggerX, int w, double cfrom, double cto) {
    return (draggerX - marginLeft) * (cto - cfrom) / (w - marginLeft - marginRight) + cfrom;
}

void CustomSlider::draw(int x, int y, int w, int h, const Cairo::RefPtr<Cairo::Context> &cr) {
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

    //draw gradiented line
    int cy = y + h / 2 - 1;
    for (int cx = marginLeft; cx < w - marginLeft - marginRight; cx += 2) {
        cr->set_source_rgb(
                getValue(cx, w, rlA, rrA),
                getValue(cx, w, glA, grA),
                getValue(cx, w, blA, brA)
        );
        cr->rectangle(cx, cy, 2, 2);
        cr->fill();
        cx += 2;
        cr->set_source_rgb(
                getValue(cx, w, rlB, rrB),
                getValue(cx, w, glB, grB),
                getValue(cx, w, blB, brB)
        );
        cr->rectangle(cx, cy, 2, 2);
        cr->fill();
    }

    //draw normal line
    //cr->move_to(marginLeft, y + h / 2);
    //cr->line_to(w - marginRight, y + h / 2);

    cr->set_source_rgb(0, 0, 0);
    cr->set_line_width(1.0);
    cr->move_to(marginLeft, y + h / 2 - 4);
    cr->line_to(marginLeft, y + h / 2 + 4);

    cr->move_to(w - marginRight, y + h / 2 - 4);
    cr->line_to(w - marginRight, y + h / 2 + 4);

    cr->stroke();

    int valX = getDraggerX(value, w);
    if (mouseMovedOverDragger) {
        cr->set_source_rgb(1, 0, 0);
    } else {
        cr->set_source_rgb(0, 0, 0);
    }
    cr->rectangle(valX - 2, y + h / 2 - 2, 4, 4);
    cr->fill();
}

void CustomSlider::doubleClick(int x, int y, int w, int h) {
    setValue(defaultValue);
}

void CustomSlider::mouseDown(int x, int y, int w, int h) {
    mouseDownX = x;
    mouseDownY = y;
    clickedValueDragger = ((abs(getDraggerX(value, w) - x) < 10) &&
            (abs(h / 2 - y) < 10));
}

void CustomSlider::mouseDragged(int x, int y, int w, int h) {
    int deltaX = x - mouseDownX;
    int deltaY = y - mouseDownY;

    if (clickedValueDragger) {
        setValue(getValue(x, w));
    } else {
        setValue(value + deltaX * (to - from) / 700);
    }

    mouseDownX = x;
    mouseDownY = y;
}

void CustomSlider::mouseMoved(int x, int y, int w, int h) {
    mouseMovedOverDragger = ((abs(getDraggerX(value, w) - x) < 10) &&
                           (abs(h / 2 - y) < 10));
}

void CustomSlider::setValue(double newvalue) {
    value = newvalue;
    if (value < from) value = from;
    if (value > to) value = to;
    value_changed(value);
}

void CustomSlider::mouseScrolled(int direction) {
    setValue(value + (direction) * (to - from) / 2000);
}
