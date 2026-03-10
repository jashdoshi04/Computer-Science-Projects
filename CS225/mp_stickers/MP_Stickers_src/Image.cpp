#include "Image.h"
//Image() : PNG() {};
//Image(unsigned int width, unsigned int height) : PNG(width, height) {};

void Image::lighten(){
    for(unsigned w = 0 ; w < width(); w++) {
        for(unsigned h = 0 ; h < height(); h++ ) {
            double p = (this->getPixel(w,h)).l; 
            //std::cout << p << std::endl;
            if(p < 0.9){
                p += 0.1;
                //std::cout << p << std::endl;
                (this->getPixel(w,h)).l = p;
            } else  {
                (this->getPixel(w,h)).l = 1;
            }
        }
    }
}
void Image::lighten(double amount){
    for(unsigned w = 0 ; w < width(); w++) {
        for(unsigned h = 0 ; h < height(); h++ ) {
            double p = (this->getPixel(w,h)).l; 
            p += amount; 
            if(p > 1) {
                (this->getPixel(w,h)).l = 1;
            } else {
                (this->getPixel(w,h)).l = p;
            }
        }
    }
}
void Image::darken(){
    for(unsigned w = 0 ; w < width(); w++) {
        for(unsigned h = 0 ; h < height(); h++ ) {
            double p = (this->getPixel(w,h)).l; 
            if(p > 0.1){
                p -= 0.1;
                (this->getPixel(w,h)).l = p;
            } else  {
                (this->getPixel(w,h)).l = 0;
            }
        }
    }
}
void Image:: darken(double amount){
    for(unsigned w = 0 ; w < width(); w++) {
        for(unsigned h = 0 ; h < height(); h++ ) {
            double p = (this->getPixel(w,h)).l; 
            p -= amount; 
            if(p < 0) {
                (this->getPixel(w,h)).l = 0;
            } else {
                (this->getPixel(w,h)).l = p;
            }
        }
    }
}
void Image::saturate(){
    for(unsigned w = 0 ; w < width(); w++) {
        for(unsigned h = 0 ; h < height(); h++ ) {
            double p = (this->getPixel(w,h)).s; 
            if(p < 0.9){
                p += 0.1;
                (this->getPixel(w,h)).s = p;
            } else  {
                (this->getPixel(w,h)).s = 1;
            }
        }
    }
}

void Image::saturate(double amount){
    for(unsigned w = 0 ; w < width(); w++) {
        for(unsigned h = 0 ; h < height(); h++ ) {
            double p = (this->getPixel(w,h)).s; 
            p += amount; 
            if(p > 1) {
                (this->getPixel(w,h)).s = 1;
            } else {
                (this->getPixel(w,h)).s = p;
            }
        }
    }
}

void Image::desaturate() {
    for(unsigned w = 0 ; w < width(); w++) {
        for(unsigned h = 0 ; h < height(); h++ ) {
            double p = (this->getPixel(w,h)).s; 
            if(p > 0.1){
                p -= 0.1;
                (this->getPixel(w,h)).s = p;
            } else  {
                (this->getPixel(w,h)).s = 0;
            }
        }
    }
}

void Image::desaturate(double amount){
    for(unsigned w = 0 ; w < width(); w++) {
        for(unsigned h = 0 ; h < height(); h++ ) {
            double p = (this->getPixel(w,h)).s; 
            p -= amount; 
            if(p < 0) {
                (this->getPixel(w,h)).s = 0;
            } else {
                (this->getPixel(w,h)).s = p;
            }
        }
    }
}
 void Image::grayscale() {
    for(unsigned w = 0 ; w < width(); w++) {
        for(unsigned h = 0 ; h < height(); h++ ) {
            (this->getPixel(w,h)).s = 0;
        }
    }
 }
 void Image::rotateColor(double degrees) {
for(unsigned w = 0 ; w < width(); w++) {
        for(unsigned h = 0 ; h < height(); h++ ) {
            double hue = (this->getPixel(w,h)).h;
            hue += degrees; 
            if(hue < 0) {
                hue += 360;
                (this->getPixel(w,h)).h = h;
            } else if(hue > 360) {
                hue -= 360;
            (this->getPixel(w,h)).h = hue; 
            } 
            (this->getPixel(w,h)).h = hue;  
        }
    }  
 }

 void Image::illinify() {
    for(unsigned w = 0 ; w < width(); w++) {
        for(unsigned h = 0 ; h < height(); h++ ) {
            double hue = (this->getPixel(w,h)).h;
            double dis2blue = 216 - hue;
            double dis2orange = 11 - hue;

            if(dis2blue < dis2orange) {
                (this->getPixel(w,h)).h = 216;
            } else  {
                (this->getPixel(w,h)).h = 11;
            }

        }
    }
 }
  void Image::scale(unsigned w, unsigned h) {
    double scale_width_factor = (double)w/(double)(this->width());
    double scale_height_factor = (double)h/(double)(this->height());

        if(scale_width_factor < scale_height_factor) {
            scale(scale_width_factor);
        } else {
            scale(scale_height_factor);
        }
  } 
    void Image::scale(double factor) {
        unsigned new_width = (unsigned)(factor * (double)this->width());
        unsigned new_height = (unsigned)(factor * (double)this->height());

        Image new_image(new_width, new_height);

        for(unsigned int i = 0 ; i < new_width; i++) {
            for(unsigned int j = 0; j < new_height; j++) {
                unsigned int old_x = (unsigned int)(double(i)/factor);
                unsigned int old_y = (unsigned int)(double(j)/factor);
                cs225:: HSLAPixel& new_pixel = new_image.getPixel(i,j);
                new_pixel = this->getPixel(old_x,old_y);
            }
        }
        this->resize(new_width,new_height);
        *this = new_image;
    }