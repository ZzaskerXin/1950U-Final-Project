#include "textrenderer.h"
#include "debug.h"

#include <locale>
#include <codecvt>



void TextRenderer::initialize(){
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);   
}

void TextRenderer::renderUIText(std::shared_ptr<Font> font, std::string text, glm::vec2 anchorPosition, AnchorPoint anchorPoint, float textBoxWidth, float fontSize, float lineSpacing, glm::vec3 textColor){
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_vao);

    float x = 0;
    float y = 0;

    float maxY = font->getCharacter(73).size.y * fontSize;
    float minY = - (font->getCharacter(106).size.y - font->getCharacter(106).bearing.y) * fontSize;

    GLint viewport[4];
glGetIntegerv(GL_VIEWPORT, viewport);
int windowWidth = viewport[2];
int windowHeight = viewport[3];
glm::vec2 screenSize = glm::vec2(windowWidth, windowHeight);

switch (anchorPoint) {
    case AnchorPoint::TopLeft:
        x = anchorPosition.x;
        y = screenSize.y - anchorPosition.y - maxY;
        break;
    case AnchorPoint::TopCenter:
        x = anchorPosition.x - textBoxWidth * 0.5f;
        y = screenSize.y - anchorPosition.y - maxY;
        break;
    case AnchorPoint::TopRight:
        x = screenSize.x - anchorPosition.x - textBoxWidth;
        y = screenSize.y - anchorPosition.y - maxY;
        break;
    case AnchorPoint::BottomLeft:
        x = anchorPosition.x;
        y = anchorPosition.y;
        break;
    case AnchorPoint::BottomCenter:
        x = anchorPosition.x - textBoxWidth * 0.5f;
        y = anchorPosition.y;
        break;
    case AnchorPoint::BottomRight:
        x = screenSize.x - anchorPosition.x - textBoxWidth;
        y = anchorPosition.y;
        break;
    case AnchorPoint::CenterLeft:
        x = anchorPosition.x;
        y = screenSize.y * 0.5f - (maxY - minY) * 0.5f;
        break;
    case AnchorPoint::Center:
        x = anchorPosition.x - textBoxWidth * 0.5f;
        y = screenSize.y * 0.5f - (maxY - minY) * 0.5f;
        break;
    case AnchorPoint::CenterRight:
        x = screenSize.x - anchorPosition.x - textBoxWidth;
        y = screenSize.y * 0.5f - (maxY - minY) * 0.5f;
        break;
    default:
        x = anchorPosition.x;
        y = screenSize.y - anchorPosition.y - maxY;
        break;
}



    float initX = x;
    float initY = y;

    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring wtext = converter.from_bytes(text);

    for (wchar_t c : wtext) {
        Character ch;
        try {
            ch = font->getCharacter(c);
        } catch (...) {
            std::cerr << "[TextRenderer] Missing glyph for codepoint: " << std::hex << (int)c << std::endl;
            continue;
        }
        
        float xpos = x + ch.bearing.x * fontSize;
        float ypos = y - (ch.size.y - ch.bearing.y) * fontSize;

        float w = ch.size.x * fontSize;
        float h = ch.size.y * fontSize;

        if(xpos + w > initX + textBoxWidth){
            x = initX;
            y -= (1.f + lineSpacing) * (maxY - minY);

            xpos = x + ch.bearing.x * fontSize;
            ypos = y - (ch.size.y - ch.bearing.y) * fontSize;
        }


        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * fontSize; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}