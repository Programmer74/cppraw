#ifndef CPPRAW_IMAGEADJUSTMENTS_H
#define CPPRAW_IMAGEADJUSTMENTS_H


class ImageAdjustments {
public:
    double exposureStop = 0;
    double brightness = 0;
    double contrast = 1.0;

    double rWb = 1.0;
    double gWb = 1.0;
    double bWb = 1.0;

    double b_treshold = 0;
    double b_exposureStop = 0;
};


#endif //CPPRAW_IMAGEADJUSTMENTS_H
