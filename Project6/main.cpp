#define STB_IMAGE_IMPLEMENTATION
#include<GL/stb_image.h>
#include <GL/glut.h>
#include <iostream>

// --- Global Variables ---
float ballX = 0.0f, ballY = 0.5f, ballZ = 5.0f;
int currentLane = 1; // 0 = left, 1 = center, 2 = right
float lanes[3] = { -2.0f, 0.0f, 2.0f };
bool isFullScreen = true;
//--- jumb variables ---
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
float finishZ = -100.0f;
bool gameOver = false;
// --- obstacle balls ---
const int obstacleCount = 6;
float obstacleZ[obstacleCount] ={ -10.0f, -25.0f,-40.0f,-55.0f,-70.0f,-85.0f};
int obstacleLane[obstacleCount] ={0,2,1,0,2,1};
float obstacleRadius = 0.35f;


// --- Function Prototypes ---
void Check(unsigned int& texture, unsigned char* imgnum);
void loadTextures();
void drawBackground();
void drawTrack();
void drawFinishLine();
void drawBall();
void drawObstacles();
void collision();


// --- Function: setupLighting ---
// Configures light sources and material properties.
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    }
    else {
        std::cout << "failed to load texture!" << std::endl;
    }
    stbi_image_free(data);
}


void loadTextures() {
    //load the background texture
    data = stbi_load("Background.jpg", &width, &height, &nrChannel, 0);
    if (data) {
        Check(backTexture, data);
    }
    else {
        std::cout << "failed to load background Texture" << std::endl;
    }
    //load the track texture
    data = stbi_load("Track.jpg", &width, &height, &nrChannel, 0);
    if (data) {
        Check(TrackTexture, data);
    }
    else {
        std::cout << "failed to load Track Texture" << std::endl;
    }
    //load the FinishLine texture
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

    //Return the 3D environment
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

}

// --- Function: drawTrack ---
// Dedicated function to render the space track.
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
    glVertex3f(-2.5, 0.0, -100.0);
    glTexCoord2f(1, 0);
    glVertex3f(2.5, 0.0, -100.0);
    glTexCoord2f(1, 1);
    glVertex3f(2.5, 0.4, -100.0);
    glTexCoord2f(0, 1);
    glVertex3f(-2.5, 0.4, -100.0);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}
// --- Function: drawBall ---
// Dedicated function to render the player ball.
void drawBall() {
    glColor3f(1.0f, 0.0f, 0.6f);
    glPushMatrix();
    glTranslatef(ballX, ballY, ballZ);
    glutWireSphere(0.6, 20, 20);
    glPopMatrix();
}

void drawObstacles() {
    glColor3f(1.0f, 1.0f, 0.0f); // yellow

    for (int i = 0; i < obstacleCount; i++)
    {
        glPushMatrix();
        glTranslatef(lanes[obstacleLane[i]], 0.5f, obstacleZ[i]);
        glutSolidSphere(obstacleRadius, 20, 20);
        glPopMatrix();
    }
}


void mydraw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawBackground();
    glLoadIdentity();
    gluLookAt(
        ballX, 3.5f, ballZ + 7.0f,   // camera position (behind ball)
        ballX, 0.0f, ballZ,          // look at ball
        0.0f, 1.0f, 0.0f
    );


    // Calling modular drawing functions
    drawTrack();
    drawFinishLine();
    drawObstacles();
    drawBall();
    collision();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    float ratio = w / (float)h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, ratio, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

//collision function
void collision() {

    // finish line
    if (ballZ - ballRadius <= finishZ + 0.4f) {
        gameWon = true;
        /*std::cout << "YOU WIN!" << std::endl;*/
    }

    // obstacle collision
    for (int i = 0; i < obstacleCount; i++) {

        bool sameLane = (currentLane == obstacleLane[i]);
        bool closeZ = abs(ballZ - obstacleZ[i]) < 1.0f;
        bool notJumpingOver = ballY < 1.4f;

        if (sameLane && closeZ && notJumpingOver) {
            gameOver = true;
            std::cout << "GAME OVER!" << std::endl;
        }
    }
}

// --- Function: keyboard ---
// Handles key presses for window management and quitting.
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'q':
    case 'Q':
        if (isFullScreen) {
            glutReshapeWindow(1280, 720); // Resize to windowed mode
            glutPositionWindow(100, 100);
            isFullScreen = false;
        }
        break;
    case 27: // ESC key
        exit(0);
        break;

    case 13 :   // Enter key
        if (!jumping)
            jumping = true;
        break;

    case 32:   // space
        if (!jumping)
            jumping = true;
        break;
    }
}

void specialKeyboard(int key, int x, int y)
{
    if (gameWon || gameOver) return;

    gameStarted = true;

    switch (key)
    {
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

    glutMainLoop();
    return 0;
}
