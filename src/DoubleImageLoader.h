#ifndef CPPRAW_DOUBLEIMAGELOADER_H
#define CPPRAW_DOUBLEIMAGELOADER_H

#include <gtkmm.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdint.h>
#include "DoubleImage.h"

class DoubleImageLoader {
public:
    DoubleImage* load_image();
};


#endif //CPPRAW_DOUBLEIMAGELOADER_H
