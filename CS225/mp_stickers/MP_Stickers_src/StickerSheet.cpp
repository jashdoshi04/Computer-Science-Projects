#include "StickerSheet.h"

StickerSheet:: StickerSheet(const Image& picture){
        base_image = picture;
        //base_image = deep;
        //num_layers = 0;
        //curr_index_x = 0;
        //curr_index_y = 0;

        //for(int i = 0; i < num_layers ; i++ ) {
            //sticker_layers.push_back(nullptr);
           // x_coordinate.push_back(0);
            //y_coordinate.push_back(0);

        //}
}
const StickerSheet& StickerSheet::operator= (const StickerSheet &other) {
    if(this == &other) {
        return *this;
    }
    this->sticker_layers.clear();
    this->x_coordinate.clear();
    this->y_coordinate.clear();
   this->base_image = other.base_image;
   num_layers = other.num_layers;

   for(unsigned int i = 0 ; i < other.num_layers; i++) {
    this->sticker_layers.push_back(other.sticker_layers.at(i));
    this->x_coordinate.push_back(other.x_coordinate.at(i));
    this->y_coordinate.push_back(other.y_coordinate.at(i));
   }
return *this;
}
StickerSheet::StickerSheet (const StickerSheet &other){

    // const Image* base_image_deep = new Image();
    // base_image_deep = other.base_image;
    // base_image = base_image_deep; 
    base_image= other.base_image;

    num_layers = other.num_layers;
    for (unsigned int i = 0; i < other.sticker_layers.size(); i++) {
    // Image* sticker_deep = new Image();
     //sticker_deep = sticker_layers.at(i);
    this->sticker_layers.push_back(other.sticker_layers.at(i));
    this->x_coordinate.push_back(other.x_coordinate.at(i));
    this->y_coordinate.push_back(other.y_coordinate.at(i));
   }
}
int StickerSheet:: addSticker(Image& sticker, int x, int y){
    std::cout << "enter" << std::endl;
    //if(num_layers > 0) {
    //     if(num_layers == 0) {
    // sticker_layers.clear();
    // }
    for(unsigned int i = 0 ; i < num_layers; i++) {
        if(sticker_layers[i] == nullptr) {
            Image* curr_stick = &sticker;
            sticker_layers[i] = curr_stick;
            x_coordinate.at(i) = x;
            y_coordinate.at(i) = y;
            num_layers++;
            //std::cout << "i"<<i <<std::endl;
            return i;
        }    
    }
    // } else if (num_layers = 0) {

    // }
    Image* curr_stick = &sticker;
    x_coordinate.push_back(x);
    y_coordinate.push_back(y);
    sticker_layers.push_back(curr_stick);
    num_layers ++;
    //std::cout << "num layers: "<<num_layers  << sticker_layers.size() <<std::endl;
    //curr_layer++;
    return num_layers -1;
}
int StickerSheet:: setStickerAtLayer(Image& sticker, unsigned layer, int x, int y){
    Image* curr_stick = &sticker;
    if(layer < num_layers) {
    
    sticker_layers.at(layer) = curr_stick;
    x_coordinate.at(layer) = x; 
    y_coordinate.at(layer)  = y;
    return layer;
    } else {
        sticker_layers.push_back(curr_stick);
        x_coordinate.push_back(x);
        y_coordinate.push_back(y);
        num_layers++;
    return num_layers - 1;
    }
    return -1;
}
bool StickerSheet:: translate(unsigned index, int x, int y) {
    std::cout << num_layers << std::endl;
    if(index < num_layers) {
        std::cout << "false enter" << std::endl;
        x_coordinate.at(index) = x;
        y_coordinate.at(index) = y;
        return true;
    } 
    return false;
}
void StickerSheet:: removeSticker(unsigned index){

if(index < sticker_layers.size()) {
    std::cout << "removed" << index << std::endl;
    // sticker_layers.at(index) = nullptr;
    // sticker_layers.erase(sticker_layers.begin())
    // x_coordinate.at(index) = 0;
    // y_coordinate.at(index) = 0;
    num_layers--;
    sticker_layers.erase(sticker_layers.begin() + index);
    x_coordinate.erase(x_coordinate.begin() + index);
    y_coordinate.erase(y_coordinate.begin() + index);
} else {
    return ;
}
}
Image* StickerSheet:: getSticker(unsigned index){
    if(index < sticker_layers.size() ) {
        //std:: cout << &sticker_layers.at(index) << std::endl;
        return sticker_layers.at(index);
    } else  {
        return nullptr;
    }
}
int StickerSheet:: layers() const {
    return num_layers;
}
Image StickerSheet:: render() const{
    int origin_x = 0;
    int origin_y = 0;
    unsigned int max_x  = 0;
    unsigned int max_y = 0;
    max_x = base_image.width();
    //max_y = base_image.height();
    unsigned int x_width = base_image.width();
    unsigned int y_height = base_image.height();
    
for(unsigned int i = 0; i < sticker_layers.size() ; i++) {
    std::cout << "loop1" << y_height <<max_y << std::endl;
    Image* current = sticker_layers.at(i);
    if(current != nullptr) {
        // if(std::abs(this->x_coordinate.at(i)) + curr->width() > x_width) {
        //     x_width = this->x_coordinate.at(i) + curr->width();
        //     //std::cout << x_width << std::endl;
        // }
        // if(std::abs(this->y_coordinate.at(i)) + curr->height() > y_height) {
        //     y_height = this->y_coordinate.at(i) + curr->height();
        // }
        if(origin_x > this->x_coordinate.at(i)) {
             origin_x = this->x_coordinate.at(i);
        }
        if(origin_y > this->y_coordinate.at(i)) {
             origin_y = this->y_coordinate.at(i);
        }
        if(max_x < this->x_coordinate.at(i) + current->width()) {
            max_x = this->x_coordinate.at(i) + current->width();
        }
        std::cout << y_height << this->y_coordinate.at(i) + (int)current->height() << std::endl;
        if((int)y_height < this->y_coordinate.at(i) + (int)current->height()) {
            y_height = this->y_coordinate.at(i) + (int)current->height();
            std::cout << this->y_coordinate.at(i) << y_coordinate.at(i)  << current->height()<< max_y <<std::endl;
        }
    }
}
unsigned int offset_x = std::abs(origin_x);
unsigned int offset_y = std::abs(origin_y);
std::cout <<"max_x" << max_x << "max_y "<< max_y << "offset_x" << offset_x <<"offset_y" <<offset_y << std::endl;
unsigned int new_x_width = max_x + offset_x;
unsigned int new_y_height = y_height + offset_y;
std::cout << "reached here"  << new_x_width << new_y_height << std::endl;
Image rendered (new_x_width,new_y_height);
std::cout << y_height << rendered.height()<< std::endl;
for(unsigned int i = 0; i < x_width; i++) {
    for(unsigned int j = 0; j < base_image.height(); j++) {
        
       //std::cout << "loop2" << std::endl;
        cs225:: HSLAPixel& new_pixel = rendered.getPixel(i + offset_x ,j + offset_y);
        new_pixel = base_image.getPixel(i,j);
    }
}
for(unsigned int i = 0 ; i < sticker_layers.size(); i++) {
    std::cout << "loop3 " << sticker_layers.size()<< " " << num_layers<< std::endl;
    std::cout << i << std::endl;
    if(this->sticker_layers.at(i) != nullptr) {
        Image& curr = *(sticker_layers.at(i));
        std::cout << "loop4" << std::endl;
        for(unsigned int w = 0; w < curr.width();w++) {
            for(unsigned int h = 0; h < curr.height();h++) {
                //std::cout << "loop4" << std::endl;
                //std::cout << rendered.width() << rendered.height() << curr.width() << curr.height() << std::endl;
                
                cs225::HSLAPixel& sticker_pixel = curr.getPixel(w, h);
                
                if (sticker_pixel.a != 0) {
                        rendered.getPixel(w + x_coordinate.at(i) + offset_x ,h + y_coordinate.at(i) + offset_y) = sticker_pixel;
                    }
            }
        }
    }
}
return rendered;
}
