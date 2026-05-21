#include <Ixtli/Graphics/Font.h>

#if defined(IXTLI_ENABLE_FREETYPE)
#include <ft2build.h>
#include FT_FREETYPE_H
#endif

using namespace Ixtli;

#if defined(IXTLI_ENABLE_FREETYPE)
namespace {
    inline FT_Library asLibrary(void* handle){
        return reinterpret_cast<FT_Library>(handle);
    }

    inline FT_Face asFace(void* handle){
        return reinterpret_cast<FT_Face>(handle);
    }
}
#endif

Font::Font() : path(), loaded(false), libraryHandle(nullptr), faceHandle(nullptr), glyphCache() {}

Font::Font(const char* fontPath) : Font(){
    if(fontPath != nullptr){
        loadFromFile(fontPath);
    }
}

Font::Font(const std::string& fontPath) : Font(){
    loadFromFile(fontPath);
}

Font::~Font(){
    clearGlyphCache();

#if defined(IXTLI_ENABLE_FREETYPE)
    if(faceHandle != nullptr){
        FT_Done_Face(asFace(faceHandle));
        faceHandle = nullptr;
    }

    if(libraryHandle != nullptr){
        FT_Done_FreeType(asLibrary(libraryHandle));
        libraryHandle = nullptr;
    }
#endif
}

bool Font::loadFromFile(const char* fontPath){
    if(fontPath == nullptr){
        return false;
    }
    return loadFromFile(std::string(fontPath));
}

bool Font::loadFromFile(const std::string& fontPath){
    clearGlyphCache();
    loaded = false;
    path = fontPath;

#if defined(IXTLI_ENABLE_FREETYPE)
    if(faceHandle != nullptr){
        FT_Done_Face(asFace(faceHandle));
        faceHandle = nullptr;
    }

    if(libraryHandle != nullptr){
        FT_Done_FreeType(asLibrary(libraryHandle));
        libraryHandle = nullptr;
    }

    if(fontPath.empty()){
        return false;
    }

    FT_Library library = nullptr;
    if(FT_Init_FreeType(&library) != 0){
        return false;
    }

    FT_Face face = nullptr;
    if(FT_New_Face(library, fontPath.c_str(), 0, &face) != 0){
        FT_Done_FreeType(library);
        return false;
    }

    libraryHandle = library;
    faceHandle = face;
    loaded = true;
    return true;
#else
    (void)fontPath;
    return false;
#endif
}

bool Font::isLoaded() const{
    return loaded;
}

const std::string& Font::getPath() const{
    return path;
}

void Font::clearGlyphCache() const{
    bool hasContext = glutGetWindow() != 0;

    for(auto& sizeEntry : glyphCache){
        for(auto& glyphEntry : sizeEntry.second){
            if(hasContext && glyphEntry.second.textureId != 0){
                GLuint textureId = glyphEntry.second.textureId;
                glDeleteTextures(1, &textureId);
            }
        }
    }

    glyphCache.clear();
}

bool Font::cacheGlyph(unsigned char c, unsigned int pixelSize) const{
#if defined(IXTLI_ENABLE_FREETYPE)
    if(!loaded || faceHandle == nullptr){
        return false;
    }

    unsigned int resolvedSize = max(1U, pixelSize);
    auto& glyphSet = glyphCache[resolvedSize];
    if(glyphSet.count(c) > 0){
        return true;
    }

    FT_Face face = asFace(faceHandle);
    if(FT_Set_Pixel_Sizes(face, 0, resolvedSize) != 0){
        return false;
    }

    if(FT_Load_Char(face, c, FT_LOAD_RENDER) != 0){
        return false;
    }

    Glyph glyph;
    glyph.width = face->glyph->bitmap.width;
    glyph.height = face->glyph->bitmap.rows;
    glyph.bearingX = face->glyph->bitmap_left;
    glyph.bearingY = face->glyph->bitmap_top;
    glyph.advance = face->glyph->advance.x >> 6;

    if(glyph.width > 0 && glyph.height > 0){
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGenTextures(1, &glyph.textureId);
        glBindTexture(GL_TEXTURE_2D, glyph.textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_ALPHA,
            glyph.width,
            glyph.height,
            0,
            GL_ALPHA,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glyphSet[c] = glyph;
    return true;
#else
    (void)c;
    (void)pixelSize;
    return false;
#endif
}

int Font::getTextWidth(const std::string& text, unsigned int pixelSize) const{
    if(!loaded){
        return 0;
    }

    unsigned int resolvedSize = max(1U, pixelSize);
    int currentWidth = 0;
    int maxWidth = 0;

    for(char ch : text){
        if(ch == '\n'){
            maxWidth = max(maxWidth, currentWidth);
            currentWidth = 0;
            continue;
        }

        unsigned char glyphCode = static_cast<unsigned char>(ch);
        if(cacheGlyph(glyphCode, resolvedSize)){
            currentWidth += glyphCache.at(resolvedSize).at(glyphCode).advance;
        }else{
            currentWidth += resolvedSize / 2;
        }
    }

    return max(maxWidth, currentWidth);
}

int Font::getTextHeight(unsigned int pixelSize) const{
    unsigned int resolvedSize = max(1U, pixelSize);

#if defined(IXTLI_ENABLE_FREETYPE)
    if(loaded && faceHandle != nullptr){
        FT_Face face = asFace(faceHandle);
        if(FT_Set_Pixel_Sizes(face, 0, resolvedSize) == 0 && face->size != nullptr){
            int lineHeight = face->size->metrics.height >> 6;
            if(lineHeight > 0){
                return lineHeight;
            }
        }
    }
#endif

    return static_cast<int>(resolvedSize);
}

bool Font::getTextBounds(const std::string& text, unsigned int pixelSize, Rect& bounds) const{
    if(!loaded){
        return false;
    }

    int lineHeight = getTextHeight(pixelSize);
    int lineCount = 1;

    for(char ch : text){
        if(ch == '\n'){
            lineCount++;
        }
    }

    bounds.x1 = 0;
    bounds.y1 = 0;
    bounds.x2 = getTextWidth(text, pixelSize);
    bounds.y2 = lineCount * lineHeight;
    return true;
}

void Font::drawText(const std::string& text, float x, float y, const Color& color, unsigned int pixelSize) const{
#if defined(IXTLI_ENABLE_FREETYPE)
    if(!loaded){
        return;
    }

    unsigned int resolvedSize = max(1U, pixelSize);
    float penX = x;
    float baselineY = y + getTextHeight(resolvedSize);

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_TEXTURE_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_POLYGON_SMOOTH);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor4ub(color.r(), color.g(), color.b(), color.a());

    for(char ch : text){
        if(ch == '\n'){
            penX = x;
            baselineY += getTextHeight(resolvedSize);
            continue;
        }

        unsigned char glyphCode = static_cast<unsigned char>(ch);
        if(!cacheGlyph(glyphCode, resolvedSize)){
            continue;
        }

        const Glyph& glyph = glyphCache.at(resolvedSize).at(glyphCode);
        float xpos = penX + glyph.bearingX;
        float ypos = baselineY - glyph.bearingY;

        if(glyph.textureId != 0 && glyph.width > 0 && glyph.height > 0){
            glBindTexture(GL_TEXTURE_2D, glyph.textureId);
            glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f); glVertex2f(xpos, ypos);
                glTexCoord2f(1.0f, 0.0f); glVertex2f(xpos + glyph.width, ypos);
                glTexCoord2f(1.0f, 1.0f); glVertex2f(xpos + glyph.width, ypos + glyph.height);
                glTexCoord2f(0.0f, 1.0f); glVertex2f(xpos, ypos + glyph.height);
            glEnd();
        }

        penX += glyph.advance;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glPopAttrib();
#else
    (void)text;
    (void)x;
    (void)y;
    (void)color;
    (void)pixelSize;
#endif
}