#ifndef CPPRAW_CUSTOMSLIDER_H
#define CPPRAW_CUSTOMSLIDER_H

#include <string>
#include <cairomm/context.h>
#include <functional>

class CustomSlider {

public:
    CustomSlider(std::string caption, double from, double to, double value, std::function<void(double)> value_changed) :
            caption(caption), from(from), to(to), value(value), value_changed(value_changed) {}

    virtual ~CustomSlider();

    double getValue();

    void setValue(double newvalue);

    void draw(int x, int y, int w, int h, const Cairo::RefPtr<Cairo::Context> &cr);

    void mouseDown(int x, int y);

    void mouseDragged(int x, int y);

    void mouseMoved(int x, int y);

    void mouseScrolled(int direction);

private:
    double from, to, value;
    std::string caption;
    std::function<void(double)> value_changed;

    int getDraggerX(double value, int w);

    double getValue(int draggerX);
};


#endif //CPPRAW_CUSTOMSLIDER_H
