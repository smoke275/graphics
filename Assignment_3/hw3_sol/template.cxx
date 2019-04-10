/*********************************************************************
 *  COMP 597                                                         *
 *  Solution code for HW 3                                           *
 *  Shashwata Mandal                                                 *
 *  sum675@psu.edu                                                   *
 *                                                                   *
 *                                                                   *
 *  Description:                                                     *
 *                                                                   *
 *  This is a template code for homework 3.                          *
 *  It takes three points from users through left button click.      *
 *  Then, it draws a triangle with the three points as its vertices. *
 *                                                                   *
 *                                                                   *
 *  User interface:                                                  *
 *                                                                   *
 *  1. When it starts, its shading mode is set to WIREFRAME.         *
 *     WIREFRAME mode is already implemented for your reference.     *
 *  2. For flat shading, press 'f' to put it in FLAT shading mode.   *
 *     Then, select 3 points by clicking left mouse button.          *
 *     The program draws a triangle with the selected points         *
 *     as its vertices using the color of your choice.               *
 *  3. For Gouraud shading, press 'g' to put it in Gouraud           *
 *     shading mode. Then, select 3 points by clicking left mouse    *
 *     button. The program draws a triangle with the selected points *
 *     as its vertices, interpolating the vertex colors (red, green, *
 *     blue for first, second, third points in that order)           *
 *  4. Press 'w' to go back to WIREFRAME mode.                       *
 *  5. To quit the program, press 'q'.                               *
 *  4. Any other keys that are not used to switch shading modes      *
 *     will put the shading mode to WIREFRAME.                       *
 *********************************************************************/


#include <GL/glut.h>
#include <bits/stdc++.h>

#define USE_GL_LINES 0


using std::cin;
using std::cerr;
using std::endl;


// callbacks for glut (see main() for what they do)
void reshape(int w, int h);
void display();
void mouse(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);




// Simple structure for a point
struct Point
{
    int x;
    int y;
    Point() : x(-1), y(-1) {}
    Point(int x, int y) : x(x), y(y) {}
};

struct Color
{
    float r;
    float g;
    float b;

    Color() : r(0), g(0), b(0) {}
    Color(float r, float g, float b) : r(r), g(g), b(b) {}
};


// helpers
void init();
void addPoint(int x, int y);
void keyboard_input();
void draw_point(int x, int y, Color c);
void draw_line(int x0, int y0, int x1, int y1, Color c);


void draw_triangle();
void triangle_wireframe(Color color);
void triangle_flat(Color color);
void midpoint_line(Point p0, Point p1, Color c1, Color c2);
void triangle_gouraud(Color color0, Color c1, Color c2);


// Keeps track of current shading mode.
enum ShadingMode { WIREFRAME, FLAT, GOURAUD };
ShadingMode shading_mode = WIREFRAME;



// Initial window size
int win_w = 512;
int win_h = 512;


// For triangles, 3 points will do.
Point points[3];

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




/*****************
 *   callbacks   *
 *****************/


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
    draw_triangle();

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
        case 'f':  // flat shading
            shading_mode = FLAT;
            break;
        case 'g':  // gouraud shading
            shading_mode = GOURAUD;
            break;
        case 'k':  // for grading purpose only--do not modify
            keyboard_input();
            num_points = 0;
            break;
        default:
            shading_mode = WIREFRAME;
            break;
    }
}




/**************
 *   helpers  *
 **************/


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



// add the point just selected by mouse button
void addPoint(int x, int y)
{
    points[num_points++] = Point(x, y);
    if (num_points == 3) {
        // we have 3 points now, so we can draw a triangle

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
}



// for grading purpose only
// do not modify this function
// do not use this function
//   -- it's not tested fully and does not work correctly
void keyboard_input()
{
    int x, y;
    num_points = 0;
    for (int i=0; i<3; i++) {
        cerr << "Enter point " << i << " => ";
        cin >> x >> y;
        cerr << endl;
        addPoint(x, y);
    }
}


void draw_point(int x, int y, Color c)
{
    glBegin(GL_POINTS);
    {
        glColor3f(c.r, c.g, c.b);
        glVertex2d(x, win_h-y);
    }
    glEnd();
}


void draw_line(int x0, int y0, int x1, int y1, Color c)
{
    glBegin(GL_LINES);
    glColor3f(c.r, c.g, c.b);
    glVertex2d(x0, win_h - y0);
    glVertex2d(x1, win_h - y1);
    glEnd();
}



// a helper function to draw a line
// you need to modify this function for midpoint algorithm
// anything you draw here goes to back buffer
void draw_triangle()
{
    // now, draw a line between 2 end points
    // you need to draw a line using incremental midpoint algorithm
    //     without any floating point calculations, so remove
    //     next 4 lines and implement incremental midpoint algorithm here.
    //
    // you cannot use GL_LINES for this assignment.
    // you must use GL_POINTS instead.
    //
    // To draw a pixel (x,y) with the color you set above, use
    //         glBegin(GL_POINTS);
    //            glVertex2d(x, y);
    //         glEnd();
    // Any number of glVertex2d() may appear between glBegin() and glEnd()
    // Or, even glColor3f() may appear there, if you want o change the color
    //      of each point.
    // Also, any c++ code can appear between glBegin() and glEnd()
    //
    // Note: GL provides commands for drawing.
    //       glut provides gui, including window on which you are drawing.
    //       The origin (0,0) of the coordinate system is
    //          at bottom left in GL and
    //          at top left in glut.
    //       You must compensate the difference between the coordinate
    //       systems used by GL and glut, when drawing.

    switch (shading_mode) {
        case WIREFRAME:
        {
            // choose the color for wireframe
            Color color(0.129, 0.588, 0.952);
            // draw a triangle as wireframe
            // using draw_line()
            triangle_wireframe(color);
            break;
        }
        case FLAT:
        {
            // choose the color for flat shading
            Color color(0.960, 0.498, 0.090);
            // HERE, draw a triangle with flat shading
            //       using draw_point()
            triangle_flat(color);
            break;
        }
        case GOURAUD:
        {
            // choose the vertex colors for gouraud shading
            Color c0(1.0, 0.0, 0.0);
            Color c1(0.0, 1.0, 0.0);
            Color c2(0.0, 0.0, 1.0);
            // HERE, draw a triangle with gouraud shading
            //       using draw_point()
            triangle_gouraud(c0,c1,c2);
            break;
        }
    }
}


void triangle_wireframe(Color color)
{
    // not much to do.
    // just draw 3 lines using the 3 points
    for (int i=0; i<3; i++) {
        int x0 = points[i].x, y0 = points[i].y;
        int x1 = points[(i+1)%3].x, y1 = points[(i+1)%3].y;
        draw_line(x0, y0, x1, y1, color);
    }
}

void triangle_flat(Color color)
{
    /*
     * This method is used to generate a flat triangle
     * This also fixes the problem of creating less points that required in
     * case two points are nearby and one point is far away. It doesn't use
     * GL lines but uses the line drawing algorithm from last assignment. To
     * use this feature set USE_GL_LINES to 1
     */
    // not much to do.
    // just draw 3 lines using the 3 points
    int x_min = INT_MAX, x_max = INT_MIN;
    int y_min = INT_MAX, y_max = INT_MIN;

    x_min = std::min(points[0].x, std::min(points[1].x, points[2].x));
    x_max = std::max(points[0].x, std::max(points[1].x, points[2].x));
    y_min = std::min(points[0].y, std::min(points[1].y, points[2].y));
    y_max = std::max(points[0].y, std::max(points[1].y, points[2].y));



    int a1 = points[1].x - points[0].x,
            b1 = points[2].x - points[0].x;
    int a2 = points[1].y - points[0].y,
            b2 = points[2].y - points[0].y;

    int den = a1*b2 - a2*b1;
    int multliplier = 1;

    if(den<0)
        multliplier = -1;

    den*=multliplier;


    if(USE_GL_LINES){
        midpoint_line(points[0], points[1], color, color);
        midpoint_line(points[1], points[2], color, color);
        midpoint_line(points[0], points[2], color, color);
    }


    for(int x = x_min; x <= x_max; x++){
        for(int y = y_min; y <= y_max; y++){


            if(den!=0){
                int c1 = x - points[0].x, c2 = y - points[0].y;
                int beta = (b2*c1 - b1*c2)*multliplier;
                int gamma = (a1*c2 - a2*c1)*multliplier;
                int alpha = den - beta - gamma;



                if(beta>=0 && gamma>=0 && alpha>=0){
                    draw_point(x, y, color);
                }
            }
        }
    }





}

void triangle_gouraud(Color color0, Color color1, Color color2) {
    /*
     * This method is used to generate a gouraud triangle
     * This also fixes the problem of creating less points that required in
     * case two points are nearby and one point is far away. It doesn't use
     * GL lines but uses the line drawing algorithm from last assignment. To
     * use this feature set USE_GL_LINES to 1
     */
    // just draw 3 lines using the 3 points
    int x_min = INT_MAX, x_max = INT_MIN;
    int y_min = INT_MAX, y_max = INT_MIN;

    x_min = std::min(points[0].x, std::min(points[1].x, points[2].x));
    x_max = std::max(points[0].x, std::max(points[1].x, points[2].x));
    y_min = std::min(points[0].y, std::min(points[1].y, points[2].y));
    y_max = std::max(points[0].y, std::max(points[1].y, points[2].y));


    int a1 = points[1].x - points[0].x,
            b1 = points[2].x - points[0].x;
    int a2 = points[1].y - points[0].y,
            b2 = points[2].y - points[0].y;

    int den = a1 * b2 - a2 * b1;

    int multliplier = 1;

    if(den<0)
        multliplier = -1;

    den*=multliplier;

    if(USE_GL_LINES){//
        midpoint_line(points[0], points[1], color0, color1);
        midpoint_line(points[1], points[2], color1, color2);
        midpoint_line(points[0], points[2], color0, color2);
    }


    for (int x = x_min; x <= x_max; x++) {
        for (int y = y_min; y <= y_max; y++) {

            if (den != 0) {
                int c1 = x - points[0].x, c2 = y - points[0].y;
                int beta = (b2*c1 - b1*c2)*multliplier;
                int gamma = (a1*c2 - a2*c1)*multliplier;
                int alpha = den - beta - gamma;


                if (beta >= 0 && gamma >= 0 && alpha >= 0) {
                    Color color((color0.r * alpha + color1.r * beta + color2.r * gamma) / (double)den,
                                (color0.g * alpha + color1.g * beta + color2.g * gamma) / (double)den,
                                (color0.b * alpha + color1.b * beta + color2.b * gamma) / (double)den);
                    draw_point(x, y, color);
                }
            }

        }
    }




}

void midpoint_line(Point P0, Point P1, Color c0, Color c1) {
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
    int shading = 1;
    if(c0.r==c1.r && c0.g==c1.g && c0.b==c1.b)
        shading = 0;

    /* The line is draw each time you exit the window.
     * Hence its better not to change the original
     * points
     */

    if(P0.y - P1.y<0) {
        //swap P0 and P1 to follow clause 2 mentioned before
        std::swap(P1.x,P0.x);
        std::swap(P1.y,P0.y);
        Color temp = c0;
        c0 = c1;
        c1 = temp;
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



        Color color;

        if(shading) {
            double beta = (double)x/(double)endpoint.x;
            double alpha = 1.0 - beta;
            color = Color((c0.r * alpha + c1.r * beta),
                        (c0.g * alpha + c1.g * beta),
                        (c0.b * alpha + c1.b * beta));
        } else{
            color = c1;
        }

        draw_point(origin.x + x1, win_h - (origin.y + y1), color);

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

}


