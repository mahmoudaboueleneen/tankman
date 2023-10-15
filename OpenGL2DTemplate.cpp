#include <stdio.h>
#include <math.h>
#include <random>
#include <glut.h>

void drawRect(int x, int y, int w, int h);
void drawCircle(int x, int y, float r);
void drawStar(int x, int y);

void Key(unsigned char key, int x, int y);
void KeyUp(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void Timer(int value);
void Display();

int selectedBar = 0; // used to determine which bar has the mouse currently over it
int bar1Y = 0;       // holds the Y translation value of bar 1
int bar2Y = 0;       // holds the Y translation value of bar 2
int bar3Y = 0;       // holds the Y translation value of bar 3
int ballY = 0;       // holds the value of the Y coordinate of the ball

void main(int argc, char** argr) {
    glutInit(&argc, argr);

    glutInitWindowSize(800, 800);
    glutInitWindowPosition(0, 0);

    glutCreateWindow("Game");
    glutDisplayFunc(Display);
    glutKeyboardFunc(Key);
    glutKeyboardUpFunc(KeyUp);
    glutMouseFunc(Mouse);
    glutTimerFunc(0, Timer, 0);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glClearColor(1, 1, 1, 0);
    gluOrtho2D(0, 800, 0, 800);

    glutMainLoop();
}

void drawRect(int x, int y, int w, int h) {
    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void drawCircle(int x, int y, float r) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    GLUquadric* quadObj = gluNewQuadric();
    gluDisk(quadObj, 0, r, 50, 50);
    glPopMatrix();
}

void drawStar(int x, int y) {
    glPushMatrix();
    glTranslatef(x, y, 0);

    glBegin(GL_LINE_LOOP);
    glVertex2f(0.0, 1.0);
    glVertex2f(0.58779, -0.80902);
    glVertex2f(-0.95106, 0.30902);
    glVertex2f(0.95106, 0.30902);
    glVertex2f(-0.58779, -0.80902);
    glEnd();

    glPopMatrix();
}



// Keyboard handler function
//   key: the key pressed on the keyboard
//   x  : the X coordinate of the mouse cursor at the moment of pressing the key
//   y  : the Y coordinate of the mouse cursor at the moment of pressing the key
void Key(unsigned char key, int x, int y) {
    // pick a bar based on the X position of the mouse, assuming the window is divided into 3 vertical slices
    if (x < 0)
        selectedBar = 1;
    else if (x < 0)
        selectedBar = 2;
    else
        selectedBar = 3;

    // check the selected bar, to decide which variable to control: bar1Y, bar2Y or bar3Y
    switch (selectedBar) {
    case 1:
        // if the key 'a' is pressed, increment bar1Y until it reaches the top limit of its support
        if (key == 'a' && bar1Y < 270)
            bar1Y++;
        // if the key 'z' is pressed, decrement bar1Y until it reaches the bottom limit of its support
        if (key == 'z' && bar1Y > -270)
            bar1Y--;
        break;
    case 2:
        // if the key 'a' is pressed, increment bar2Y until it reaches the top limit of its support
        if (key == 'a' && bar2Y < 90)
            bar2Y++;
        // if the key 'z' is pressed, decrement bar2Y until it reaches the bottom limit of its support
        if (key == 'z' && bar2Y > -90)
            bar2Y--;
        break;
    case 3:
        // if the key 'a' is pressed, increment bar3Y until it reaches the top limit of its support
        if (key == 'a' && bar3Y < 20)
            bar3Y++;
        // if the key 'z' is pressed, decrement bar3Y until it reaches the bottom limit of its support
        if (key == 'z' && bar3Y > -20)
            bar3Y--;
        break;
    }

    // ask OpenGL to recall the display function to reflect the changes on the window
    glutPostRedisplay();
}

// KeyboardUp handler function
//   similar to the Keyboard handler function except that it is called only when the key is released
void KeyUp(unsigned char key, int x, int y) {
    // reset the selectedBar value to 0 to unselect the selected bar
    selectedBar = 0;

    // ask OpenGL to recall the display function to reflect the changes on the window
    glutPostRedisplay();
}

// Mouse handler function
//   button: the mouse button clicked: left, right or middle
//   state:  the state of the button: clicked (down) or released (up)
//   x  : the X coordinate of the mouse cursor at the moment of clicking the mouse button
//   y  : the Y coordinate of the mouse cursor at the moment of clicking the mouse button
void Mouse(int button, int state, int x, int y) {
    // calculate the Y coordinate of the tip of the arm
    int barTipY = bar1Y + bar2Y + bar3Y + 400;

    // if the tip of the bar is inside the ball's circumference
    if (barTipY <= ballY + 15 && barTipY >= ballY - 15)
        // color the background green
        glClearColor(0, 1, 0, 0);
    else
        // otherwise, color it red
        glClearColor(1, 0, 0, 0);

    // ask OpenGL to recall the display function to reflect the changes on the window
    glutPostRedisplay();
}

void Timer(int value) {
    // set the ball's Y coordinate to a random number between 10 and 780 (since the window's height is 800)
    ballY = rand() % 780 + 10;

    // ask OpenGL to recall the display function to reflect the changes on the window
    glutPostRedisplay();

    // recall the Timer function after 20 seconds (20,000 milliseconds)
    glutTimerFunc(20 * 1000, Timer, 0);
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT);



  
    glPushMatrix();
    glTranslatef(0, bar1Y, 0);
    glColor3f(0.5, 0.5, 0.5);
    drawRect(30, 390, 150, 20); // rectangle 2

    // if the mouse is over the 2nd bar, color it yellow, otherwise black
    if (selectedBar == 2)
        glColor3f(1, 1, 0);
    else
        glColor3f(0, 0, 0);
    drawRect(180, 300, 20, 200); // rectangle 3

    //===== 2. create a new stack to control the 2nd part of the arm (rectangles 4 and 5)
    //         since this is a stack, rectangle 6 will be already drawn and will translate with rectangles 4 and 5
    glPushMatrix();
    glTranslatef(0, bar2Y, 0);
    glColor3f(0.5, 0.5, 0.5);
    drawRect(200, 390, 100, 20); // rectangle 4

    // if the mouse is over the 3rd bar, color it yellow, otherwise black
    if (selectedBar == 3)
        glColor3f(1, 1, 0);
    else
        glColor3f(0, 0, 0);
    drawRect(300, 370, 20, 60); // rectangle 5

    //===== 1. create a new stack to control the 3rd part of the arm (rectangle 6)
    glPushMatrix();
    glTranslatef(0, bar3Y, 0);
    glColor3f(0.5, 0.5, 0.5);
    drawRect(320, 390, 60, 20); // rectangle 6
    glPopMatrix();


    glFlush();
}
