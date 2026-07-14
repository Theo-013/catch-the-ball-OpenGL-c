#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define BASKET_WIDTH 100
#define BASKET_HEIGHT 20
#define BALL_RADIUS 16
#define GAME_TIME 25.0f

#define TIME_BONUS 0.2f
#define TIME_PENALTY 0.3f

typedef enum {
    MENU,
    RUNNING,
    OVER,
    TIME_OVER
} GameState;

typedef struct {
    float x, y;
    float dy;
    float r, g, b;  // Color
    int caught;     // if this ball was caught
    float textTimer;
    char text[20];  // Text when caught
}
Ball;

typedef struct {
    float x, y;
    float speed;
}
Basket;

Ball balls[100];
int numBalls = 0;
Basket basket;
int score = 0;
float timerSeconds = GAME_TIME;

int isMovingLeft = 0;
int isMovingRight = 0;

GameState gameState = MENU;

float lastFrameTime = 0.0f;  // Time of the last frame for delta time calculations

// Function to display text
void drawText(float x, float y, const char* text, void* font) {
    glRasterPos2f(x, y);
    for (int i = 0; text[i] != '\0'; i++) {
        glutBitmapCharacter(font, text[i]);
    }
}

// Initialize the game state
void initGame() {
    srand(time(NULL));
    basket.x = WINDOW_WIDTH / 2 - BASKET_WIDTH / 2;
    basket.y = 50;
    basket.speed = 500.0f;  // Basket speed
    numBalls = 0;
    score = 0;
    timerSeconds = GAME_TIME;
    isMovingLeft = 0;
    isMovingRight = 0;
}

// Draw the basket
void drawBasket() {
    glColor3f(0.7f, 0.7f, 0.7f);
    glRectf(basket.x, basket.y, basket.x + BASKET_WIDTH, basket.y + BASKET_HEIGHT);
}

// Draw a ball
void drawBall(Ball* ball) {
    if (!ball->caught) {
        glColor3f(ball->r, ball->g, ball->b);
        glBegin(GL_POLYGON);
        for (int i = 0; i < 360; i += 10) {
            float angle = i * M_PI / 180.0f;
            float x = ball->x + BALL_RADIUS * cos(angle);
            float y = ball->y + BALL_RADIUS * sin(angle);
            glVertex2f(x, y);
        }
        glEnd();
    } else if (ball->textTimer > 0) {
        // Adjust color based on bonus type
        if (strstr(ball->text, "+1") != NULL) {
            glColor3f(1.0f, 1.0f, 0.0f);  // Yellow for +1
        } else {
            glColor3f(1.0f, 1.0f, 1.0f);  // White for other texts
        }

        glRasterPos2f(ball->x - 15, ball->y);
        for (int i = 0; ball->text[i] != '\0'; i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ball->text[i]);
        }
    }
}

// Generate a new ball
void generateBall() {
    Ball newBall;
    newBall.x = rand() % (WINDOW_WIDTH - 2 * BALL_RADIUS) + BALL_RADIUS;
    newBall.y = WINDOW_HEIGHT;
    newBall.dy = -(rand() % 150 + 100);  // Falling speed in pixels per second
    newBall.caught = 0;
    newBall.textTimer = 0;

    int type = rand() % 100;
    if (type < 45) {
        newBall.r = 1.0f;
        newBall.g = 1.0f;
        newBall.b = 0.0f;
    } else if (type < 80) {
        newBall.r = 1.0f;
        newBall.g = 0.0f;
        newBall.b = 0.0f;
    } else {
        newBall.r = 1.0f;
        newBall.g = 0.0f;
        newBall.b = 1.0f;
    }

    balls[numBalls++] = newBall;
}

// Update the game logic
void updateGame(float deltaTime) {
    // Update basket movement
    if (isMovingLeft && basket.x > 0) {
        basket.x -= basket.speed * deltaTime;
    }
    if (isMovingRight && basket.x + BASKET_WIDTH < WINDOW_WIDTH) {
        basket.x += basket.speed * deltaTime;
    }

    // Update balls
    for (int i = 0; i < numBalls; i++) {
        if (!balls[i].caught) {
            balls[i].y += balls[i].dy * deltaTime;

            // Remove balls that fall out of the screen
            if (balls[i].y < 0) {
                balls[i] = balls[numBalls - 1];
                numBalls--;
                i--;
                continue;
            }

            // Check is Ball hits the Basket(bar)
            if (balls[i].y - BALL_RADIUS <= basket.y + BASKET_HEIGHT &&
                balls[i].y + BALL_RADIUS >= basket.y &&
                balls[i].x >= basket.x && balls[i].x <= basket.x + BASKET_WIDTH) {
                if (balls[i].r == 1.0f && balls[i].g == 0.0f && balls[i].b == 0.0f) {
                    gameState = OVER;
                    return;
                } else if (balls[i].r == 1.0f && balls[i].g == 1.0f && balls[i].b == 0.0f) {
                    score++;
                    timerSeconds += TIME_BONUS;
                    sprintf(balls[i].text, "+%.1fs", TIME_BONUS);
                } else if (balls[i].r == 1.0f && balls[i].g == 0.0f && balls[i].b == 1.0f) {
                    timerSeconds -= TIME_PENALTY;
                    sprintf(balls[i].text, "-%.1fs", TIME_PENALTY);
                }

                balls[i].caught = 1;
                balls[i].textTimer = 0.3f;
            }
        } else if (balls[i].textTimer > 0) {
            balls[i].textTimer -= deltaTime;

        } else {
            balls[i] = balls[numBalls - 1];
            numBalls--;
            i--;
        }
    }

    // Check for time end
    if (timerSeconds <= 0) {
        gameState = TIME_OVER;
    }
}

void drawCountdown(float timerSeconds) {
    int integerPart = (int)timerSeconds;
    int decimalPart = (int)((timerSeconds - integerPart) * 10);

    char timerText[20];
    sprintf(timerText, "%d.%d", integerPart, decimalPart);

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT - 20);
    for (int i = 0; timerText[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, timerText[i]);
    }
}
// Display callback

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (gameState == MENU) {
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 40, "Careful of the Red ones!", GLUT_BITMAP_HELVETICA_18);
         glColor3f(1.0f, 1.0f, 0.0f);
        drawText(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 - 170, "Click to Start", GLUT_BITMAP_HELVETICA_18);
        glColor3f(1.0f, 1.0f, 0.0f);
        drawText(10, WINDOW_HEIGHT - 595,"Yellow Balls = 1 point and + 0.2sec", GLUT_BITMAP_HELVETICA_18);
        glColor3f(1.0f, 0.0f, 1.0f);
        drawText(360, WINDOW_HEIGHT - 595,"Pink Ball = -1.0sec", GLUT_BITMAP_HELVETICA_18);
        glColor3f(1.0f, 0.0f, 0.0f);
        drawText(570, WINDOW_HEIGHT - 595,"Yellow Balls = GameOver", GLUT_BITMAP_HELVETICA_18);
    } else if (gameState == RUNNING) {
        for (int i = 0; i < numBalls; i++) {
            drawBall(&balls[i]);
        }
        drawBasket();

        // Display score and timer
        glColor3f(1.0f, 1.0f, 0.0f);
        char scoreText[20];
        sprintf(scoreText, "Score: %d", score);
        drawText(10, WINDOW_HEIGHT - 20, scoreText, GLUT_BITMAP_HELVETICA_18);
        drawCountdown(timerSeconds);
        glColor3f(1.0f, 1.0f, 0.0f);
        drawText(10, WINDOW_HEIGHT - 595,"Yellow Balls = 1 point and + 0.2sec", GLUT_BITMAP_HELVETICA_18);
        glColor3f(1.0f, 0.0f, 1.0f);
        drawText(360, WINDOW_HEIGHT - 595,"Pink Ball = -0.3sec", GLUT_BITMAP_HELVETICA_18);
        glColor3f(1.0f, 0.0f, 0.0f);
        drawText(570, WINDOW_HEIGHT - 595,"Yellow Balls = GameOver", GLUT_BITMAP_HELVETICA_18);
    } else if (gameState == OVER) {
        glColor3f(1.0f, 0.0f, 0.0f);
        drawText(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2, "GAME OVER!", GLUT_BITMAP_HELVETICA_18);
        char finalScore[30];
        glColor3f(1.0f, 1.5f, 0.0f);
        sprintf(finalScore, "Final Score: %d", score);
        drawText(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2-50, finalScore, GLUT_BITMAP_HELVETICA_18);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 - 280, "Click to Restart", GLUT_BITMAP_HELVETICA_18);
    } else if (gameState == TIME_OVER) {
        glColor3f(1.0f, 0.5f, 0.0f);
        drawText(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 40, "Time is Over!", GLUT_BITMAP_HELVETICA_18);
        char finalScore[30];
         glColor3f(1.0f, 1.5f, 0.0f);
        sprintf(finalScore, "Final Score: %d", score);
        drawText(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2, finalScore, GLUT_BITMAP_HELVETICA_18);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 - 280, "Click to Restart", GLUT_BITMAP_HELVETICA_18);
    }

    glutSwapBuffers();
}

// Timer
void timer(int value) {
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;  // Time in seconds
    float deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    if (gameState == RUNNING) {
        timerSeconds -= deltaTime;

        // Randomly generate balls
        if (rand() % 10 == 0 && numBalls < 34) {
            generateBall();
        }


        updateGame(deltaTime);
    }

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);  // 60 FPS
}

// Input
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) {
        exit(0);
    }
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (gameState == MENU || gameState == OVER || gameState == TIME_OVER) {
            gameState = RUNNING;
            initGame();
        }
    }
}

void specialInput(int key, int x, int y) {
    if (gameState == RUNNING) {
        if (key == GLUT_KEY_LEFT) {
            isMovingLeft = 1;
        } else if (key == GLUT_KEY_RIGHT) {
            isMovingRight = 1;
        }
    }
}

void specialInputUp(int key, int x, int y) {
    if (gameState == RUNNING) {
        if (key == GLUT_KEY_LEFT) {
            isMovingLeft = 0;
        } else if (key == GLUT_KEY_RIGHT) {
            isMovingRight = 0;
        }
    }
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Catch The Yellow Balls!");

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);

    glutDisplayFunc(display);
    glutTimerFunc(16, timer, 0);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);

    glutSpecialFunc(specialInput);
    glutSpecialUpFunc(specialInputUp);


    lastFrameTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    glutMainLoop();
    return 0;
}
