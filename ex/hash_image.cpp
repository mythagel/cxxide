#include <iostream>
#include <vector>
#include <iterator>
#include <string>
#include <bitset>

uint64_t fnv1a(const std::string& str)
{
    uint64_t hash = 14695981039346656037ULL;
    for(auto c : str)
    {
        hash ^= static_cast<uint64_t>(c);
        hash *= 1099511628211ULL;
    }
    return hash;
}

struct ppm_image_t
{
    struct pixel_t
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
    };
    size_t x;
    size_t y;
    std::vector<pixel_t> buf;
    
    ppm_image_t(size_t x, size_t y)
     : x(x), y(y)
    {
        buf.resize(x*y);
    }
    
    void pixel(int x, int y, const pixel_t& p)
    {
        buf[this->x * y + x] = p;
    }
    pixel_t pixel(int x, int y) const
    {
        return buf[this->x * y + x];
    }
};

void scale(const ppm_image_t& source, ppm_image_t& dest)
{
    for(size_t x = 0; x < dest.x; ++x)
        for(size_t y = 0; y < dest.y; ++y)
            dest.pixel(x, y, source.pixel(x*source.x/dest.x, y*source.y/dest.y));
}

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

int main(int argc, char* argv[])
{
    std::string key;
    ppm_image_t img(8, 8);
    
    if(argc > 1)
        key = argv[1];
    
    std::bitset<64> hash(fnv1a(key));
    
    std::bitset<64> mask(0xFF);
    ppm_image_t::pixel_t c;
    c.r = (hash & mask).to_ulong();
    c.g = ((hash & (mask << 8)) >> 8).to_ulong();
    c.b = ((hash & (mask << 16)) >> 16).to_ulong();
    ppm_image_t::pixel_t w;
    w.r = 255;
    w.g = 255;
    w.b = 255;    
    
    int i = 0;
    for(auto& p : img.buf)
        p = hash[i++] ? c : w;

    {    
        ppm_image_t nimg(512, 512);
        
        scale(img, nimg);
        
        std::cout << nimg;
    }
    return 0;
}
