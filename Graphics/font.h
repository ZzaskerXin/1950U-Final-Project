#pragma once

#include "ft2build.h"
#include FT_FREETYPE_H
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <map>
#include <string>

struct Character {
    GLuint textureID;         // Glyph texture ID
    glm::ivec2 size;          // Size of glyph
    glm::ivec2 bearing;       // Offset from baseline to left/top of glyph
    unsigned int advance;     // Offset to advance to next glyph
};

class Font {
public:
    Font(const std::string filepath);
    ~Font();

    // Returns the Character data for a given Unicode codepoint
    Character& getCharacter(wchar_t c);

private:
    void createGlyphTexture(wchar_t c, FT_Face& face);

    // Unicode glyph map
    std::map<wchar_t, Character> m_characters;
};
