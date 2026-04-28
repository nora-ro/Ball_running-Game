#include <GL/glut.h>
#include <iostream>

// --- Global Variables ---
float ballX = 0.0f, ballY = 0.5f, ballZ = 5.0f;
bool isFullScreen = true;

// --- Function Prototypes ---
void drawTrack();
void drawBall();

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
    setupLighting();
}

// --- Function: drawTrack ---
// Dedicated function to render the space track.
void drawTrack() {
    glDisable(GL_LIGHTING);
    glColor3f(0.1f, 0.1f, 0.3f);
    glBegin(GL_QUADS);
    glVertex3f(-2.5, 0.0, 15.0);
    glVertex3f(2.5, 0.0, 15.0);
    glVertex3f(2.5, 0.0, -100.0);
    glVertex3f(-2.5, 0.0, -100.0);
    glEnd();
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

void mydraw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(0.0, 3.5, 12.0,
        0.0, 0.0, 0.0,
        0.0, 1.0, 0.0);

    // Calling modular drawing functions
    drawTrack();
    drawBall();

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
    }
}

void timer(int v) {
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
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}