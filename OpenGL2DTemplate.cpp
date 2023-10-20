#include <stdio.h>
#include <math.h>
#include <random>
#include <glut.h>
#include <sstream>
#include <set>
#include <utility>
#include <irrKlang/irrKlang.h> using namespace irrklang;

std::pair<int, int> randomRange(int min, int max);
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
void AnimBackground(int value);
void UpdateGameTime(int value);
void HandlePlayerMovement(int value);
bool checkCollision(int x1, int y1, int r1, int x2, int y2, int r2);
void EndInvincibility(int value);
void EndSpeedBoost(int value);
void EndScoreBoost(int value);
void DisplayWinningScreen();
void DisplayLosingScreen();
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
std::set<std::pair<int, int>> usedPositions;
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
int boundaryX;
int boundaryY;
int boundaryW;
int boundaryH;
bool isPlayerInvincible;
bool isSpeedBoostActive;
bool isScoreBoostActive;
float backgroundAnimState;
bool isGameOver;

void main(int argc, char** argr) {
    glutInit(&argc, argr);

    srand(time(NULL));

    glutInitWindowSize(800, 800);
    glutInitWindowPosition(0, 0);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutCreateWindow("Game");
    glutDisplayFunc(Display);
    glutKeyboardFunc(Key);
    glutKeyboardUpFunc(KeyUp);
    glutMouseFunc(Mouse);

    glutTimerFunc(1000 / 60, AnimBackground, 0);
    glutTimerFunc(1000, UpdateGameTime, 0);
    glutTimerFunc(0, HandlePlayerMovement, 0);

    gluOrtho2D(0, 800, 0, 800);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize game boundary
    boundaryX = 50;
    boundaryY = 50;
    boundaryW = 700; 
    boundaryH = 700;

    // Initialize player settings
    remainingTime = 30;
    remainingLives = 5;
    score = 0;

    // Initialize environment 
    numberOfRings = 10;
    numberOfObstacles = 30;
    numberOfSpeedPowerups = 1;
    numberOfScorePowerups = 1;

    // Initialize rings
    rings = new Ring[numberOfRings];
    for (int i = 0; i < numberOfRings; i++) {
        std::pair<int, int> position = randomRange(boundaryX + 50, boundaryH + 30);
        rings[i].x = position.first;
        rings[i].y = position.second;
    }

    // Initialize obstacles
    obstacles = new Obstacle[numberOfObstacles];
    for (int i = 0; i < numberOfObstacles; i++) {
        std::pair<int, int> position = randomRange(boundaryX + 70, boundaryH + 0);
        obstacles[i].x = position.first;
        obstacles[i].y = position.second;
    }

    // Initialize speed powerups
    speedPowerups = new SpeedPowerup[numberOfSpeedPowerups];
    for (int i = 0; i < numberOfSpeedPowerups; i++) {
        std::pair<int, int> position = randomRange(boundaryX + 70, boundaryH + 0);
        speedPowerups[i].x = position.first;
        speedPowerups[i].y = position.second;
    }

    // Initialize score powerups
    scorePowerups = new ScorePowerup[numberOfScorePowerups];
    for (int i = 0; i < numberOfScorePowerups; i++) {
        std::pair<int, int> position = randomRange(boundaryX + 70, boundaryH + 0);
        scorePowerups[i].x = position.first;
        scorePowerups[i].y = position.second;
    }

    // Initialize goal
    goalX = (boundaryX + 70);
    goalY = (boundaryY + 70);

    // Initialize player
    playerX = 400;
    playerY = 400;
    playerAngle = 0;
    playerSpeed = 5;

    glutMainLoop();
}

std::pair<int, int> randomRange(int min, int max) {
    int x, y;
    int emptySpaceAroundEachPosition = STANDARD_RADIUS + 40;
    bool isTooClose;

    do {
        x = min + rand() % (max - min + 1);
        y = min + rand() % (max - min + 1);
        isTooClose = false;
        // check if the new position is too close to any of the used positions
        for (const auto& usedPos : usedPositions) 
            if (abs(x - usedPos.first) < emptySpaceAroundEachPosition && abs(y - usedPos.second) < emptySpaceAroundEachPosition) {
                isTooClose = true;
                break;
            }
    } while (
        isTooClose // don't place too close to another used position
            ||
        usedPositions.find(std::make_pair(x, y)) != usedPositions.end() // don't place in the same position as another used position
            ||
        (x > 400 - emptySpaceAroundEachPosition && x < 400 + emptySpaceAroundEachPosition &&
         y > 400 - emptySpaceAroundEachPosition && y < 400 + emptySpaceAroundEachPosition) // don't place in middle of screen (as it's where the player spawns)
            ||
        (x > boundaryX + 70 - emptySpaceAroundEachPosition && x < boundaryX + 70 + emptySpaceAroundEachPosition &&
         y > boundaryY + 70 - emptySpaceAroundEachPosition && y < boundaryY + 70 + emptySpaceAroundEachPosition) // don't place in the left corner of the screen (as it's where the goal spawns)
        );

    usedPositions.insert(std::make_pair(x, y));
    return std::make_pair(x, y);
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

    glColor3f(1.0, 1.0f, 0.0); // Yellow

    glBegin(GL_POLYGON);
    glVertex2f(-0.5, 1.0);
    glVertex2f(0.5, 1.0);
    glVertex2f(0.0, 0.5);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(-0.5, 1.0);
    glVertex2f(0.0, 0.5);
    glVertex2f(-0.5, 0.0);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(0.5, 1.0);
    glVertex2f(0.5, 0.0);
    glVertex2f(0.0, 0.5);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(-0.5, 0.0);
    glVertex2f(0.5, 0.0);
    glVertex2f(0.0, -1.0);
    glEnd();

    glPopMatrix();
}
void drawScorePowerup(int x, int y, float scaleFactor) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(scaleFactor, scaleFactor, 1);

    glColor3f(1,0,0); // Red

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
    
    if (isPlayerInvincible) {
        glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
    }
    else {
        glColor3f(1.0f, 1.0f, 1.0f);
    }

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
    // glutPostRedisplay();
}
void UpdateGameTime(int value) {
    if (isGameOver) {
        return;
    }

    if (remainingTime <= 0) {
        glutKeyboardFunc(NULL);
        glutKeyboardUpFunc(NULL);
        glutMouseFunc(NULL);
        glutDisplayFunc(DisplayLosingScreen);
        return;
    }

    remainingTime--;

    glutPostRedisplay();

    glutTimerFunc(1000, UpdateGameTime, value);
}
void HandlePlayerMovement(int value) {
    if (isGameOver) {
        return;
    }

    int oldPlayerX = playerX;
    int oldPlayerY = playerY;

    if (keys['w'] || keys['W']) {
        playerY += isSpeedBoostActive ? 2 * playerSpeed : playerSpeed;
        playerAngle = 0;
    }
    if (keys['s'] || keys['S']) {
        playerY -= isSpeedBoostActive ? 2 * playerSpeed : playerSpeed;
        playerAngle = 180;
    }
    if (keys['a'] || keys['A']) {
        playerX -= isSpeedBoostActive ? 2 * playerSpeed : playerSpeed;
        playerAngle = 90;
    }
    if (keys['d'] || keys['D']) {
        playerX += isSpeedBoostActive ? 2 * playerSpeed : playerSpeed;
        playerAngle = 270;
    }

    // Check for collision with goal
    if (checkCollision(playerX, playerY, STANDARD_RADIUS, goalX, goalY, STANDARD_RADIUS)) {
        isGameOver = true;
        
        glutKeyboardFunc(NULL);
        glutKeyboardUpFunc(NULL);
        glutMouseFunc(NULL);
        glutDisplayFunc(DisplayWinningScreen);
    }

    // Check for collision with rings
    for (int i = 0; i < numberOfRings; i++) {
        if (checkCollision(playerX, playerY, STANDARD_RADIUS, rings[i].x, rings[i].y, STANDARD_RADIUS)) {
            score += isScoreBoostActive ? 200 : 100;

            // Remove the ring by swapping it with the last one and decreasing the count
            rings[i] = rings[numberOfRings - 1];
            numberOfRings--;
            break;
        }
    }

    // Check for collision speed powerups
    for (int i = 0; i < numberOfSpeedPowerups; i++) {
        if (checkCollision(playerX, playerY, STANDARD_RADIUS, speedPowerups[i].x, speedPowerups[i].y, STANDARD_RADIUS)) {
            isSpeedBoostActive = true;
            glutTimerFunc(5000, EndSpeedBoost, 0);
            speedPowerups[i] = speedPowerups[numberOfSpeedPowerups - 1];
            numberOfSpeedPowerups--;
            break;
        }
    }

    // Check for collision with score powerups
    for (int i = 0; i < numberOfScorePowerups; i++) {
        if (checkCollision(playerX, playerY, STANDARD_RADIUS, scorePowerups[i].x, scorePowerups[i].y, STANDARD_RADIUS)) {
            isScoreBoostActive = true;
            glutTimerFunc(5000, EndScoreBoost, 0);
            scorePowerups[i] = scorePowerups[numberOfScorePowerups - 1];
            numberOfScorePowerups--;
            break;
        }
    }

    // Check for collision with obstacles
    for (int i = 0; i < numberOfObstacles; i++) {
        if (checkCollision(playerX, playerY, STANDARD_RADIUS, obstacles[i].x, obstacles[i].y, STANDARD_RADIUS)) {
            if (!isPlayerInvincible) {
                remainingLives--;
                isPlayerInvincible = true;
                glutTimerFunc(1000, EndInvincibility, 0);
            }
            playerX = oldPlayerX;
            playerY = oldPlayerY;
            break;
        }
    }

    // Check for collision with game boundary
    if (playerX - STANDARD_RADIUS < boundaryX ||
        playerX + STANDARD_RADIUS > boundaryX + boundaryW ||
        playerY - STANDARD_RADIUS < boundaryY ||
        playerY + STANDARD_RADIUS > boundaryY + boundaryH) {
        if (!isPlayerInvincible) {
            remainingLives--;
            isPlayerInvincible = true;
            glutTimerFunc(1000, EndInvincibility, 0);
        }
        
        playerX = oldPlayerX;
        playerY = oldPlayerY;
    }

    // Check if the player is out of lives
    if (remainingLives <= 0) {
        isGameOver = true;

        glutKeyboardFunc(NULL);
        glutKeyboardUpFunc(NULL);
        glutMouseFunc(NULL);
        glutDisplayFunc(DisplayLosingScreen);
    }

    glutPostRedisplay();

    // Handle player movement once every 1/240 seconds, or 240 times per second. (240 FPS)
    glutTimerFunc(1000 / 240, HandlePlayerMovement, value + 1); 
}
bool checkCollision(int x1, int y1, int r1, int x2, int y2, int r2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    double distance = sqrt(dx * dx + dy * dy);
    return distance < r1 + r2;
}
void EndInvincibility(int value) {
    isPlayerInvincible = false;
}
void EndSpeedBoost(int value) {
    isSpeedBoostActive = false;
}
void EndScoreBoost(int value) {
    isScoreBoostActive = false;
}
void DisplayWinningScreen() {
    glClear(GL_COLOR_BUFFER_BIT);

    glClearColor(0.0f, 0.5f, 0.0f, 1.0f);

    glColor3f(0, 1, 0);
    glPointSize(70.0);

    glRasterPos2i(350, 400);
    std::string winningText = "You win!";
    for (char c : winningText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    glFlush();
}
void DisplayLosingScreen() {
    glClear(GL_COLOR_BUFFER_BIT);

    glClearColor(0.5f, 0.0f, 0.0f, 1.0f);

    glColor3f(1, 0, 0);
    glPointSize(70.0);

    glRasterPos2i(350, 400);
    std::string winningText = "You lose :(";
    for (char c : winningText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    glFlush();
}
void AnimBackground(int value) {
    backgroundAnimState += 0.01f;
    glutPostRedisplay();
    glutTimerFunc(1000 / 60, AnimBackground, 0);
}
void Display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Background (animated with shades of dark blue to black)
    float black = 0.1f * (sin(backgroundAnimState) + 1.0f);
    glClearColor(black, black, black, 1.0f);

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
    if (isScoreBoostActive) {
        glColor3f(1, 0, 0); // Red
        glRasterPos2i(370, 770);
        for (char c : "2X") {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
    }
    glColor3f(1, 1, 1); // White
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
    drawBoundary(boundaryX, boundaryY, boundaryW, boundaryW);

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
