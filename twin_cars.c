#include <GL/glut.h>
#include <stdio.h>
#include <stdbool.h>

float car1X = -0.4f; // Car 1 position
float car2X = 0.4f;  // Car 2 position
bool isPaused = false;
bool isGameOver = false;
int score = 0; 

struct Block {
    float x;  // X position
    float y;  // Y position
};

int numBlocks = 5; // Number of blocks on the screen
struct Block blocks[5];

void renderBitmapString(float x, float y, void* font, const char* text) {
    glRasterPos2f(x, y); // Set the position for drawing
    while (*text) {      // Loop through each character in the string
        glutBitmapCharacter(font, *text);
        text++;
    }
}


void initializeBlocks() {
    for (int i = 0; i < numBlocks; i++) {
        // Random lanes: center positions -0.5 (left) or 0.5 (right)
        float bias = (rand() % 11 - 5) / 20.0f;
        blocks[i].x = ((rand() % 2 == 0 ? -0.5f : 0.5f)+bias); 
        blocks[i].y = 1.0f + i * 0.5f; // Staggered start positions
    }
}


void drawBlocks() {
    glColor3f(1.0, 0.0, 0.0); // Red for blocks
    for (int i = 0; i < numBlocks; i++) {
        glBegin(GL_POLYGON);
        glVertex2f(blocks[i].x - 0.1f, blocks[i].y);
        glVertex2f(blocks[i].x - 0.1f, blocks[i].y + 0.2f);
        glVertex2f(blocks[i].x + 0.1f, blocks[i].y + 0.2f);
        glVertex2f(blocks[i].x + 0.1f, blocks[i].y);
        glEnd();
    }
}


void moveBlocks() {
    for (int i = 0; i < numBlocks; i++) {
        blocks[i].y -= 0.01f; // Move block down

        if (blocks[i].y < -1.0f) { // Reset block when it goes off-screen
            blocks[i].y = 1.0f;
            blocks[i].x = (rand() % 2 == 0 ? -0.5f : 0.5f); // Randomize lane

            score += 1; // Increment score when block passes cars
        }
    }
}

void checkCollision() {
    for (int i = 0; i < numBlocks; i++) {
        // Check collision with car1
        if (blocks[i].y <= -0.8f && blocks[i].y >= -1.0f &&
            blocks[i].x >= car1X - 0.1f && blocks[i].x <= car1X + 0.1f) {
            isGameOver = true;
        }

        // Check collision with car2
        if (blocks[i].y <= -0.8f && blocks[i].y >= -1.0f &&
            blocks[i].x >= car2X - 0.1f && blocks[i].x <= car2X + 0.1f) {
            isGameOver = true;
        }
    }
}


void drawCar(float x, float y) {
    // Body of the car (Blue)
    glColor3f(0.0, 0.0, 1.0); // Blue for the car body
    glBegin(GL_POLYGON);
    glVertex2f(x - 0.1, y);       // Bottom-left corner of the car body
    glVertex2f(x - 0.1, y + 0.2); // Top-left corner
    glVertex2f(x + 0.1, y + 0.2); // Top-right corner
    glVertex2f(x + 0.1, y);       // Bottom-right corner
    glEnd();

    // Front Trapezium (Yellow)
    glColor3f(1.0, 1.0, 0.0); // Yellow for the front trapezium
    glBegin(GL_POLYGON);
    glVertex2f(x - 0.1, y+0.2);       // Bottom-left corner of the car body
    glVertex2f(x - 0.07, y + 0.24); // Top-left corner
    glVertex2f(x + 0.07, y + 0.24); // Top-right corner
    glVertex2f(x + 0.1, y+0.2);       // Bottom-right corner
    glEnd();

    // Back Trapezium (Yellow)
    glBegin(GL_POLYGON);
    glVertex2f(x - 0.07, y-0.04);       // Bottom-left corner of the car body
    glVertex2f(x - 0.1, y); // Top-left corner
    glVertex2f(x + 0.1, y); // Top-right corner
    glVertex2f(x + 0.07, y-0.04);       // Bottom-right corner
    glEnd();

    // Wheels of the car (Gray)
    glColor3f(0.5, 0.5, 0.5); // Gray for the wheels
    glBegin(GL_QUADS);

    // Bottom-left wheel
    glVertex2f(x - 0.12, y + 0.02);
    glVertex2f(x - 0.12, y + 0.07);
    glVertex2f(x - 0.08, y + 0.07);
    glVertex2f(x - 0.08, y + 0.02);

    // Bottom-right wheel
    glVertex2f(x + 0.08, y + 0.02);
    glVertex2f(x + 0.08, y + 0.07);
    glVertex2f(x + 0.12, y + 0.07);
    glVertex2f(x + 0.12, y + 0.02);

    // Top-left wheel
    glVertex2f(x - 0.12, y + 0.13);
    glVertex2f(x - 0.12, y + 0.18);
    glVertex2f(x - 0.08, y + 0.18);
    glVertex2f(x - 0.08, y + 0.13);

    // Top-right wheel
    glVertex2f(x + 0.12, y + 0.13);
    glVertex2f(x + 0.12, y + 0.18);
    glVertex2f(x + 0.08, y + 0.18);
    glVertex2f(x + 0.08, y + 0.13);
    glEnd();
}



void drawRoad() {
    // Road background
    glColor3f(0.0, 0.0, 0.0); // Black for road
    glBegin(GL_QUADS);
    glVertex2f(-1.0, -1.0);
    glVertex2f(-1.0, 1.0);
    glVertex2f(1.0, 1.0);
    glVertex2f(1.0, -1.0);
    glEnd();

    // Green divider
    glColor3f(0.0, 1.0, 0.0); // Green
    glBegin(GL_QUADS);
    glVertex2f(-0.02, -1.0);
    glVertex2f(-0.02, 1.0);
    glVertex2f(0.02, 1.0);
    glVertex2f(0.02, -1.0);
    glEnd();

    // White lane dividers (thicker lines)
    glColor3f(1.0, 1.0, 1.0); // White
    float laneWidth = 0.02f;  // Thickness of the lines

    // Left side lane
    for (float j = -1.0f; j <= 1.0f; j += 0.2f) { // Dashed lines
        glBegin(GL_QUADS);
        glVertex2f(-0.5f - laneWidth, j);        // Left side of the line
        glVertex2f(-0.5f - laneWidth, j + 0.1f); // Top-left of the line
        glVertex2f(-0.5f + laneWidth, j + 0.1f); // Top-right of the line
        glVertex2f(-0.5f + laneWidth, j);        // Bottom-right of the line
        glEnd();
    }

    // Right side lane
    for (float j = -1.0f; j <= 1.0f; j += 0.2f) { // Dashed lines
        glBegin(GL_QUADS);
        glVertex2f(0.5f - laneWidth, j);        // Left side of the line
        glVertex2f(0.5f - laneWidth, j + 0.1f); // Top-left of the line
        glVertex2f(0.5f + laneWidth, j + 0.1f); // Top-right of the line
        glVertex2f(0.5f + laneWidth, j);        // Bottom-right of the line
        glEnd();
    }
}





void drawGameOver() {
    glColor3f(1.0, 0.0, 0.0); // Red for Game Over text
    glRasterPos2f(-0.2, 0.0);
    const char *msg = "GAME OVER";
    while (*msg) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *msg);
        msg++;
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (isGameOver) {
        drawGameOver();
    } else {
        drawRoad();
        drawCar(car1X, -0.8f); // Draw car 1
        drawCar(car2X, -0.8f); // Draw car 2
        drawBlocks();
    }

    glColor3f(1.0f, 1.0f, 1.0f);
    char scoreText[50];
    sprintf(scoreText, "Score: %d", score);
    renderBitmapString(-0.9f, 0.9f, GLUT_BITMAP_HELVETICA_18, scoreText);

    glFlush();
}

void keyboard(unsigned char key, int x, int y) {
    //if (isGameOver) return;

    if (key == 'a' || key == 'A') {
        car1X -= 0.1f;
        if (car1X < -0.9f) car1X = -0.9f; // Constrain movement
    }
    if (key == 'd' || key == 'D') {
        car1X += 0.1f;
        if (car1X > -0.3f) car1X = -0.3f; // Constrain movement
    }
    if (key == 'j' || key == 'J') {
        car2X -= 0.1f;
        if (car2X < 0.3f) car2X = 0.3f; // Constrain movement
    }
    if (key == 'l' || key == 'L') {
        car2X += 0.1f;
        if (car2X > 0.9f) car2X = 0.9f; // Constrain movement
    }

    if (key == 'p' || key == 'P') {
        isPaused = !isPaused; // Toggle pause
    }

    if (key == 's' || key == 'S') {
        initializeBlocks();
        isGameOver = false;
    }

    if (key == 'q' || key == 'Q') {
        exit(0); // Quit the game
    }

    glutPostRedisplay();
}

void timer(int value) {
    if (!isPaused && !isGameOver) {
        moveBlocks();     // Move blocks downward
        checkCollision(); // Check for collisions
    }
    glutPostRedisplay();
    glutTimerFunc(32, timer, 0); // Refresh at ~60 FPS
}

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0); // Black background
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}



int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Twin Cars");
    init();
    initializeBlocks();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, timer, 0); // Start timer
    glutMainLoop();
    return 0;
}
