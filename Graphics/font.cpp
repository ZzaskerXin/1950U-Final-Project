#include "font.h"
#include <iostream>
#include <locale>
#include <codecvt>

Font::Font(const std::string filepath){
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }

    FT_Face face;
    if (FT_New_Face(ft, filepath.data(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font: " << filepath << std::endl;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load characters explicitly
    for (wchar_t c = 32; c < 127; ++c) {
        createGlyphTexture(c, face);
    }

    std::wstring extras = L"♥█░";
    for (wchar_t c : extras) {
        createGlyphTexture(c, face);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

Font::~Font() {}

void Font::createGlyphTexture(wchar_t c, FT_Face& face) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
        std::cout << "ERROR::FREETYPE: Failed to load Glyph for " << (int)c << std::endl;
        return;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        face->glyph->bitmap.width,
        face->glyph->bitmap.rows,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        face->glyph->bitmap.buffer
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Character character = {
        texture,
        glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
        glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
        static_cast<unsigned int>(face->glyph->advance.x)
    };

    m_characters[c] = character;
}

Character& Font::getCharacter(wchar_t c) {
    if (m_characters.count(c) == 0) {
        std::cerr << "[Font] Missing glyph: " << (int)c << std::endl;
        static Character fallback = m_characters.count('?') ? m_characters['?'] : Character{};
        return fallback;
    }
    return m_characters[c];
}
