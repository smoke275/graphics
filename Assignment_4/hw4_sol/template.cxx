/*********************************************************************
 *  COMP 597                                                         *
 *  Solution code for HW 4                                           *
 *  Shashwata Mandal                                                 *
 *  sum675@psu.edu                                                   *
 *                                                                   *
 *                                                                   *
 *  Description:                                                     *
 *                                                                   *
 *  This is a solution code for homework 4.                          *
 *********************************************************************/



#include <GL/glut.h>
#include <glm/glm.hpp>
#include <cstdlib>
#include <iostream>
#include <cmath>

#define alpha 0.5
#define R 2.0


// typedefs
typedef glm::dvec3 Vector3;   // 3D vectors of double
typedef glm::dvec3 Point3;    // 3D points of double
typedef glm::dvec4 HPoint3;   // 3D points in Homogeneous coordinate system
typedef glm::dmat4 Matrix4;   // 4-by-4 matrix


// glut callbacks
void reshape(int w, int h);
void display();
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void keyboard(unsigned char key, int x, int y);


// helpers
void init();
void initObj();
void initCam();
void drawFaces();


// projection
void SetViewMatrix();
void SetOrthoMatrix();
void SetPerspMatrix();


// utilities for matrices and vectors
void DeviceToWorld(double u, double v, double& x, double& y);
Matrix4 Mult4(Matrix4 a, Matrix4 b);         // (4x4 matrix) . (4x4 matrix)
HPoint3 Homogenize(HPoint3 a);               // returns homogenized HPoint3
HPoint3 TransHPoint3(Matrix4 m, HPoint3 p);  // (4x4 matrix) . (4x1 Vector)


// transformations
void Rotate(double dx, double dy);
void Translate_xy(double tx, double ty);
void Translate_xz(double tx, double ty);
void Scale(double s);


// transformation helpers
Matrix4 SetScaleMatrix(double sx, double sy, double sz); // 4x4 scale matrix
Matrix4 SetTransMatrix(double tx, double ty, double tz); // 4x4 translation matrix
Matrix4 SetRotMatrix(Vector3 n, double angle);           // 4x4 rotation matrix


// default device window size
int win_w = 512;
int win_h = 512;


const double EPSILON = 0.0000001;


// for your convenience while debugging
using std::cout;
using std::cerr;
using std::endl;

void PrintMat(Matrix4 m);     // print Matrix4
void PrintHPoint(HPoint3 p);  // print HPoint3
void PrintPoint(Point3 p);    // print Point3



// for tracking mouse events
struct MouseTracker
{
    int modifiers;
    int button;
    double initx, inity;
    double finalx, finaly;
};

MouseTracker mtracker;


// for camera parameters
struct Camera
{
    bool perspective;               /* projection method */
    double l, r, b, t, n, f;        /* view volume */
    Point3 eye;                     /* eye position */
    Vector3 u, v, w;                /* eye coordinate system */
    Matrix4 Mo;                     /* orthographic projection */
    Matrix4 Mv;                     /* view matrix for arbitrary view*/
    Matrix4 Mp;                     /* perspective matrix */
};

Camera cam;


// for objects
const int MAXNAMESTRING = 20;
const int MAXVERTICES = 1000;
const int MAXEDGES = 500;
const int MAXFACES = 50;


struct Object3D {
    char name[MAXNAMESTRING];       /* The name of object for printing */
    int Nvertices;                  /* number of vertices */
    int Nfaces ;                    /* number of faces */
    Matrix4 frame;                  /* the object to world coord transform */
    Point3 center;                  /* center of mass */
    HPoint3 vertices[MAXVERTICES];  /* coodrdinates of each vertex */
    int faces[MAXFACES][6];         /* If face has N vertices, give N + 1
			  	     numbers -> first the number of vertices
			  	     in the face, then the index numbers of
				     each vertex as it appears in the 
				     "vertices"  array. */
};



// Note: We will keep the initial coordinates of the vertices
//       as originally given.  In other words, we will not change
//       the given coordinates of the vertices, even after any
//       transformation.  All the transformation will be recorded
//       in frame.  This way, you can reset the object to the
//       original position at any time, even after applying many
//       transformations.

Object3D obj = {
        "house", 10, 7,

        Matrix4(1.0), // identiy matrix (no transformations applied yet)

        // center of the object is at origin
        {0.0, 0.0, 0.0},

        // vertices of the object in no particular order
        {  HPoint3(0.0, 1.0, 2.0, 1.0),    HPoint3(-1.0, 0.5, 2.0, 1.0),
           HPoint3(-1.0, -1.0, 2.0, 1.0),  HPoint3(1.0, -1.0, 2.0, 1.0),
           HPoint3(1.0, 0.5, 2.0, 1.0),    HPoint3(0.0, 1.0, -2.0, 1.0),
           HPoint3(-1.0, 0.5, -2.0, 1.0),  HPoint3(-1.0, -1.0, -2.0, 1.0),
           HPoint3(1.0, -1.0, -2.0, 1.0),  HPoint3(1.0, 0.5, -2.0, 1.0)   },

        // faces
        {  {5,   0, 1, 2, 3, 4},
           {5,   9, 8, 7, 6, 5},
           {4,   4, 3, 8, 9},
           {4,   0, 4, 9, 5},
           {4,   1, 0, 5, 6},
           {4,   2, 1, 6, 7},
           {4,   3, 2, 7, 8}    }
};




// OpenGL/glut programs will have the structure shown here
//    although with different args and callbacks.
//
// You should not modify main().
// If you really want to modify it, do it at your own risk.
//
// For complete description of each glut functions used, see
// glut manual page on class website.

int main(int argc, char *argv[])
{
    // initialize glut
    glutInit(&argc, argv);

    // use double buffering with RGB colors
    // double buffer removes most of the flickering
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    // set window size and position
    glutInitWindowSize(win_w, win_h);
    glutInitWindowPosition(100, 100);

    // now, create window with title "Viewing"
    glutCreateWindow("Viewing");


    // other stuffs like background color, viewing, etc will be
    // set up in this function.
    init();

    // initialize (arrange) the object
    initObj();

    // initialize the camera
    initCam();


    // register callbacks for glut
    glutDisplayFunc(display);   // for display
    glutReshapeFunc(reshape);   // for window move/resize
    glutMouseFunc(mouse);       // for mouse buttons
    glutMotionFunc(motion);     // for mouse movement while mouse button pressed
    glutKeyboardFunc(keyboard); // for keyboard


    // start event processing, i.e., accept user inputs
    glutMainLoop();

    return 0;
}




/********************************************
 *     implementation for glut callbacks    *
 ********************************************/


// called when the window is resized/moved (and some other cases)
void reshape(int w, int h)
{
    // change window size
    win_w = w;
    win_h = h;

    // set the new viewport
    glViewport(0, 0, (GLint)win_w, (GLint)win_h);

    // we will use orthographic projection when drawing the object.
    //
    // NOTE: This has nothing to do with the projections you are
    //       to implement in this assignment.  We only need this
    //       when you draw 2D lines.  In other words, find the 2D
    //       projections of the end points of a given 3D line using
    //       the projection matrices you implemented and then draw
    //       a 2D line between the projected end-points.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 511.0, 0.0, 511.0, -1.0, 1.0);
}


// called when the window needs to be redrawn
void display()
{
    // clear the buffer with bg color set in init()
    // you can think of the buffer as a raster array provided by GL
    glClear(GL_COLOR_BUFFER_BIT);

    // draw the object on the buffer you just cleared
    drawFaces();

    // swap the buffers.
    // we are using 2 buffers provided by GL (see main) -- double buffer.
    // they are called front / back buffers.
    // what you see on the screen is the content of front buffer
    // what you clear/draw above is done on back buffer
    // once drawing is done on the back buffer,
    //       you need to display the content of the back buffer.
    // swapping buffers means swapping back buffer with front buffer
    //       so that front buffer becomes back buffer and
    //       back buffer becomes front buffer.
    // once back buffer becomes front buffer, the content of it will be
    //       drawn on the screen, so you can see it.
    glutSwapBuffers();
}


// called when a mouse event (button pressed/released) occurs in glut, 
//     mouse buttons are represented as
//           GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, and GLUT_RIGHT_BUTTON
//     status mouse buttons are represented as
//           GLUT_UP and GLUT_DOWN
// 
void mouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN) {  // mouse pressed.  record the details
        // which button?
        mtracker.button = button;
        // any modifiers (keys like shift/ctrl/alt) pressed?
        mtracker.modifiers = glutGetModifiers();
        // convert mouse position to a position in world coord. system
        DeviceToWorld(double(x), double(y), mtracker.initx, mtracker.inity);
    }
}


// called when a mouse moves with a button pressed
void motion(int x, int y)
{
    // get the mouse position in world
    DeviceToWorld(double(x), double(y), mtracker.finalx, mtracker.finaly);


    /*cout << '(' << x << ',' << y << ',' << mtracker.initx << ',' << mtracker.inity << ")  ("
         << x << ',' << y << ',' << mtracker.finalx << ',' << mtracker.finaly << ")" << endl;*/

    // now, process the user input, i.e., mouse movement
    switch (mtracker.button) {
        case GLUT_LEFT_BUTTON:
            if (mtracker.modifiers & GLUT_ACTIVE_SHIFT) {
                // shift + left button ==> translate in xz plane
                Translate_xz(mtracker.finalx - mtracker.initx,
                             mtracker.finaly - mtracker.inity);
            }
            else {
                // left button ==> translate in xy plane
                Translate_xy(mtracker.finalx - mtracker.initx,
                             mtracker.finaly - mtracker.inity);
            }
            break;
        case GLUT_RIGHT_BUTTON:
            // right button ==> scale
            Scale(mtracker.finalx - mtracker.initx);
            break;
        case GLUT_MIDDLE_BUTTON:
            // middle button ==> rotate
            Rotate(mtracker.finalx - mtracker.initx,
                   mtracker.finaly - mtracker.inity);
            break;
    }

    // redisplay after transformation
    glutPostRedisplay();

    // reset the mouse position
    mtracker.initx = mtracker.finalx;
    mtracker.inity = mtracker.finaly;
}


// called when a keyboard event (key typed) occurs
// you need to add cases for 'r' and 'b'
void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 'Q':  // quit the program
        case 'q':
            exit(0);
        case 'P':  // toggle the projection method
        case 'p':  // between orthographic and perspective projections
            cam.perspective = !cam.perspective;
            SetPerspMatrix();
            glutPostRedisplay();
            break;
        case 'R': // reset
        case 'r':
            initObj();
            initCam();
            glutPostRedisplay();
            break;
    }
}




/********************************************
 *        implementation for helpers        *
 ********************************************/


void init()
{
    // set background color to black
    glClearColor(0.0, 0.0, 0.0, 0.0);
}


// arrange the object to its initial position
// NOTE: you may change the parameters as you like
void initObj()
{
    obj.center = Point3(0.0, 0.0, 0.0);

    Vector3 n;

    // rotate around y-axis
    n.x = 0.0;  n.y = 1.0;  n.z = 0.0;
    double angle = M_PI / 6.0;
    Matrix4 m1 = SetRotMatrix(n, angle);

    // rotate around x-axis
    n.x = 1.0;  n.y = 0.0;  n.z = 0.0;
    angle = M_PI / 6.0;
    Matrix4 m2 = SetRotMatrix(n, angle);

    // notice the order of the transformations applied
    //  i.e., Ry -> Rx -> T  becomes  TRxRy in matrix multiplication

    // translate so that the object is inside view volume
    // (see initCam() for the view volume)
    obj.frame = Mult4(m2, m1);
    Translate_xz(0.0, -5.0);

    //obj.frame = Matrix4(1.0);  // identity matrix
}


// initialize camera parameters
// NOTE: you may change the parameters as you like
void initCam()
{
    // use orthographic projection as default
    cam.perspective = false;

    // camera position
    cam.eye.x = 0.0;
    cam.eye.y = 0.0;
    cam.eye.z = 0.0;

    // view volume
    cam.l = -5.0;  cam.r = 5.0;
    cam.b = -5.0;  cam.t = 5.0;
    cam.n = -1.0;  cam.f = -6.0;

    // camera coordinate system
    cam.u.x = 1.0;  cam.u.y = 0.0;  cam.u.z = 0.0;
    cam.v.x = 0.0;  cam.v.y = 1.0;  cam.v.z = 0.0;
    cam.w.x = 0.0;  cam.w.y = 0.0;  cam.w.z = 1.0;

    // set Mcam, Mp, Mo
    SetViewMatrix();
    SetPerspMatrix();
    SetOrthoMatrix();
}


// draw object faces
// IMPORTANT: you are only allowed to use glVertex2d with GL_LINE_LOOP
void drawFaces()
{

    Matrix4 M = Mult4(cam.Mo,Mult4(cam.Mp,Mult4(cam.Mv, obj.frame)));

    HPoint3 mod_vertices[MAXVERTICES];

    for(int i = 0; i < obj.Nvertices; i++){//converting all the vertices
        mod_vertices[i] = HPoint3(obj.vertices[i].x, obj.vertices[i].y, obj.vertices[i].z, 1.0);
        mod_vertices[i] = TransHPoint3(M, mod_vertices[i]);
        mod_vertices[i] = Homogenize(mod_vertices[i]);
    }

    for(int i = 0; i < obj.Nfaces; i++){//drawing face wise
        glBegin(GL_LINE_LOOP);
        for(int j = 1; j <= obj.faces[i][0]; j++){
            glVertex2d(mod_vertices[obj.faces[i][j]].x, mod_vertices[obj.faces[i][j]].y);
        }
        glEnd();
    }

}




/********************************************
 *      implementation for projection       *
 ********************************************/




// Mcam
void SetViewMatrix()
{
    Matrix4 Mv;
    Mv[0][0] =  cam.u.x;  Mv[0][1] =  cam.u.y;  Mv[0][2] = cam.u.z;  Mv[0][3] = 0.0;
    Mv[1][0] =  cam.v.x;  Mv[1][1] =  cam.v.y;  Mv[1][2] = cam.v.z;  Mv[1][3] = 0.0;
    Mv[2][0] =  cam.w.x;  Mv[2][1] =  cam.w.y;  Mv[2][2] = cam.w.z;  Mv[2][3] = 0.0;
    Mv[3][0] =  0.0;  Mv[3][1] =  0.0;  Mv[3][2] = 0.0;  Mv[3][3] = 1.0;


    Matrix4 Mep = SetTransMatrix(-cam.eye.x, -cam.eye.y, -cam.eye.z);

    cam.Mv = Mult4(Mv, Mep);  //setting the view matrix

}

// Mo = Mvp . Morth
void SetOrthoMatrix()
{

    Matrix4 Mvp;
    Mvp[0][0] =  win_w/2.0;  Mvp[0][1] =  0.0;  Mvp[0][2] = 0.0;  Mvp[0][3] = (win_w - 1)/2.0;
    Mvp[1][0] =  0.0;  Mvp[1][1] =  win_h/2.0;  Mvp[1][2] = 0.0;  Mvp[1][3] = (win_h - 1)/2.0;
    Mvp[2][0] =  0.0;  Mvp[2][1] =  0.0;  Mvp[2][2] = 1.0;  Mvp[2][3] = 0.0;
    Mvp[3][0] =  0.0;  Mvp[3][1] =  0.0;  Mvp[3][2] = 0.0;  Mvp[3][3] = 1.0;

    Matrix4 Mortho;
    Mortho[0][0] =  2.0/(cam.r - cam.l);  Mortho[0][1] =  0.0;  Mortho[0][2] = 0.0;  Mortho[0][3] = -(cam.r + cam.l)/(cam.r - cam.l);
    Mortho[1][0] =  0.0;  Mortho[1][1] =  2.0/(cam.t - cam.b);  Mortho[1][2] = 0.0;  Mortho[1][3] = -(cam.t + cam.b)/(cam.t - cam.b);
    Mortho[2][0] =  0.0;  Mortho[2][1] =  0.0;  Mortho[2][2] = 2.0/(cam.n - cam.f);  Mortho[2][3] = -(cam.n + cam.f)/(cam.n - cam.f);
    Mortho[3][0] =  0.0;  Mortho[3][1] =  0.0;  Mortho[3][2] = 0.0;  Mortho[3][3] = 1.0;

    cam.Mo = Mult4(Mvp, Mortho); //setting the otho matrix

}


// Mp
void SetPerspMatrix()
{

    Matrix4 m;

    if(cam.perspective){
        m[0][0] =  1.0;  m[0][1] =  0.0;  m[0][2] = 0.0;  m[0][3] = 0.0;
        m[1][0] =  0.0;  m[1][1] =  1.0;  m[1][2] = 0.0;  m[1][3] = 0.0;
        m[2][0] =  0.0;  m[2][1] =  0.0;  m[2][2] = (cam.n+cam.f)/cam.n;  m[2][3] = -cam.f;
        m[3][0] =  0.0;  m[3][1] =  0.0;  m[3][2] = 1.0/cam.n;  m[3][3] = 0.0;
    } else{
        m = Matrix4(1.0);
    }

    cam.Mp = m;  //setting the perspective matrix

}





/********************************************************
 * implementation of utilities for matrices and vectors *
 ********************************************************/



// convert device coordinate to world coordinate
void DeviceToWorld(double u, double v, double& x, double& y)
{
    x = ((cam.r-cam.l)/win_w) * u;

    y = -((cam.t-cam.b)/win_h) * v;
}



// returns the product of two 4x4 matrices
Matrix4 Mult4(Matrix4 a, Matrix4 b)
{
    Matrix4 m;
    register double sum;

    for (int j=0; j<4;  j++)
        for (int i=0; i<4; i++) {
            sum = 0.0;
            for (int k=0; k<4; k++)
                sum +=  a[j][k] * b[k][i];
            m[j][i] = sum;
        }

    return m;
}





// returns the result of homogenization of the input point
HPoint3 Homogenize(HPoint3 a)
{
    HPoint3 p;
    if ((a.w) != 0.0) {
        p.x = a.x /(a.w);  p.y = a.y /(a.w);
        p.z = a.z /(a.w);  p.w = 1.0;
    }
    else {
        cerr << "Cannot Homogenize, returning original point\n";
        p.x = a.x;  p.y = a.y;  p.z = a.z;  p.w = a.w;
    }
    return p;
}


// returns the homogeneous 3d point as a result of
// multiplying a 4x4 matrix with a homogeneous point
HPoint3 TransHPoint3(Matrix4 m, HPoint3 p)
{
    HPoint3 temp;
    temp.x = m[0][0]*p.x + m[0][1]*p.y + m[0][2]*p.z + m[0][3]*p.w;
    temp.y = m[1][0]*p.x + m[1][1]*p.y + m[1][2]*p.z + m[1][3]*p.w;
    temp.z = m[2][0]*p.x + m[2][1]*p.y + m[2][2]*p.z + m[2][3]*p.w;
    temp.w = m[3][0]*p.x + m[3][1]*p.y + m[3][2]*p.z + m[3][3]*p.w;
    return temp;
}




/***********************************************
 *     implementation for transformations      *
 ***********************************************/



// translation in xy-plane
void Translate_xy(double tx, double ty)
{

    obj.center.x += tx;//updating the center
    obj.center.y += ty;

    obj.frame = Mult4(SetTransMatrix(tx, ty, 0.0), obj.frame);

}


// translation in xz-plane
void Translate_xz(double tx, double tz)
{
    obj.center.x += tx;//updating the center
    obj.center.z += tz;

    obj.frame = Mult4(SetTransMatrix(tx, 0.0, tz), obj.frame);

}


// uniform scale
void Scale(double sx)
{

    double scaleValue = 1.0 + alpha * sx;


    Matrix4 m1 = SetTransMatrix(-obj.center.x, -obj.center.y, -obj.center.z); //bring to center
    Matrix4 m2 = SetScaleMatrix(scaleValue, scaleValue, scaleValue); //apply scaling
    Matrix4 m3 = SetTransMatrix(obj.center.x, obj.center.y, obj.center.z); //bring to center


    obj.frame = Mult4(m3, Mult4(m2,Mult4(m1, obj.frame))); //multiplying the matrices

}



// rotation using the Rolling Ball transformation
void Rotate(double dx, double dy)
{
    double dr = sqrt( dx * dx + dy * dy );

    Vector3 n(-dy/dr,  dx/dr, 0);

    if( n.x != n.x || n.y != n.y || dr!=dr )
        return;


    double theta = atan(dr/R);


    Matrix4 m1 = SetTransMatrix(-obj.center.x, -obj.center.y, -obj.center.z); //bring to center
    Matrix4 m2 = SetRotMatrix(n, theta); //apply rotation
    Matrix4 m3 = SetTransMatrix(obj.center.x, obj.center.y, obj.center.z); //bring to center

    obj.frame = Mult4(m3, Mult4(m2,Mult4(m1, obj.frame))); //multiplying the matrices




}



/*********************************************
 * Implementation for transformation helpers *
 *********************************************/


// returns a 4x4 scale matrix, given sx, sy, sz as inputs 
Matrix4 SetScaleMatrix(double sx, double sy, double sz)
{
    Matrix4 m;
    m = Matrix4(1.0);
    m[0][0] = sx;
    m[1][1] = sy;
    m[2][2] = sz;
    return m;
}


// returns a 4x4 translation matrix, given tx, ty, tz as inputs 
Matrix4 SetTransMatrix(double tx, double ty, double tz)
{
    Matrix4 m;
    m = Matrix4(1.0);
    m[0][3] = tx;
    m[1][3] = ty;
    m[2][3] = tz;
    return m;
}


// returns a 4x4 rotation matrix, given an axis and an angle 
Matrix4 SetRotMatrix(Vector3 n, double angle)
{
    Matrix4 m;

    double cs = cos(angle), sn = sin(angle);


    m[0][0] =  cs+n.x*n.x*(1-cs);  m[0][1] =  n.x*n.y*(1-cs)-n.z*sn;  m[0][2] = n.x*n.z*(1-cs)+n.y*sn;  m[0][3] = 0.0;
    m[1][0] =  n.y*n.x*(1-cs)+n.z*sn;  m[1][1] =  cs+n.y*n.y*(1-cs);  m[1][2] = n.y*n.z*(1-cs)-n.x*sn;  m[1][3] = 0.0;
    m[2][0] =  n.z*n.x*(1-cs)-n.y*sn;  m[2][1] =  n.z*n.y*(1-cs)+n.x*sn;  m[2][2] = cs+n.z*n.z*(1-cs);  m[2][3] = 0.0;
    m[3][0] =  0.0;  m[3][1] =  0.0;  m[3][2] = 0.0;  m[3][3] = 1.0;

    return m;

}




/*********************************************
 *    for your convenience when debugging    *
 *********************************************/


// prints a 4x4 matrix
void PrintMat(Matrix4 m)
{
    for (int i=0;i<4;i++) {
        for (int j=0;j<4;j++) {
            std::cerr << m[i][j] << " ";
        }
        std::cerr << std::endl;
    }
}

// prints a homogeneous 3d point / vector
void PrintHPoint(HPoint3 p)
{
    std::cerr << "("
              << p.x << " "
              << p.y << " "
              << p.z << " "
              << p.w << ")" << std::endl;
}

// prints a 3d point / vector
void PrintPoint(Point3 p) {
    std::cerr << "("
              << p.x << " "
              << p.y << " "
              << p.z << ")" << std::endl;
}



