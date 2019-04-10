/*********************************************************************
 *  COMP 597                                                         *
 *  Solution code for HW 2                                           *
 *  February 6, 2019                                                 *
 *  Shashwata Mandal                                                 *
 *  sum675@psu.edu                                                   *
 *                                                                   *
 *                                                                   *
 *  Description:                                                     *
 *                                                                   *
 *  This is a solution code for homework 2.                          *
 *  It takes two points from users through left button clicks.       *
 *  Then, it draws a line between the two points using               *
 *  Midpoint Algorithm. It does the same for circle as well.         *
 *                                                                   *
 *                                                                   *
 *  User interface:                                                  *
 *                                                                   *
 *  1. When it starts, its drawing mode is set to NONE.              *
 *     To draw a line, press 'l' to put it in LINE drawing mode.     *
 *     Then, select two points by clicking left mouse button         *
 *     The program draws a line between the two points.              *
 *     To draw a line, press 'c' to put it in CIRCLE drawing mode.   *
 *     Then, select two points by clicking left mouse button         *
 *     The program draws a circle between the two points.            *
 *  2. You can also input the points using keyboard.                 *
 *     To do this, press 'k' and type the coordinates                *
 *     on the terminal (NOTE: This feature is for grading purpose    *
 *     only and has not been tested extensively)                     *
 *  3. To quit the program, press 'q'.                               *
 *  4. Any other keys that are not used to switch drawing modes      *
 *     will put the drawing mode to NONE                             *
 *********************************************************************/


#include <GL/glut.h>
#include <bits/stdc++.h>


using std::cin;
using std::cerr;
using std::endl;

// callbacks for glut (see main() for what they do)
void reshape(int w, int h);
void display();
void mouse(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);


// helpers
void init();
void addPoint(int x, int y);
void keyboard_input();
void midpoint_line();
void midpoint_circle();



// Simple structure for a point
struct Point
{
    int x;
    int y;
    Point() : x(-1), y(-1) {}
    Point(int x, int y) : x(x), y(y) {}
};


// Keeps track of what I am drawing currently.
enum DrawingMode { NONE, LINE, CIRCLE};
DrawingMode drawing_mode = NONE;



// Initial window size
int win_w = 512;
int win_h = 512;


// For lines, 2 points will do.
Point points[2];

// Used to keep track of how many points I have so far
int num_points;



// OpenGL/glut programs typically have the structure shown here,
// although with different args and callbacks.
//
// You should not need to modify main().
// If you want to modify it, do it at your own risk.
//
// For complete description of each glut functions used, see
// glut manual page.
int main(int argc, char* argv[])
{
    // initialize glut
    glutInit(&argc, argv);

    // use double buffering with RGB colors
    // double buffer removes most of the flickering
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    // set window size and position
    glutInitWindowSize(win_w, win_h);
    glutInitWindowPosition(100, 100);

    // now, create window with title "Scan Conversion"
    glutCreateWindow("Scan Conversion");


    // other stuffs like background color, viewing, etc will be
    // set up in this function.
    init();


    // register callbacks for glut
    glutDisplayFunc(display);   // for display
    glutReshapeFunc(reshape);   // for window move/resize
    glutMouseFunc(mouse);       // for mouse buttons
    glutKeyboardFunc(keyboard); // for keyboard


    // start event processing, i.e., accept user inputs
    glutMainLoop();

    return 0;
}



void init()
{
    // set background color to black
    glClearColor(0.0, 0.0, 0.0, 0.0);

    // create viewing volume
    // -- will use orthogonal projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, win_w-1, 0.0, win_h-1, -1.0, 1.0);

}



// called when the window is resized/moved (plus some other cases)
void reshape(int w, int h)
{
    win_w = w;
    win_h = h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, win_w-1, 0.0, win_h-1, -1.0, 1.0);

    glViewport(0, 0, win_w, win_h);
}


// called when the window needs to be redrawn
void display()
{
    // buffer is a raster array provided by OpenGL
    // recall that we are using 2 buffers (double buffering)
    // they are called front buffer and back buffer
    // what you see on the screen is the content of front buffer
    // what you draw is drawn only on back buffer


    // clear back buffer with background color that is set in init()
    glClear(GL_COLOR_BUFFER_BIT);

    // now, draw on back buffer just cleared
    switch (drawing_mode) {
        case LINE:
            midpoint_line();
            break;
        case CIRCLE:
            midpoint_circle();
            break;
        default:
            break;
    }

    // swap the buffers.
    // - all the drawing is done on the back buffer
    // - once drawing is done on the back buffer, 
    //       you need to display the content of the back buffer.
    // - swapping buffers means swapping back buffer with front buffer
    //       so that front buffer becomes back buffer and
    //       back buffer becomes front buffer.
    // - once back buffer becomes front buffer, the content of it will be
    //       displayed on the screen, so you can see it.
    glutSwapBuffers();
}


// called when a mouse event (button pressed/released/moved/dragged) occurs
// in glut, 
//     mouse buttons are represented as
//           GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, and GLUT_RIGHT_BUTTON
//     status of mouse buttons are represented as
//           GLUT_UP and GLUT_DOWN
//     (x, y) is the mouse position when the event occurred
void mouse(int button, int state, int x, int y)
{
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN)
                addPoint(x, y);
            break;
        default:
            break;
    }
}


// called when a keyboard event (key typed) occurs
void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 'q':  // quit the program
            exit(0);
        case 'l':  // draw a line
            drawing_mode = LINE;
            num_points = 0;
            break;
        case 'c':  // draw a circle
            drawing_mode = CIRCLE;
            num_points = 0;
            break;
        case 'k':  // for grading purpose only--do not modify
            keyboard_input();
            num_points = 0;
            break;
        default:
            drawing_mode = NONE;
            break;
    }
}


// add the point just selected by mouse button
void addPoint(int x, int y)
{
    switch (drawing_mode) {
        case LINE:  // save the points until we have 2 points
            points[num_points++] = Point(x, y);
            if (num_points == 2) {
                // we have 2 points now, so we can draw a line

                // reset the num_points to 0 for next line
                num_points = 0;

                // tell glut that the current window needs to be redisplayed.
                // glut will then redisplay the current window.
                // this means display() callback will be called.
                // display() in turn will draw a midpoint line on back buffer
                //   and swap the back buffer with the front buffer
                // by swapping the buffers, the back buffer becomes visible,
                //   ie, displayed on the window
                glutPostRedisplay();
            }
            break;
        case CIRCLE:  // save the points until we have 2 points
            points[num_points++] = Point(x, y);
            if (num_points == 2) {
                // we have 2 points now, so we can draw a line

                // reset the num_points to 0 for next line
                num_points = 0;

                // tell glut that the current window needs to be redisplayed.
                // glut will then redisplay the current window.
                // this means display() callback will be called.
                // display() in turn will draw a midpoint line on back buffer
                //   and swap the back buffer with the front buffer
                // by swapping the buffers, the back buffer becomes visible,
                //   ie, displayed on the window
                glutPostRedisplay();
            }
            break;
        default:
            break;
    }
}


// for grading purpose only
// do not modify this function
// do not use this function
//   -- it's not tested fully and does not work correctly
void keyboard_input()
{
    if (drawing_mode == NONE) {
        cerr << "Select drawing mode first...\n";
        return;
    }

    int x, y;
    num_points = 0;
    for (int i=0; i<2; i++) {
        cerr << "Enter point " << i << " => ";
        cin >> x >> y;
        cerr << endl;
        addPoint(x, y);
    }
}


// a helper function to draw a line
// you need to modify this function for midpoint algorithm
// anything you draw here goes to back buffer

void midpoint_line()
{
    //     __ _                     _     _             _       _
    //    / /(_)_ __   ___    /\/\ (_) __| |_ __   ___ (_)_ __ | |_
    //   / / | | '_ \ / _ \  /    \| |/ _` | '_ \ / _ \| | '_ \| __|
    //  / /__| | | | |  __/ / /\/\ \ | (_| | |_) | (_) | | | | | |_
    //  \____/_|_| |_|\___| \/    \/_|\__,_| .__/ \___/|_|_| |_|\__|
    //                                     |_|

    /*
     * 1_//_ A line can be drawn by making sure it works from m<1
     * in the first quadrant. If this works, every other line
     * can be drawn by a simple reflection.
     *
     * 2_//_ A line drawn in the third and fourth quadrants can
     * easily be converted to a lines drawn in quadrant one
     * and quadrant two.
     */

    // select a line color of your choice
    glColor3f(0.129, 0.588, 0.952); // we choose red for line

    /* The line is draw each time you exit the window.
     * Hence its better not to change the original
     * points
     */
    Point P0 = Point(points[0].x, points[0].y);
    Point P1 = Point(points[1].x, points[1].y);

    if(P0.y - P1.y<0) {
        //swap P0 and P1 to follow clause 2 mentioned before
        std::swap(P1.x,P0.x);
        std::swap(P1.y,P0.y);
    }


    Point origin = Point(P0.x, win_h - P0.y);
    Point endpoint = Point( P1.x - P0.x, P0.y - P1.y);
    /*
     * Here we shift axes to make P0 as origin
     */

    int reflect_y_flag = 0;
    if(endpoint.x<0){
        /* We check if P1.x is in the second quadrant
         * if it is we would have to have a -x for
         * each point. Hence we set the flag
         */
        reflect_y_flag = 1;
        endpoint.x = -endpoint.x;
    }

    int reflect_x_eq_y_flag = 0;
    if(endpoint.y>endpoint.x){
        /* By this time all the value should be in the first
         * quadrant. Hence in order to draw values of line
         * for m>1, we need more rise than rolls. Hence
         * we swap the x and y coordinates
         */
        reflect_x_eq_y_flag = 1;
        std::swap(endpoint.x, endpoint.y);
    }


    int d = -2*endpoint.y + endpoint.x; // setting the base value of d

    int precalc_2xendpoint_y = -2*endpoint.y; // precalculating the values
    int precalc_2xendpoint_x_min_2xendpoint_y
            = 2*(endpoint.x - endpoint.y);

    int flag_m_eq_0 = 0;
    if(endpoint.x==0)  // checking if the slope is x
        flag_m_eq_0 = 1;


    glBegin(GL_POINTS);

    int y = 0;

    for(int x = 0; x <= endpoint.x; x++){
        /* starting the midpoint algorithm
         *
         */
        int x1 = x, y1 = y;
        if(reflect_x_eq_y_flag){
            x1 = y; // reflecting points along y=x
            y1 = x;
        }

        if(reflect_y_flag)
            x1 = -x1; //reflecting points along y=0

        glVertex2d(origin.x + x1, origin.y + y1);

        //end of displaying point
        if(!flag_m_eq_0){
            if(d<0){
                d = d + precalc_2xendpoint_x_min_2xendpoint_y;
                y++;
            } else{
                d = d + precalc_2xendpoint_y;
            }
        }
    }
    glEnd();

}

void midpoint_circle()
{
    //   _____ _          _       ___  ____     _             _       _
    //  /  __ (_)        | |      |  \/  (_)   | |           (_)     | |
    //  | /  \/_ _ __ ___| | ___  | .  . |_  __| |_ __   ___  _ _ __ | |_
    //  | |   | | '__/ __| |/ _ \ | |\/| | |/ _` | '_ \ / _ \| | '_ \| __|
    //  | \__/\ | | | (__| |  __/ | |  | | | (_| | |_) | (_) | | | | | |_
    //   \____/_|_|  \___|_|\___| \_|  |_/_|\__,_| .__/ \___/|_|_| |_|\__|
    //                                           | |
    //                                           |_|

    /*
     * A circle can be drawn by figuring out the points from (0,R) to
     * the point on the line y=x. Once this arc is created, the
     * remaining 7 arcs are simple reflections of it.
     */


    // select a line color of your choice
    glColor3f(0.960, 0.498, 0.090); // we choose red for circle


    Point origin = Point(points[0].x, win_h - points[0].y);

    Point endpoint = Point( points[1].x - points[0].x, points[0].y - points[1].y);

    int R = round(sqrt(endpoint.x*endpoint.x + endpoint.y*endpoint.y));

    int D = 5 - 4*R;

    glBegin(GL_POINTS);

    glVertex2d(origin.x, origin.y);

    int y = R;

    for(int x = 0; x <= y; x++){

        //display and reflect the points
        glVertex2d(origin.x + x, origin.y + y);//1
        glVertex2d(origin.x + y, origin.y - x);//2
        glVertex2d(origin.x - x, origin.y - y);//3
        glVertex2d(origin.x - y, origin.y + x);//4

        if(x!=y && x != 0) {
            glVertex2d(origin.x + y, origin.y + x);//1
            glVertex2d(origin.x - x, origin.y + y);//2
            glVertex2d(origin.x - y, origin.y - x);//3
            glVertex2d(origin.x + x, origin.y - y);//4
        }
        //end of displaying point

        if(D < 0){
            D = D + 8*x + 12;
        } else{
            D = D + 8*(x - y) + 20;
            y--;
        }
    }
    glEnd();

}
      
