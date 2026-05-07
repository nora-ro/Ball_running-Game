#define STB_IMAGE_IMPLEMENTATION
#include <GL/stb_image.h>
#include <GL/glut.h>
#include <iostream>
#include <string>
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")


// --- Global Variables ---
float ballX = 0.0f, ballY = 0.5f, ballZ = 5.0f;

int currentLane = 1; // 0 = left, 1 = center, 2 = right
float lanes[3] = { -2.0f, 0.0f, 2.0f };
bool isFullScreen = true;
float ballRotation = 0.0f;

// --- jump variables ---
bool jumping = false;
bool falling = false;
float groundY = 0.5f;
float jumpHeight = 3.5f;

// --- for Texture ---
unsigned char* data = NULL;
unsigned int backTexture, TrackTexture, FLineTexture;
int width, height, nrChannel;

// --- collision variables ---
bool gameStarted = false;
bool gameWon = false;
float ballRadius = 0.6f;
float finishZ = -99.5f;
bool gameOver = false;

// --- obstacle balls ---
const int obstacleCount = 6;
float obstacleZ[obstacleCount] = { -10.0f, -25.0f, -40.0f, -55.0f, -70.0f, -85.0f };
int obstacleLane[obstacleCount] = { 0, 2, 1, 0, 2, 1 };
float obstacleRadius = 0.35f;

float obsColors[6][3] = {
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {1.0f, 1.0f, 0.0f},
    {1.0f, 0.0f, 1.0f},
    {0.0f, 1.0f, 1.0f},
    {1.0f, 0.5f, 0.0f}
};

// --- Score Variables ---
int score = 0;
bool obstaclePassed[obstacleCount] = { false, false, false, false, false, false };

// --- sound variables ---
bool winSoundPlayed = false;
bool loseSoundPlayed = false;

int windowWidth = 1280;

// --- coin variables ---
const int coinCount = 18;

float coinZ[coinCount] = {
    -8,-12,-16,
    -22,-26,-30,
    -36,-40,-44,
    -50,-54,-58,
    -64,-68,-72,
    -78,-82,-86
};

int coinLane[coinCount] = {
    0,1,2,
    1,2,0,
    2,0,1,
    0,2,1,
    1,0,2,
    2,1,0
};

bool coinCollected[coinCount] = {
    false,false,false,false,false,false,
    false,false,false,false,false,false,
    false,false,false,false,false,false
};
float coinRadius = 0.12f;


// --- Function Prototypes ---
void Check(unsigned int& texture, unsigned char* imgnum);
void loadTextures();
void drawBackground();
void drawTrack();
void drawFinishLine();
void drawBall();
void drawObstacles();
void drawScore();
void collision();
void resetGame();
void mouseMove(int x, int y);
void mouseClick(int button, int state, int x, int y);
void drawCoins();


// --- Function: setupLighting ---
void setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    GLfloat light_pos[] = { 0.0f, 10.0f, 5.0f, 1.0f };
    GLfloat ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
}


void init() {
    glClearColor(0.02f, 0.02f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    loadTextures();
    setupLighting();
}


void Check(unsigned int& imgnum, unsigned char* data) {
    if (data) {
        glGenTextures(1, &imgnum);
        glBindTexture(GL_TEXTURE_2D, imgnum);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGB,
            width,
            height,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            data
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        std::cout << "failed to load texture!" << std::endl;
    }

    stbi_image_free(data);
}


void loadTextures() {
    // load the background texture
    data = stbi_load("Background.jpg", &width, &height, &nrChannel, 0);
    if (data) {
        Check(backTexture, data);
    }
    else {
        std::cout << "failed to load background Texture" << std::endl;
    }

    // load the track texture
    data = stbi_load("Track.jpg", &width, &height, &nrChannel, 0);
    if (data) {
        Check(TrackTexture, data);
    }
    else {
        std::cout << "failed to load Track Texture" << std::endl;
    }

    // load the FinishLine texture
    data = stbi_load("race-finish.jpg", &width, &height, &nrChannel, 0);
    if (data) {
        Check(FLineTexture, data);
    }
    else {
        std::cout << "failed to load Finish line Texture" << std::endl;
    }
}


void drawBackground() {
    // Switch to orthographic projection to render a full-screen 2D background
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1, 0, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, backTexture);

    glColor3f(1, 1, 1);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1);
    glVertex2f(0, 0);

    glTexCoord2f(1, 1);
    glVertex2f(1, 0);

    glTexCoord2f(1, 0);
    glVertex2f(1, 1);

    glTexCoord2f(0, 0);
    glVertex2f(0, 1);
    glEnd();

    // Return the 3D environment
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
}


// --- Function: drawTrack ---
void drawTrack() {
    glDisable(GL_LIGHTING);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, TrackTexture);

    glColor3f(1, 1, 1);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(-2.5, 0.0, 15.0);

    glTexCoord2f(1, 0);
    glVertex3f(2.5, 0.0, 15.0);

    glTexCoord2f(1, 1);
    glVertex3f(2.5, 0.0, -100.0);

    glTexCoord2f(0, 1);
    glVertex3f(-2.5, 0.0, -100.0);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glEnable(GL_LIGHTING);
}


void drawFinishLine() {
    glDisable(GL_LIGHTING);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, FLineTexture);

    glColor3f(1, 1, 1);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(-2.5, 0.1, -96.0);

    glTexCoord2f(5, 0);
    glVertex3f(2.5, 0.1, -96.0);

    glTexCoord2f(5, 1);
    glVertex3f(2.5, 0.1, -98.5);

    glTexCoord2f(0, 1);
    glVertex3f(-2.5, 0.1, -98.5);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glEnable(GL_LIGHTING);
}


// --- Function: drawBall ---
void drawBall() {
    glColor3f(1.0f, 0.0f, 0.6f);

    glPushMatrix();
    glTranslatef(ballX, ballY, ballZ);
    glRotatef(ballRotation, 1.0f, 0.0f, 0.0f);
    glutWireSphere(0.6, 20, 20);
    glPopMatrix();
}


void drawObstacles() {
    for (int i = 0; i < obstacleCount; i++) {
        glColor3fv(obsColors[i]);

        glPushMatrix();
        glTranslatef(lanes[obstacleLane[i]], 0.5f, obstacleZ[i]);
        glutWireSphere(obstacleRadius, 20, 20);
        glPopMatrix();
    }
}

void drawCoins() {
    for (int i = 0; i < coinCount; i++) {
        if (!coinCollected[i]) {
            glColor3f(1.0f, 0.84f, 0.0f);

            glPushMatrix();
            glTranslatef(lanes[coinLane[i]], 1.0f, coinZ[i]);

            // rotate coin
            glRotatef(ballRotation * 2, 0, 1, 0);

            glutSolidTorus(0.03, 0.12, 12, 20);
            glPopMatrix();
        }
    }
}
// --- Function: drawScore ---
void drawScore() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    gluOrtho2D(0, 1280, 0, 720);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    glColor3f(1.0f, 1.0f, 1.0f);

    std::string text = "Score: " + std::to_string(score);

    glRasterPos2f(30, 680);

    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
}


void mydraw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawBackground();

    glLoadIdentity();

    gluLookAt(
        ballX, 3.5f, ballZ + 7.0f,   // camera position behind ball
        ballX, 0.0f, ballZ,          // look at ball
        0.0f, 1.0f, 0.0f
    );

    drawTrack();
    drawFinishLine();
    drawObstacles();
    drawCoins();
    drawBall();

    collision();
    
    drawScore();

   
    glutSwapBuffers();
}


void reshape(int w, int h) {
    if (h == 0) h = 1;

    windowWidth = w;

    float ratio = w / (float)h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0, ratio, 1.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
}


// collision function
void collision() {
    // finish line
    if (ballZ - ballRadius <= finishZ + 0.4f && !gameWon) {
        gameWon = true;

        if (!winSoundPlayed) {
            PlaySound(TEXT("WinGame.wav"), NULL, SND_FILENAME | SND_ASYNC);
            winSoundPlayed = true;
        }
    }

    // obstacle collision
    for (int i = 0; i < obstacleCount; i++) {
        bool sameLane = (currentLane == obstacleLane[i]);
        bool closeZ = abs(ballZ - obstacleZ[i]) < 1.0f;
        bool notJumpingOver = ballY < 1.4f;

        if (sameLane && closeZ && notJumpingOver && !gameOver) {
            gameOver = true;

            std::cout << "GAME OVER!" << std::endl;

            if (!loseSoundPlayed) {
                PlaySound(TEXT("gameover.wav"), NULL, SND_FILENAME | SND_ASYNC);
                loseSoundPlayed = true;
            }
        }
    }
    // coin collection
    for (int i = 0; i < coinCount; i++) {
        bool sameLane = (currentLane == coinLane[i]);
        bool closeZ = abs(ballZ - coinZ[i]) < 0.5f;
        bool closeY = abs(ballY - 1.0f) < 1.0f;

        if (sameLane && closeZ && closeY && !coinCollected[i]) {
            coinCollected[i] = true;
            score += 5;

            std::cout << "Coin collected! Score: " << score << std::endl;
        }
    }
}


void resetGame() {
    ballX = 0.0f;
    ballY = 0.5f;
    ballZ = 5.0f;

    currentLane = 1;

    jumping = false;
    falling = false;

    gameStarted = false;
    gameOver = false;
    gameWon = false;

    winSoundPlayed = false;
    loseSoundPlayed = false;

    score = 0;

    for (int i = 0; i < obstacleCount; i++) {
        obstaclePassed[i] = false;
    }
    for (int i = 0; i < coinCount; i++) {
        coinCollected[i] = false;
    }
    std::cout << "Game Reset!" << std::endl;
}


// --- Function: keyboard ---
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'q':
    case 'Q':
        if (isFullScreen) {
            glutReshapeWindow(1280, 720);
            glutPositionWindow(100, 100);
            isFullScreen = false;
        }
        break;

    case 'r':
    case 'R':
        resetGame();
        break;

    case 27: // ESC key
        exit(0);
        break;

    case 13: // Enter key
        if (!jumping)
            jumping = true;
        break;

    case 32: // Space
        if (!jumping) {
            jumping = true;
            PlaySound(TEXT("JumpBall.wav"), NULL, SND_FILENAME | SND_ASYNC);
        }

        if (!gameStarted)
            gameStarted = true;

        break;
    }

    glutPostRedisplay();
}


void specialKeyboard(int key, int x, int y) {
    if (gameWon || gameOver) return;

    gameStarted = true;

    switch (key) {
    case GLUT_KEY_LEFT:
        if (currentLane > 0)
            currentLane--;
        break;

    case GLUT_KEY_RIGHT:
        if (currentLane < 2)
            currentLane++;
        break;

    case GLUT_KEY_UP:
        ballZ -= 0.5f;
        break;

    case GLUT_KEY_DOWN:
        ballZ += 0.5f;
        break;
    }

    // move ball instantly to lane
    ballX = lanes[currentLane];

    glutPostRedisplay();
}


void timer(int v) {
    if (!gameWon && !gameOver && gameStarted) {
        ballZ -= 0.2f;
        ballRotation += 4.0f;
        
    }

    // Jump up
    if (jumping && !falling) {
        ballY += 0.2f;

        if (ballY >= jumpHeight) {
            falling = true;
        }
    }

    // Fall down
    if (falling) {
        ballY -= 0.2f;

        if (ballY <= groundY) {
            ballY = groundY;
            jumping = false;
            falling = false;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timer, 0);
}


void mouseMove(int x, int y) {
    float normalized = (float)x / windowWidth; // 0 -> 1

    if (normalized < 0.33)
        currentLane = 0;
    else if (normalized < 0.66)
        currentLane = 1;
    else
        currentLane = 2;

    ballX = lanes[currentLane];
}


void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (!jumping) {
            jumping = true;
        }

        gameStarted = true;
    }
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);

    glutCreateWindow("Space Runner 3D");

    // Start in Full Screen mode
    glutFullScreen();

    init();

    glutDisplayFunc(mydraw);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);
    glutTimerFunc(0, timer, 0);
    glutPassiveMotionFunc(mouseMove);
    glutMouseFunc(mouseClick);

    glutMainLoop();

    return 0;
}
