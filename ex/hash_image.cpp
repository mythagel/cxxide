#include <iostream>
#include <vector>
#include <iterator>

struct ppm_image_t
{
    struct pixel
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
    };
    size_t x;
    size_t y;
    std::vector<pixel> buf;
    
    ppm_image_t(size_t x, size_t y)
     : x(x), y(y)
    {
        buf.resize(x*y);
    }
};

std::ostream& operator<<(std::ostream& os, const ppm_image_t& img)
{
    os << "P6\n";
    os << img.x << ' ' << img.y << ' ' << ' ' << 255 << '\n';
    for(auto& p : img.buf)
        os << p.r << p.g << p.b;
    return os;
}

ppm_image_t image_from_string(const std::string& word)
{
    /* todo
       hash string
       set pixels in pattern based on hash with vertical symmetry.
       
    */
}

int main()
{
    ppm_image_t img(128, 128);
    
    int i = 0;
    for(auto& p : img.buf)
    {
        if(i % 2 == 0)
        {
            p.r = 255;
            p.g = 0;
            p.b = 0;
        }
        else if(i % 3 == 0)
        {
            p.r = 0;
            p.g = 255;
            p.b = 0;
        }
        else if(i % 5 == 0)
        {
            p.r = 0;
            p.g = 0;
            p.b = 255;
        }
        ++i;
    }
    
    std::cout << img;
    
    return 0;
}
