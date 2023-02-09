#ifndef IXTLI_GRAPHICS_COLOR_H
#define IXTLI_GRAPHICS_COLOR_H

#include <iostream>
#include <Ixtli/Math.h>

namespace Ixtli{

class Color{
    private:
        unsigned int color;
    public: 
        enum Colors : unsigned int{
            BLACK = 0xFF000000,
            DARK_GRAY = 0xFF5E5E5E,
            GRAY = 0xFF808080,
            LIGHT_GRAY = 0xFFD0D0D0,
            WHITE = 0xFFFFFFFF,
            RED = 0xFFFF0000,
            GREEN = 0xFF00FF00,
            BLUE = 0xFF0000FF,
            YELLOW = 0xFFFFFF00,
            CYAN = 0xFF00FFFF,
            MAGENTA = 0xFFFF00FF,
            TRANSPARENT = 0x00000000,
        };

        enum Mask : unsigned int{
            MASK_ALPHA = 0xFF000000,
            MASK_RED = 0x00FF0000,
            MASK_GREEN = 0x0000FF00,
            MASK_BLUE = 0x000000FF,
        };
        
        Color(unsigned int value = RED):color(value){}; 
        ~Color(){};
        
        inline unsigned int a() const 
            { return (0xFF) & (color >> 24); }

        inline unsigned int r() const 
            { return (0xFF) & (color >> 16); }

        inline unsigned int g() const 
            { return (0xFF) & (color >> 8); }

        inline unsigned int b() const 
            { return (0xFF) & (color >> 0); }

        inline float a_f() const 
            { return static_cast<float>(a())/255.0; }

        inline float r_f() const 
            { return static_cast<float>(r())/255.0; }

        inline float g_f() const 
            { return static_cast<float>(g())/255.0; }

        inline float b_f() const 
            { return static_cast<float>(b())/255.0; }


        inline Color& setAlpha(unsigned int a)
            { color &= (~MASK_ALPHA);  color |= (a&0xFF) << 24; return *this; }
        
        inline Color& setR(unsigned int r)
            { color &= (~MASK_RED);  color |= (r&0xFF) << 16; return *this; }
        
        inline Color& setG(unsigned int g)
            { color &= (~MASK_GREEN);  color |= (g&0xFF) << 8; return *this; }
        
        inline Color& setB(unsigned int b)
            { color &= (~MASK_BLUE);  color |= (b&0xFF); return *this; }

        inline Color inverse(bool invertAlpha = false) const{
            if(invertAlpha)
                return Color::fromARGB(255U - a(), 255U - r(), 255U - g(), 255U - b());
            return Color::fromARGB(a(), 255U - r(), 255U - g(), 255U - b());
        }

        inline Color operator *(float p) const{
            if(p <= 0) return Color::fromARGB(a(), 0, 0, 0);
            unsigned int cr = max((unsigned int)(r()*p), 255U); 
            unsigned int cg = max((unsigned int)(g()*p), 255U); 
            unsigned int cb = max((unsigned int)(b()*p), 255U);
            return Color::fromARGB(a(), cr, cg, cb);
        }

        inline Color operator *(double p) const{
            if(p <= 0) return Color::fromARGB(a(), 0, 0, 0);
            unsigned int cr = max((unsigned int)(r()*p), 255U); 
            unsigned int cg = max((unsigned int)(g()*p), 255U); 
            unsigned int cb = max((unsigned int)(b()*p), 255U);
            return Color::fromARGB(a(), cr, cg, cb);
        }

        inline Color operator +(const Color& c) const{
            unsigned int ca = (a()*a() + c.a()*c.a()) >> 1;
            unsigned int cr = (r()*r() + c.r()*c.r()) >> 1;
            unsigned int cg = (g()*g() + c.g()*c.g()) >> 1;
            unsigned int cb = (b()*b() + c.b()*c.b()) >> 1;

            ca = min(255U, isqrt(ca));
            cr = min(255U, isqrt(cr));
            cg = min(255U, isqrt(cg));
            cb = min(255U, isqrt(cb));
            return Color::fromARGB(ca, cr, cg, cb);
        }

        inline Color operator -(const Color& b) const{
            return (*this + b.inverse());
        }

        inline Color operator ~() const{
            return inverse();
        }

        static inline Color fromARGB(unsigned int a, unsigned int r, unsigned int g, unsigned int b){
            Color c;
            c.color = (a&0xFF) << 24;
            c.color |= (r&0xFF) << 16;
            c.color |= (g&0xFF) << 8;
            c.color |= (b&0xFF);
            return c;
        }

        static inline Color fromRGB(unsigned int r, unsigned int g, unsigned int b){
            return fromARGB(255U, r, g, b);
        }

        static inline Color parse(const std::string& str, const Color& defaultColor = Color::BLACK){
            size_t i = 0;
            unsigned int color = 0;

            if(str.length() == 0 || str.at(0) != '#') return defaultColor;
            
            if(str.length() != 7 && str.length() != 9) return defaultColor;

            for(i=1; i < str.length(); i++){
                char dig = str[i];
                if(dig >= '0' && dig <= '9'){
                    color = (color << 4) + (dig - '0');
                }else if (dig >= 'a' && dig <= 'f'){
                    color = (color << 4) + (dig -'a') + 10UL;
                }else if(dig >= 'A' && dig <= 'F'){
                    color = (color << 4) + (dig -'A') + 10UL;
                } else{
                    return defaultColor;
                }
            }

            if(str.length() == 7) color |= (0xFF000000);

            return Color(color);
        }

        inline std::string toString() const{
            return "rgb: (" + std::to_string(r()) + "," + std::to_string(g()) + "," + std::to_string(b()) + ")";
        }
};

inline Color operator *(float p, const Color& c){
    if(p <= 0) return Color::fromARGB(c.a(), 0, 0, 0);
    unsigned int cr = min((unsigned int)(c.r()*p), 255U); 
    unsigned int cg = min((unsigned int)(c.g()*p), 255U); 
    unsigned int cb = min((unsigned int)(c.b()*p), 255U);
    return Color::fromARGB(c.a(), cr, cg, cb);
}

inline Color operator *(double p, const Color& c){
    if(p <= 0) return Color::fromARGB(c.a(), 0, 0, 0);
    unsigned int cr = min((unsigned int)(c.r()*p), 255U); 
    unsigned int cg = min((unsigned int)(c.g()*p), 255U); 
    unsigned int cb = min((unsigned int)(c.b()*p), 255U);
    return Color::fromARGB(c.a(), cr, cg, cb);
}

inline std::ostream& operator << (std::ostream& os, const Color& c){
    os << "argb: (" << c.a() << "," << c.r() << "," << c.g() << "," << c.b() << ")";
    return os;
}

};


#endif //IXTLI_GRAPHICS_COLOR_H