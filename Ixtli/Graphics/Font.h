#ifndef IXTLI_GRAPHICS_FONT_H
#define IXTLI_GRAPHICS_FONT_H

#include <string>
#include <unordered_map>

#include <GL/freeglut.h>

#include <Ixtli/Graphics/Color.h>
#include <Ixtli/Graphics/Rectangle.h>

namespace Ixtli {

class Font{
    public:
        struct Glyph{
            GLuint textureId;
            int width;
            int height;
            int bearingX;
            int bearingY;
            int advance;

            Glyph() : textureId(0), width(0), height(0), bearingX(0), bearingY(0), advance(0) {}
        };

    private:
        std::string path;
        bool loaded;
        mutable void* libraryHandle;
        mutable void* faceHandle;
        mutable std::unordered_map<unsigned int, std::unordered_map<unsigned char, Glyph>> glyphCache;

        void clearGlyphCache() const;
        bool cacheGlyph(unsigned char c, unsigned int pixelSize) const;

    public:
        Font();
        explicit Font(const char* path);
        explicit Font(const std::string& path);
        virtual ~Font();

        bool loadFromFile(const char* path);
        bool loadFromFile(const std::string& path);

        bool isLoaded() const;
        const std::string& getPath() const;

        int getTextWidth(const std::string& text, unsigned int pixelSize) const;
        int getTextHeight(unsigned int pixelSize) const;
        bool getTextBounds(const std::string& text, unsigned int pixelSize, Rect& bounds) const;

        void drawText(const std::string& text, float x, float y, const Color& color, unsigned int pixelSize) const;
};

}
#endif //IXTLI_GRAPHICS_FONT_H