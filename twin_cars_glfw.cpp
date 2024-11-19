#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <GL/glut.h>
#include <thread>
#include <chrono>
#include "rendertext.h"

const int TARGET_FPS = 30;
const double FRAME_DURATION = 1.0 / TARGET_FPS;

float car1X = -0.4f; // Car 1 position
float car2X = 0.4f;  // Car 2 position
bool isPaused = false;
bool isGameOver = false;
int score = 0;

struct Block
{
    float x; // X position
    float y; // Y position
};

const int numBlocks = 5;
Block blocks[numBlocks];

void initializeBlocks()
{
    for (int i = 0; i < numBlocks; i++)
    {
        float bias = (rand() % 11 - 5) / 20.0f;
        blocks[i].x = ((rand() % 2 == 0 ? -0.5f : 0.5f) + bias);
        blocks[i].y = 1.0f + i * 0.5f;
    }
}

void drawBlocks()
{
    glColor3f(1.0f, 0.0f, 0.0f);
    for (int i = 0; i < numBlocks; i++)
    {
        glBegin(GL_POLYGON);
        glVertex2f(blocks[i].x - 0.1f, blocks[i].y);
        glVertex2f(blocks[i].x - 0.1f, blocks[i].y + 0.2f);
        glVertex2f(blocks[i].x + 0.1f, blocks[i].y + 0.2f);
        glVertex2f(blocks[i].x + 0.1f, blocks[i].y);
        glEnd();
    }
}

void moveBlocks()
{
    for (int i = 0; i < numBlocks; i++)
    {
        blocks[i].y -= 0.01f;
        if (blocks[i].y < -1.0f)
        {
            blocks[i].y = 1.0f;
            blocks[i].x = (rand() % 2 == 0 ? -0.5f : 0.5f);
            score += 1;
        }
    }
}

void checkCollision()
{
    for (int i = 0; i < numBlocks; i++)
    {
        if (blocks[i].y <= -0.8f && blocks[i].y >= -1.0f &&
            blocks[i].x >= car1X - 0.1f && blocks[i].x <= car1X + 0.1f)
        {
            isGameOver = true;
        }
        if (blocks[i].y <= -0.8f && blocks[i].y >= -1.0f &&
            blocks[i].x >= car2X - 0.1f && blocks[i].x <= car2X + 0.1f)
        {
            isGameOver = true;
        }
    }
}

void drawCar(float x, float y)
{
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
    glVertex2f(x - 0.1, y + 0.2);   // Bottom-left corner of the car body
    glVertex2f(x - 0.07, y + 0.24); // Top-left corner
    glVertex2f(x + 0.07, y + 0.24); // Top-right corner
    glVertex2f(x + 0.1, y + 0.2);   // Bottom-right corner
    glEnd();

    // Back Trapezium (Yellow)
    glBegin(GL_POLYGON);
    glVertex2f(x - 0.07, y - 0.04); // Bottom-left corner of the car body
    glVertex2f(x - 0.1, y);         // Top-left corner
    glVertex2f(x + 0.1, y);         // Top-right corner
    glVertex2f(x + 0.07, y - 0.04); // Bottom-right corner
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

void drawRoad()
{
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(-1.0f, 1.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(1.0f, -1.0f);
    glEnd();

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
    for (float j = -1.0f; j <= 1.0f; j += 0.2f)
    { // Dashed lines
        glBegin(GL_QUADS);
        glVertex2f(-0.5f - laneWidth, j);        // Left side of the line
        glVertex2f(-0.5f - laneWidth, j + 0.1f); // Top-left of the line
        glVertex2f(-0.5f + laneWidth, j + 0.1f); // Top-right of the line
        glVertex2f(-0.5f + laneWidth, j);        // Bottom-right of the line
        glEnd();
    }

    // Right side lane
    for (float j = -1.0f; j <= 1.0f; j += 0.2f)
    { // Dashed lines
        glBegin(GL_QUADS);
        glVertex2f(0.5f - laneWidth, j);        // Left side of the line
        glVertex2f(0.5f - laneWidth, j + 0.1f); // Top-left of the line
        glVertex2f(0.5f + laneWidth, j + 0.1f); // Top-right of the line
        glVertex2f(0.5f + laneWidth, j);        // Bottom-right of the line
        glEnd();
    }
}

void drawGameOver()
{
    glColor3f(1.0f, 0.0f, 0.0f);
    glRasterPos2f(-0.2f, 0.0f);
    const char *msg = "GAME OVER";
    while (*msg)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *msg);
        msg++;
    }
}

void drawInstructions()
{
    glColor3f(1.0f, 1.0f, 1.0f);
    // Display instructions on the right side
    glRasterPos2f(0.5f, 0.2f); // Adjust position for the right-side text
    const char *rightInstructions1 = "A: Move Left";
    while (*rightInstructions1)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *rightInstructions1);
        rightInstructions1++;
    }

    glRasterPos2f(0.5f, 0.1f); // Adjust position for the next line
    const char *rightInstructions2 = "D: Move Right";
    while (*rightInstructions2)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *rightInstructions2);
        rightInstructions2++;
    }

    // Display instructions on the left side
    glRasterPos2f(-0.8f, 0.2f); // Adjust position for the left-side text
    const char *leftInstructions1 = "J: Move Left";
    while (*leftInstructions1)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *leftInstructions1);
        leftInstructions1++;
    }

    glRasterPos2f(-0.8f, 0.1f); // Adjust position for the next line
    const char *leftInstructions2 = "L: Move Right";
    while (*leftInstructions2)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *leftInstructions2);
        leftInstructions2++;
    }
}

void drawPauseScreen()
{
    glColor3f(0.0f, 0.0f, 1.0f);
    glRasterPos2f(-0.2f, 0.0f);
    const char *msg = "PAUSED";
    while (*msg)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *msg);
        msg++;
    }
}


void renderBitmapString(float x, float y, void *font, const std::string &text)
{
    glRasterPos2f(x, y);
    for (char c : text)
    {
        glutBitmapCharacter(font, c);
    }
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        car1X -= 0.1f;
        if (car1X < -0.9f)
            car1X = -0.9f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        car1X += 0.1f;
        if (car1X > -0.3f)
            car1X = -0.3f;
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
    {
        car2X -= 0.1f;
        if (car2X < 0.3f)
            car2X = 0.3f;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
        car2X += 0.1f;
        if (car2X > 0.9f)
            car2X = 0.9f;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        isPaused = true;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {
        isPaused = false;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        initializeBlocks();
        isGameOver = false;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    if (isGameOver)
    {
        drawGameOver();
    }
    else if (!isPaused)
    {
        drawRoad();
        drawCar(car1X, -0.8f);
        drawCar(car2X, -0.8f);
        drawBlocks();
    }
    else
    {
        drawPauseScreen();
        drawInstructions();
    }
    glColor3f(1.0f, 1.0f, 1.0f);
    // renderBitmapString(-0.9f, 0.9f, GLUT_BITMAP_HELVETICA_18, "Score: " + std::to_string(score));
    renderText(-0.8f, 0.8f, "Score: " + std::to_string(score), GLUT_BITMAP_HELVETICA_18);
    glfwSwapBuffers(glfwGetCurrentContext());
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow *window = glfwCreateWindow(800, 800, "Car Game", NULL, NULL);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    int argc = 1;
    char *argv[1] = {(char *)"Something"};
    glutInit(&argc, argv);

    initializeBlocks();

    while (!glfwWindowShouldClose(window))
    {
        double lastFrameTime = glfwGetTime();
        double currentTime = glfwGetTime();
        double elapsedTime = currentTime - lastFrameTime;
        if (elapsedTime < FRAME_DURATION)
        {
            // Sleep for the remaining time of the frame
            std::this_thread::sleep_for(
                std::chrono::duration<double>(FRAME_DURATION - elapsedTime));
        }
        lastFrameTime = glfwGetTime();

        processInput(window);
        if (!isPaused && !isGameOver)
        {
            moveBlocks();
            checkCollision();
        }
        display();
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
