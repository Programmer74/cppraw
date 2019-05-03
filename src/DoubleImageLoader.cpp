#include "DoubleImageLoader.h"

double gGamma = 1 / 2.2222;
double gA = 1.0;

int atoi(std::vector<char> data, int offset) {
    int res = 0;
    int i = offset;
    while (i < offset + 10) {
        char c = (char) data[i];
        if ((c < '0') || (c > '9')) {
            break;
        }
        res = res * 10 + (c - '0');
        i++;
    }
    return res;
}

DoubleImage* DoubleImageLoader::load_image() {
    std::cout << "I am in " << getenv("PWD") << std::endl;
    std::ifstream infile("../../bayered.pgm", std::ifstream::binary);

    std::vector<char> data;

//get length of file
    infile.seekg(0, infile.end);
    size_t length = infile.tellg();
    infile.seekg(0, infile.beg);

//read file
    if (length > 0) {
        data.resize(length);
        infile.read(&data[0], length);
    }


    int offset = 0;

    if (data[0] != 'P') {
        std::cerr << "NOT FOUND P" << std::endl;
    }

    if ((data[1] != ('5')) && (data[1] != ('6'))) {
        std::cerr << "NOT FOUND P5 OR P6" << std::endl;
    }
    offset += 3;

    int cols = atoi(data, offset);
    offset += 4 + 1;
    int rows = atoi(data, offset);
    offset += 4 + 1;

    int image_width = cols;
    int image_height = rows;

    std::cout << "Dimensions: " << image_width << "x" << image_height << std::endl;
    int maxValue = atoi(data, offset);
    std::cout << "Max value: " << std::endl;
    offset += 5 + 1;

    if (data[1] == ('6')) {
        std::cout << "Processing coloured PGM" << std::endl;
        //lastProcessedImageIsColouredImage = true;
        cols *= 3;
    } else {
        std::cout << "Processing monochrome PGM (not debayered)" << std::endl;
        //lastProcessedImageIsColouredImage = false;
    }

    int *pgmPixels = (int *) std::malloc(rows * cols * sizeof(int));

    std::cout << "Array: " << cols << "x" + rows << std::endl;
    //doubleImage = new DoubleImage(image_width, image_height, getDefaultValues());
    std::cout << "Reading inputStream image of size " << image_width << " by " << image_height << std::endl;

    int pixelH, pixelL;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            pixelH = data[offset] & 0xFF;
            pixelL = data[offset + 1] & 0xFF;
            pgmPixels[r * cols + c] = pixelH << 8 | pixelL;
            offset += 2;
        }
    }

    std::cout << "Read OK" << std::endl;

    auto* ans = new DoubleImage(image_width, image_height);


    // copy the pixels values
    for (int r = 0; r < rows; r++)
        for (int c = 0, x = 0; c < cols && x < image_width; c += 3, x++) {

            double rv = 0, gv = 0, bv = 0;

            rv = pgmPixels[r * cols + c] * 1.0 / maxValue;
            gv = pgmPixels[r * cols + c + 1] * 1.0 / maxValue;
            bv = pgmPixels[r * cols + c + 2] * 1.0 / maxValue;

            //gamma correction (slow in runtime)
            rv = gA * std::pow(rv, gGamma);
            gv = gA * std::pow(gv, gGamma);
            bv = gA * std::pow(bv, gGamma);

            ans->set_color(x, r, rv, gv, bv);
        }
    ans->autoAdjustWhiteBalance();
    return ans;
}
