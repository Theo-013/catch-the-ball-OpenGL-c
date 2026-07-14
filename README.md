# Catch The Ball

A fast-paced 2D arcade game built with C and OpenGL/GLUT, where the player 
controls a basket to catch falling balls, racing against a countdown timer 
that shrinks or grows based on which balls are caught.

## Features
- Real-time falling-ball physics with randomized spawn position, fall speed, and color
- Three distinct ball types with different gameplay effects:
  - **Yellow** → +1 point and +0.2s to the timer
  - **Pink** → −0.3s penalty to the timer
  - **Red** → instant Game Over
- Countdown timer with decimal precision display
- Basket movement controlled via left/right arrow keys, with screen-edge collision limits
- Full game state machine: Menu → Running → Game Over / Time Over → Restart
- On-screen legend explaining ball colors and effects during gameplay
- Delta-time based updates for frame-rate independent motion
- Mouse-click driven start/restart flow


## Tech Stack
- **Language:** C
- **Graphics:** OpenGL, GLUT

## How to Build & Run

### Linux
```bash
sudo apt-get install freeglut3-dev
gcc main.c -o catch_the_ball -lGL -lGLU -lglut -lm
./catch_the_ball
```

### Windows (MinGW)
```bash
gcc main.c -o catch_the_ball.exe -lfreeglut -lopengl32 -lglu32
catch_the_ball.exe
```

## Controls
| Input       | Action                          |
|-------------|----------------------------------|
| ← / →       | Move basket left / right         |
| Left Click  | Start game / Restart after game ends |
| Esc         | Quit                              |

## Technical Highlights
- Custom game state machine (`MENU`, `RUNNING`, `OVER`, `TIME_OVER`) driving 
  rendering and input logic
- Ball pool managed with a fixed-size array and swap-and-pop removal for 
  efficient deletion of off-screen or caught balls
- Circle rendering via manual trigonometric vertex generation (`GL_POLYGON`)
- Collision detection between falling balls and the basket using axis-aligned 
  bounding checks
- Floating text feedback (e.g. "+1", "+0.2s", "-0.3s") rendered briefly at the 
  catch location using a per-ball timer
- Frame-independent movement and timers using delta time computed from 
  `glutGet(GLUT_ELAPSED_TIME)`
