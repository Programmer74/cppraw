#ifndef CPPRAW_CUSTOMSLIDER_H
#define CPPRAW_CUSTOMSLIDER_H

#include <string>
#include <cairomm/context.h>
#include <functional>

class CustomSlider {

public:
    CustomSlider(std::string caption, double from, double to, double value, std::function<void(double)> value_changed) :
            caption(caption), from(from), to(to), value(value), value_changed(value_changed),
            mouseMovedOverDragger(false), mouseDownX(0), mouseDownY(0),
            clickedValueDragger(false) {}

    virtual ~CustomSlider();

    double getValue();

    void setValue(double newvalue);

    void draw(int x, int y, int w, int h, const Cairo::RefPtr<Cairo::Context> &cr);

    void mouseDown(int x, int y, int w, int h);

    void mouseDragged(int x, int y, int w, int h);

    void mouseMoved(int x, int y, int w, int h);

    void mouseScrolled(int direction);

private:
    double from, to, value;
    std::string caption;
    std::function<void(double)> value_changed;

    bool mouseMovedOverDragger;

    int mouseDownX;
    int mouseDownY;

    bool clickedValueDragger;

    int getDraggerX(double value, int w);

    double getValue(int draggerX, int w);
};


#endif //CPPRAW_CUSTOMSLIDER_H
