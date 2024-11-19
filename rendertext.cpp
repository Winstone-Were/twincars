#include <GL/glut.h>
#include <string>

// Function to render text on the screen
void renderText(float x, float y, const std::string& text, void* font = GLUT_BITMAP_HELVETICA_18) {
    // Set the position for the text
    glRasterPos2f(x, y);

    // Render each character in the string
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
}
