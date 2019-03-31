#ifndef CPPRAW_CUSTOMSLIDER_H
#define CPPRAW_CUSTOMSLIDER_H

#include <string>
#include <cairomm/context.h>
#include <functional>

class CustomSlider {

public:
    CustomSlider(std::string caption, double from, double to, double value, std::function<void(double)> value_changed) :
            caption(caption), from(from), to(to), value(value), defaultValue(value), value_changed(value_changed),
            mouseMovedOverDragger(false), mouseDownX(0), mouseDownY(0),
            clickedValueDragger(false) {}

    CustomSlider(std::string caption, double from, double to, double value, std::function<void(double)> value_changed,
                 double rlA, double glA, double blA, double rrA,
                 double grA, double brA, double rlB, double glB, double blB, double rrB, double grB, double brB) : from(
            from), to(to), value(value), defaultValue(value), caption(caption), value_changed(value_changed), rlA(rlA),
                                                                                                                   glA(glA),
                                                                                                                   blA(blA),
                                                                                                                   rrA(rrA),
                                                                                                                   grA(grA),
                                                                                                                   brA(brA),
                                                                                                                   rlB(rlB),
                                                                                                                   glB(glB),
                                                                                                                   blB(blB),
                                                                                                                   rrB(rrB),
                                                                                                                   grB(grB),
                                                                                                                   brB(brB) {}
    CustomSlider(std::string caption, double from, double to, double value, std::function<void(double)> value_changed,
                 double rlA, double glA, double blA, double rrA,
                 double grA, double brA) : from(
            from), to(to), value(value), defaultValue(value), caption(caption), value_changed(value_changed), rlA(rlA),
                                                                                                                   glA(glA),
                                                                                                                   blA(blA),
                                                                                                                   rrA(rrA),
                                                                                                                   grA(grA),
                                                                                                                   brA(brA),
                                                                                                                   rlB(rlA),
                                                                                                                   glB(glA),
                                                                                                                   blB(blA),
                                                                                                                   rrB(rrA),
                                                                                                                   grB(grA),
                                                                                                                   brB(brA) {}

    virtual ~CustomSlider();

    double getValue();

    void setValue(double newvalue);

    void draw(int x, int y, int w, int h, const Cairo::RefPtr<Cairo::Context> &cr);

    void doubleClick(int x, int y, int w, int h);

    void mouseDown(int x, int y, int w, int h);

    void mouseDragged(int x, int y, int w, int h);

    void mouseMoved(int x, int y, int w, int h);

    void mouseScrolled(int direction);

private:
    double from, to, value, defaultValue;
    std::string caption;
    std::function<void(double)> value_changed;

    bool mouseMovedOverDragger;

    int mouseDownX;
    int mouseDownY;

    bool clickedValueDragger;

    double rlA = 0, glA = 0, blA = 0, rrA = 0, grA = 0, brA = 0;
    double rlB = 0, glB = 0, blB = 0, rrB = 0, grB = 0, brB = 0;

    int getDraggerX(double value, int w);

    double getValue(int draggerX, int w);

    double getValue(int draggerX, int w, double from, double to);
};


#endif //CPPRAW_CUSTOMSLIDER_H
