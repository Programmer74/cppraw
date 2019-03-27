#include <gtkmm.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdint.h>

Gtk::ApplicationWindow *pWindow = nullptr;
Gtk::DrawingArea *gImage = nullptr;

uint8_t *static_image_buf = new uint8_t[1920 * 1080 * 3];

typedef struct {
    double r;
    double g;
    double b;
} pixel_t;

pixel_t *image = nullptr;
int image_width = 0;
int image_height = 0;

int image_pane_offset_x = 0;
int image_pane_offset_y = 0;
double image_pane_scale = 0;
bool autoscale_performed = false;
bool force_centering_image = false;

int image_pane_mouse_x = -1;
int image_pane_mouse_y = -1;

int image_pane_mouse_down_x = -1;
int image_pane_mouse_down_y = -1;

void set_color(int x, int y, double r, double g, double b) {
    image[x * image_height + y].r = r;
    image[x * image_height + y].g = g;
    image[x * image_height + y].b = b;
}

void get_color(int x, int y, double *r, double *g, double *b) {
    *r = image[x * image_height + y].r;
    *g = image[x * image_height + y].g;
    *b = image[x * image_height + y].b;
}

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

void load_image() {
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

    image_width = cols;
    image_height = rows;

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
    image = (pixel_t *) std::malloc(image_width * image_height * 3 * sizeof(pixel_t));

    // copy the pixels values
    for (int r = 0; r < rows; r++)
        for (int c = 0, x = 0; c < cols && x < image_width; c += 3, x++) {

            double rv = 0, gv = 0, bv = 0;

            rv = pgmPixels[r * cols + c] * 1.0 / maxValue;
            gv = pgmPixels[r * cols + c + 1] * 1.0 / maxValue;
            bv = pgmPixels[r * cols + c + 2] * 1.0 / maxValue;

            set_color(x, r, rv, gv, bv);
        }
}

bool on_imagepane_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
    Gtk::Allocation allocation = gImage->get_allocation();
    const int pane_width = allocation.get_width();
    const int pane_height = allocation.get_height();

    Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_data(static_image_buf,
                                                                     Gdk::COLORSPACE_RGB, false, 8, pane_width,
                                                                     pane_height,
                                                                     pane_width * 3);

    if (!autoscale_performed) {
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
                get_color(sx, sy, &r, &g, &b);
            }
            static_image_buf[y * pane_width * 3 + x * 3] = (uint8_t) (r * 255.0);
            static_image_buf[y * pane_width * 3 + x * 3 + 1] = (uint8_t) (g * 255.0);
            static_image_buf[y * pane_width * 3 + x * 3 + 2] = (uint8_t) (b * 255.0);
        }
    }

    Gdk::Cairo::set_source_pixbuf(cr, pixbuf, 0, 0);
    cr->rectangle(0, 0, pixbuf->get_width(), pixbuf->get_height());
    cr->fill();

    return true;
}

bool on_imagepane_scroll(GdkEventScroll *e) {
    if (e->direction == GDK_SCROLL_UP) {
        image_pane_scale += 0.1;
        if (image_pane_scale > 10) image_pane_scale = 10;
    } else if (e->direction == GDK_SCROLL_DOWN) {
        image_pane_scale -= 0.1;
        if (image_pane_scale < 0.5) image_pane_scale = 0.5;
    }
    gImage->queue_draw();
    std::cout << "Scale: " << image_pane_scale << std::endl;

    return true;
}

bool on_imagepane_motion(GdkEventMotion *e) {
    image_pane_mouse_x = e->x;
    image_pane_mouse_y = e->y;

    int ox = (image_pane_mouse_down_x - image_pane_mouse_x);
    int oy = (image_pane_mouse_down_y - image_pane_mouse_y);

    image_pane_offset_x -= ox;
    image_pane_offset_y -= oy;

    image_pane_mouse_down_x = image_pane_mouse_x;
    image_pane_mouse_down_y = image_pane_mouse_y;

    std::cout << "offsets " << image_pane_offset_x << " " << image_pane_offset_y << std::endl;

    gImage->queue_draw();

    return true;
}

bool on_imagepane_press_event(GdkEventButton *e) {
    if ((e->type == GDK_BUTTON_PRESS) && (e->button == 1)) {
        image_pane_mouse_down_x = e->x;
        image_pane_mouse_down_y = e->y;
    }
    return true;
}

int main(int argc, char **argv) {
    load_image();
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

    auto refBuilder = Gtk::Builder::create();
    try {
        refBuilder->add_from_file("../../test.glade");
    }
    catch (const Glib::FileError &ex) {
        std::cerr << "FileError: " << ex.what() << std::endl;
        return 1;
    }
    catch (const Glib::MarkupError &ex) {
        std::cerr << "MarkupError: " << ex.what() << std::endl;
        return 1;
    }
    catch (const Gtk::BuilderError &ex) {
        std::cerr << "BuilderError: " << ex.what() << std::endl;
        return 1;
    }

    refBuilder->get_widget("applicationwindow1", pWindow);
    if (pWindow) {
        //Get the GtkBuilder-instantiated Button, and connect a signal handler:

        refBuilder->get_widget("gImage", gImage);
        if (gImage) {
            std::cout << "lel" << std::endl;
            gImage->signal_draw().connect(sigc::ptr_fun(on_imagepane_draw));
            gImage->add_events(Gdk::SCROLL_MASK | Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON_PRESS_MASK |
                               Gdk::POINTER_MOTION_HINT_MASK);
            gImage->signal_scroll_event().connect(sigc::ptr_fun(on_imagepane_scroll));
            gImage->signal_motion_notify_event().connect(sigc::ptr_fun(on_imagepane_motion));
            gImage->signal_button_press_event().connect(sigc::ptr_fun(on_imagepane_press_event));
        }

        app->run(*pWindow);
    }

    delete pWindow;

    return 0;
}
