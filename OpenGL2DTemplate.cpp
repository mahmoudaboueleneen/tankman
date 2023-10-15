#include <stdio.h>
#include <math.h>
#include <random>
#include <glut.h>
#include <sstream>

int randomRange(int min, int max);
void drawRect(int x, int y, int w, int h);
void drawCircle(int x, int y, float r);
void drawHealthBar(int lives);
void drawSpeedPowerup(int x, int y, float scaleFactor);
void drawScorePowerup(int x, int y, float scaleFactor);
void drawRing(int x, int y, float innerR, float outerR);
void drawPlayer(int x, int y, float angle);
void drawBoundary(int x, int y, int w, int h);
void drawObstacle(int x, int y);
void drawGoal(int x, int y, float r);
void Key(unsigned char key, int x, int y);
void KeyUp(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void UpdateGameTime(int value);
void UpdateFrame(int value);
void Display();

struct Ring {
    int x;
    int y;
};

struct Obstacle {
    int x;
    int y;
};

struct SpeedPowerup {
    int x;
    int y;
};

struct ScorePowerup {
    int x;
    int y;
};

Ring* rings;
Obstacle* obstacles;
SpeedPowerup* speedPowerups;
ScorePowerup* scorePowerups;
float STANDARD_RADIUS = 15;
int remainingTime;
int remainingLives;
int score;
int numberOfRings;
int numberOfObstacles;
int numberOfSpeedPowerups;
int numberOfScorePowerups;
int goalX;
int goalY;
int playerX;
int playerY;
float playerAngle;
int playerSpeed;
bool keys[256];

void main(int argc, char** argr) {
    glutInit(&argc, argr);

    srand(time(NULL));

    glutInitWindowSize(800, 800);
    glutInitWindowPosition(0, 0);

    glutCreateWindow("Game");
    glutDisplayFunc(Display);
    glutKeyboardFunc(Key);
    glutKeyboardUpFunc(KeyUp);
    glutMouseFunc(Mouse);

    glutTimerFunc(1000, UpdateGameTime, 0);
    glutTimerFunc(0, UpdateFrame, 0);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glClearColor(0, 0, 0, 0);
    gluOrtho2D(0, 800, 0, 800);

    // Initialize player settings
    remainingTime = 30;
    remainingLives = 5;
    score = 0;

    // Initialize environment settings
    numberOfRings = 10;
    numberOfObstacles = 30;
    numberOfSpeedPowerups = 1;
    numberOfScorePowerups = 1;

    // Initialize rings
    rings = new Ring[numberOfRings];
    for (int i = 0; i < numberOfRings; i++) {
        rings[i].x = randomRange(100, 740);
        rings[i].y = randomRange(100, 740);
    }

    // Initialize obstacles
    obstacles = new Obstacle[numberOfObstacles];
    for (int i = 0; i < numberOfObstacles; i++) {
        obstacles[i].x = randomRange(120, 700);
        obstacles[i].y = randomRange(120, 700);
    }

    // Initialize speed powerups
    speedPowerups = new SpeedPowerup[numberOfSpeedPowerups];
    for (int i = 0; i < numberOfSpeedPowerups; i++) {
        speedPowerups[i].x = randomRange(120, 700);
        speedPowerups[i].y = randomRange(120, 700);
    }

    // Initialize score powerups
    scorePowerups = new ScorePowerup[numberOfScorePowerups];
    for (int i = 0; i < numberOfScorePowerups; i++) {
        scorePowerups[i].x = randomRange(120, 700);
        scorePowerups[i].y = randomRange(120, 700);
    }

    // Initialize goal
    goalX = randomRange(120, 700);
    goalY = randomRange(120, 700);

    // Initialize player
    playerX = 400;
    playerY = 400;
    playerAngle = 0;
    playerSpeed = 5;

    glutMainLoop();
}

int randomRange(int min, int max) {
    return min + rand() % (max - min + 1);
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
void drawHealthBar(int lives) {
    int startX = 50;
    int startY = 762;
    int width = 30;
    int height = 25;
    int gap = 7;

    glColor3f(0, 0.847, 0); // Green

    for (int i = 0; i < lives; i++) {
        drawRect(startX + i * (width + gap), startY, width, height);
    }
}
void drawSpeedPowerup(int x, int y, float scaleFactor) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(scaleFactor, scaleFactor, 1);

    glColor3f(0.678, 0.847, 0.902); // Light blue

    glBegin(GL_LINES);
    glVertex2f(0.0, 1.0);
    glVertex2f(0.58779, -0.80902);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(0.58779, -0.80902);
    glVertex2f(-0.95106, 0.30902);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(-0.95106, 0.30902);
    glVertex2f(0.95106, 0.30902);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(0.95106, 0.30902);
    glVertex2f(-0.58779, -0.80902);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(-0.58779, -0.80902);
    glVertex2f(0.0, 1.0);
    glEnd();

    glPopMatrix();
}
void drawScorePowerup(int x, int y, float scaleFactor) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(scaleFactor, scaleFactor, 1);

    glColor3f(1.0f, 0.0f, 0.0f); // Red

    glBegin(GL_LINES);
    glVertex2f(0.0, 1.0);
    glVertex2f(0.58779, -0.80902);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(0.58779, -0.80902);
    glVertex2f(-0.95106, 0.30902);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(-0.95106, 0.30902);
    glVertex2f(0.95106, 0.30902);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(0.95106, 0.30902);
    glVertex2f(-0.58779, -0.80902);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(-0.58779, -0.80902);
    glVertex2f(0.0, 1.0);
    glEnd();

    glPopMatrix();
}
void drawRing(int x, int y, float innerR, float outerR) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glColor3f(1.0f, 0.843137f, 0.0f); // Gold
    GLUquadric* quadObj = gluNewQuadric();
    gluDisk(quadObj, 0, outerR, 50, 50);
    glColor3f(0,0,0); // Black
    gluDisk(quadObj, 0, innerR, 50, 50);
    glPopMatrix();
}
void drawPlayer(int x, int y, float angle) {
    glPushMatrix(); 
    glTranslatef(x, y, 0);
    glRotatef(angle, 0, 0, 1);
    glColor3f(1, 1, 1);
    glLineWidth(5);

    // Tank body
    glBegin(GL_QUADS);
    glVertex2f(-10, -10);
    glVertex2f(10, -10);
    glVertex2f(10, 10);
    glVertex2f(-10, 10);
    glEnd();

    // Turret
    glBegin(GL_LINES);
    glVertex2f(0, 0);
    glVertex2f(0, 15);
    glEnd();

    // Cannon
    glBegin(GL_TRIANGLES);
    glVertex2f(0, 15);
    glVertex2f(-5, 20);
    glVertex2f(5, 20);
    glEnd();

    // Wheels
    glPointSize(5.0);
    glBegin(GL_POINTS);
    glVertex2f(-12, -12); // Front left wheel
    glVertex2f(12, -12); // Front right wheel
    glVertex2f(-12, 12); // Back left wheel
    glVertex2f(12, 12); // Back right wheel
    glEnd();

    glPopMatrix();
}
void drawBoundary(int x, int y, int w, int h) {
    glColor3f(1.0, 1.0, 1.0); // White

    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(x, y + h);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(x, y + h);
    glVertex2f(x + w, y + h);
    glEnd();
}
void drawObstacle(int x, int y) {
    glColor3f(0.5, 0.5, 0.5); // Gray

    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    glVertex2f(x + 20, y + 10);
    glVertex2f(x + 30, y + 20);
    glVertex2f(x + 20, y + 40);
    glVertex2f(x - 10, y + 30);
    glVertex2f(x - 20, y + 20);
    glVertex2f(x - 10, y + 10);
    glEnd();
}
void drawGoal(int x, int y, float r) {
    glColor3f(0.5, 0.0, 0.5); // Purple
    drawCircle(x, y, r);

    glColor3f(0.0, 0.0, 0.0); // Black
    drawCircle(x, y, r - 5);

    glColor3f(1.0, 1.0, 1.0); // White X
    glLineWidth(2.0);
    glBegin(GL_LINES);
    glVertex2f(x - r / 2, y - r / 2);
    glVertex2f(x + r / 2, y + r / 2);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(x - r / 2, y + r / 2);
    glVertex2f(x + r / 2, y - r / 2);
    glEnd();

    glColor3f(1.0, 1.0, 1.0);
    drawCircle(x, y, r / 4);
}
void Key(unsigned char key, int x, int y) {
    keys[key] = true;
}
void KeyUp(unsigned char key, int x, int y) {
    keys[key] = false;
}
void Mouse(int button, int state, int x, int y) {
    
    glutPostRedisplay();
}
void UpdateGameTime(int value) {
    if (remainingTime <= 0) {
        exit(0);
    }

    remainingTime--;

    glutPostRedisplay();

    glutTimerFunc(1000, UpdateGameTime, value);
}
void UpdateFrame(int value) {
    if (keys['w'] || keys['W']) {
        playerY += playerSpeed;
        playerAngle = 0;
    }
    if (keys['s'] || keys['S']) {
        playerY -= playerSpeed;
        playerAngle = 180;
    }
    if (keys['a'] || keys['A']) {
        playerX -= playerSpeed;
        playerAngle = 90;
    }
    if (keys['d'] || keys['D']) {
        playerX += playerSpeed;
        playerAngle = 270;
    }

    glutPostRedisplay();

    glutTimerFunc(1000 / 240, UpdateFrame, value + 1); // 240 FPS
}
void Display() {
    glClear(GL_COLOR_BUFFER_BIT);
    // Timer
    glColor3f(1, 1, 1);
    glRasterPos2i(650, 770);
    std::ostringstream oss;
    oss << remainingTime;
    std::string timeString = "Time: " + oss.str();
    for (char c : timeString) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    // Score
    glColor3f(1, 1, 1);
    glRasterPos2i(410, 770);
    std::ostringstream ossScore;
    ossScore << score;
    std::string scoreString = "Score: " + ossScore.str();
    for (char c : scoreString) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    // Health Bar
    drawHealthBar(remainingLives);

    // Boundary
    drawBoundary(50, 50, 700, 700);

    // Rings
    for (int i = 0; i < numberOfRings; i++) {
        drawRing(rings[i].x, rings[i].y, STANDARD_RADIUS/2, STANDARD_RADIUS);
    }

    // Obstacles
    for (int i = 0; i < numberOfObstacles; i++) {
        drawObstacle(obstacles[i].x, obstacles[i].y);
    }

    // Speed Powerups
    for (int i = 0; i < numberOfSpeedPowerups; i++) {
        drawSpeedPowerup(speedPowerups[i].x, speedPowerups[i].y, STANDARD_RADIUS);
    }

    // Score Powerups
    for (int i = 0; i < numberOfScorePowerups; i++) {
        drawScorePowerup(scorePowerups[i].x, scorePowerups[i].y, STANDARD_RADIUS);
    }

    // Goal
    drawGoal(goalX, goalY, STANDARD_RADIUS);

    // Player
    drawPlayer(playerX, playerY, playerAngle);

    glFlush();
}
