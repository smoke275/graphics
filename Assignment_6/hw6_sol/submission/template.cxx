/*************************************************************************
 *  COMP 597                                                             *
 *  Template code for HW 6                                               *
 *  March 28, 2019                                                       *
 *  Shashwata Mandal                                                     *
 *  sum675@psu.edu                                                       *
 *                                                                       *
 *                                                                       *
 *  Description:                                                         *
 *                                                                       *
 *  This is a template code for homework 6.                              *
 *  It reads the vertex, vertex normals  and face information            *
 *     of the triangles of a model from Wavefront .obj format.           *
 *  Then, it draws the model by drawing all triangles one by one.        *
 *                                                                       *
 *  For vectors and matrices, it uses glm library.                       *
 *  - glm library conforms the OpenGL's shading language (GLSL)          *
 *  - To understand how glm library works, see                           *
 *        www.khronos.org/registry/OpenGL/specs/gl/GLSLangSpec.4.20.pdf  *
 *  - Especially                                                         *
 *        5.10 Vector and Matrix Operations                              *
 *  - glm also provides,                                                 *
 *        glm::cross(u, v)  <= returns cross product of vectors u and v  *
 *        glm::normalize(v) <= returns unit vector of v                  *
 *    that works similar to                                              *
 *        glm::dot(u, v)    <= returns dot product of vectors u and v    *
 *    as explained in 5.10                                               *
 *                                                                       *
 *                                                                       *
 *  User interface:                                                      *
 *                                                                       *
 *  1. When it starts, its shading mode is set to GOURAUD.               *
 *  2. Use 'g' to put it back in GOURAUD mode from other modes.          *
 *  3. Use 'p' to put it in PHONG mode.                                  *
 *  4. Use 'h' to toggle the highlighting on and off (off is default).   *
 *  5. Use uparrow/downarrow to increase/decrease phong constant by 1.   *
 *  6. Use PageUp/PageDown to increase/decrease phong constant by 5.     *
 *  7. To quit the program, press 'q'.                                   *
 *************************************************************************/



#include <GL/glut.h>
#include <glm/glm.hpp>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include "model.h"

#include <iterator>
#include <algorithm>
#include <vector>



// callbacks for glut (see main() for what they do)
void reshape(int w, int h);
void display();
void mouse(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);
void special(int key, int x, int y);



// helpers
void init();

void draw_point(int x, int y, Color c);
void draw_model();


void draw_model_gouraud_shading();
void draw_model_phong_shading();


vec3 world2screen(vec3 v);

bool is_visible(const vec3& p0, const vec3& p1, const vec3& p2);
bool is_inside(const int x, const int y,                        // current pixel
               const vec3& p0, const vec3& p1, const vec3& p2,  // triangle vertices
               float& alpha, float& beta, float& gamma);        // barycentric coords for current pixel

Color calculate_color(vec3 n_hat, vec3 l_hat);



// for debugging purpose,
// overload operator<< for vec3
std::ostream& operator<< (std::ostream& out, const vec3& v);



// Keeps track of current shading mode.
enum ShadingMode { GOURAUD, PHONG };
ShadingMode shading_mode = GOURAUD;



// Initial window size
int win_w = 512;
int win_h = 512;


// z-buffer (always on)
float *zbuffer { nullptr };

// blinn-phong illumination
bool highlight_on { false };
float phong_constant { 10.0f };

// Model to render
Model *model{ nullptr };
int current_model { 0 };


// Camera position
vec3 cam { 0.0, 0.0, 10.0 };        // camera position
vec3 light { 0.0, 100.0, 100.0 };   // point light source position


// colors
vec3 c_ambient { 0.1, 0.1, 0.1 };   // ambient light
vec3 c_diffuse { 1.0, 0.0, 0.0 };   // diffuse reflectan
vec3 c_light   { 1.0, 1.0, 1.0 };   // directional light



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
    glutSpecialFunc(special);   // for arrow keys and function keys

    // start event processing, i.e., accept user inputs
    glutMainLoop();

    if (zbuffer) delete [] zbuffer;
    if (model)   delete [] model;

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

    // recreate zbuffer
    if (zbuffer)
        delete [] zbuffer;
    zbuffer = new float[win_w * win_h];

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, win_w - 1, 0.0, win_h - 1, -1.0, 1.0);

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
    draw_model();


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
}


// called when a keyboard event (key typed) occurs
void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 'q':  // quit the program
            exit(0);
        case 'g':  // gouraud shading
            std::cerr << "shading_mode changed to GOURAUD\n";
            shading_mode = GOURAUD;
            glutPostRedisplay();
            break;
        case 'p':  // phong shading
            std::cerr << "shading_mode changed to GOURAUD\n";
            shading_mode = PHONG;
            glutPostRedisplay();
            break;
        case 'h':  // highlihgt using blinn-phong illumination
            std::cerr << "turning highlight " << (highlight_on ? "off\n" : "on\n");
            highlight_on = !highlight_on;
            glutPostRedisplay();
            break;
    }
}




// called when a keyboard event (key typed) occurs
void special(int key, int x, int y)
{
    switch (key) {
        case GLUT_KEY_UP:         // up arrow key
            if (shading_mode == PHONG) {
                phong_constant += 1.0f;
                std::cerr << "  phong constant: " << phong_constant << std::endl;
            }
            else {
                std::cerr << "  phong constant cannot be changed since shading_mode is not PHONG" << std::endl;
            }
            break;
        case GLUT_KEY_DOWN:       // down arrow key
            if (shading_mode == PHONG) {
                phong_constant = std::max(1.0f, phong_constant - 1.0f);
                std::cerr << "  phong constant: " << phong_constant << std::endl;
            }
            else {
                std::cerr << "  phong constant cannot be changed since shading_mode is not PHONG" << std::endl;
            }
            break;
        case GLUT_KEY_PAGE_UP:    // up arrow key
            if (shading_mode == PHONG) {
                phong_constant += 5.0f;
                std::cerr << "  phong constant: " << phong_constant << std::endl;
            }
            else {
                std::cerr << "  phong constant cannot be changed since shading_mode is not PHONG" << std::endl;
            }
            break;
        case GLUT_KEY_PAGE_DOWN:  // down arrow key
            if (shading_mode == PHONG) {
                phong_constant = std::max(1.0f, phong_constant - 5.0f);
                std::cerr << "  phong constant: " << phong_constant << std::endl;
            }
            else {
                std::cerr << "  phong constant cannot be changed since shading_mode is not PHONG" << std::endl;
            }
            break;
        case GLUT_KEY_F1:        // F1 key for sphere model
            if (current_model == 1) return;
            current_model = 1;
            if (!model)
                delete [] model;
            model = new Model("../models/sphere-223.obj");
            break;
        case GLUT_KEY_F2:        // F2 key for teapot model
            if (current_model == 2) return;
            current_model = 2;
            if (!model)
                delete [] model;
            model = new Model("../models/wt_teapot.obj");
            break;
        case GLUT_KEY_F3:        // F3 key for african_head model
            if (current_model == 3) return;
            current_model = 3;
            if (!model)
                delete [] model;
            model = new Model("../models/african_head.obj");
            break;
        case GLUT_KEY_F4:        // F4 key for Disablo model
            if (current_model == 4) return;
            current_model = 4;
            if (!model)
                delete [] model;
            model = new Model("../models/Diablo.obj");
            break;
    }

    glutPostRedisplay();
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
    glOrtho(0.0, win_w - 1, 0.0, win_h - 1, -1.0, 1.0);

    // create zbuffer
    zbuffer = new float[win_w * win_h];
}


void draw_point(int x, int y, Color c)
{
    glBegin(GL_POINTS);
    {
        glColor3f(c.r, c.g, c.b);
        glVertex2d(x, y);
    }
    glEnd();
}

void draw_model()
{
    if (!model) return;

    switch (shading_mode) {
        case GOURAUD:
            draw_model_gouraud_shading();
            break;
        case PHONG:
            draw_model_phong_shading();
            break;
    }
}


void draw_model_gouraud_shading()
{
    int cnt = 0;

    std::fill(zbuffer, zbuffer + win_w * win_h, INT_MIN);
    int z_max = win_w*win_h;

    for(int i = 0; i < model->num_faces(); i++){

        std::vector<int> face = model->face(i);
        vec3 v0 =  model->vertex(face[0]);
        vec3 v1 =  model->vertex(face[1]);
        vec3 v2 =  model->vertex(face[2]);

        if(is_visible(v0, v1, v2)){
            vec3 p0 =  world2screen(v0);
            vec3 p1 =  world2screen(v1);
            vec3 p2 =  world2screen(v2);
            std::vector<int> faceNormalIndex = model->face_vn(i);
            vec3 n_hat_p0 =  glm::normalize(model->normal(faceNormalIndex[0]));
            vec3 n_hat_p1 =  glm::normalize(model->normal(faceNormalIndex[1]));
            vec3 n_hat_p2 =  glm::normalize(model->normal(faceNormalIndex[2]));

            vec3 l_hat_p0 =  glm::normalize(light - v0);
            vec3 l_hat_p1 =  glm::normalize(light - v1);
            vec3 l_hat_p2 =  glm::normalize(light - v2);

            vec3 color0 = calculate_color(n_hat_p0, l_hat_p0);
            vec3 color1 = calculate_color(n_hat_p1, l_hat_p1);
            vec3 color2 = calculate_color(n_hat_p2, l_hat_p2);

            int x_min = INT_MAX, x_max = INT_MIN;
            int y_min = INT_MAX, y_max = INT_MIN;

            x_min = static_cast<int>(std::min(p0.x, std::min(p1.x, p2.x)));
            x_max = static_cast<int>(std::max(p0.x, std::max(p1.x, p2.x)));
            y_min = static_cast<int>(std::min(p0.y, std::min(p1.y, p2.y)));
            y_max = static_cast<int>(std::max(p0.y, std::max(p1.y, p2.y)));


            for(int x = x_min; x < x_max; x++){
                for(int y = y_min; y < y_max; y++){

                    float alpha = 0.0f, beta = 0.0f, gamma = 0.0f;
                    if(is_inside(x, y, p0, p1, p2, alpha, beta, gamma)){

                        vec3 point = alpha * v0 + beta * v1 + gamma * v2;
                        Color color = alpha * color0 + beta * color1 + gamma * color2;

                        int index = win_h * x + y;

                        if(index<z_max && zbuffer[index] < point.z) {
                            draw_point(x, y, color);
                            zbuffer[index] = point.z;
                        }



                    }

                }
            }
            cnt++;
            }
    }

    std::cerr << "draw_model_gouraud_shading: drawn " << cnt << " / " << model->num_faces() << " triangles\n";
}


void draw_model_phong_shading()
{
    int cnt = 0;

    std::fill(zbuffer, zbuffer + win_w * win_h, INT_MIN);
    int z_max = win_w*win_h;

    for(int i = 0; i < model->num_faces(); i++){

        std::vector<int> face = model->face(i);
        vec3 v0 =  model->vertex(face[0]);
        vec3 v1 =  model->vertex(face[1]);
        vec3 v2 =  model->vertex(face[2]);

        if(is_visible(v0, v1, v2)){
            vec3 p0 =  world2screen(v0);
            vec3 p1 =  world2screen(v1);
            vec3 p2 =  world2screen(v2);

            std::vector<int> faceNormalIndex = model->face_vn(i);
            vec3 n_hat_p0 =  glm::normalize(model->normal(faceNormalIndex[0]));
            vec3 n_hat_p1 =  glm::normalize(model->normal(faceNormalIndex[1]));
            vec3 n_hat_p2 =  glm::normalize(model->normal(faceNormalIndex[2]));

            vec3 l_hat_p0 =  glm::normalize(light - v0);
            vec3 l_hat_p1 =  glm::normalize(light - v1);
            vec3 l_hat_p2 =  glm::normalize(light - v2);

            vec3 color0 = calculate_color(n_hat_p0, l_hat_p0);
            vec3 color1 = calculate_color(n_hat_p1, l_hat_p1);
            vec3 color2 = calculate_color(n_hat_p2, l_hat_p2);

            int x_min = INT_MAX, x_max = INT_MIN;
            int y_min = INT_MAX, y_max = INT_MIN;

            x_min = static_cast<int>(std::min(p0.x, std::min(p1.x, p2.x)));
            x_max = static_cast<int>(std::max(p0.x, std::max(p1.x, p2.x)));
            y_min = static_cast<int>(std::min(p0.y, std::min(p1.y, p2.y)));
            y_max = static_cast<int>(std::max(p0.y, std::max(p1.y, p2.y)));

            for(int x = x_min; x < x_max; x++){
                for(int y = y_min; y < y_max; y++){

                    float alpha = 0.0f, beta = 0.0f, gamma = 0.0f;
                    if(is_inside(x, y, p0, p1, p2, alpha, beta, gamma)){

                        vec3 point = alpha * v0 + beta * v1 + gamma * v2;
                        Color color = alpha * color0 + beta * color1 + gamma * color2;

                        if(highlight_on){
                            vec3 n_hat = glm::normalize(alpha * n_hat_p0 + beta * n_hat_p1 + gamma * n_hat_p2);
                            vec3 l_hat =  glm::normalize(light - point);
                            vec3 eye_hat = glm::normalize(cam - point);
                            vec3 h_hat = glm::normalize(l_hat + eye_hat);
                            vec3 color_highlight = c_light * powf(glm::dot(h_hat, n_hat), phong_constant);
                            color += color_highlight;
                        }

                        int index = win_h * x + y;

                        if(index<z_max && zbuffer[index] < point.z) {
                            draw_point(x, y, color);
                            zbuffer[index] = point.z;
                        }



                    }

                }
            }
            cnt++;
        }
    }

    std::cerr << "draw_model_phong_shading: drawn " << cnt << " / " << model->num_faces() << " triangles\n";
}


vec3 world2screen(vec3 v)
{
    float width = win_w / 1.0;
    float height = win_h / 1.0;

    // note that z-coordinate is not changed
    // that is,
    //   x and y coords are in screen coord sys
    //   z coord is in world coord sys
    // so that you can use z coord for z-buffering
    return vec3(static_cast<int>((v.x + 1.0) * width / 2.0 + 0.5),
                static_cast<int>((v.y + 1.0) * height / 2.0 + 0.5),
                v.z);
}


// for back-face culling
// - if face normal and eye (camera) vector have the angle greater that 90 degree,
//   no need to draw the face since the eye cannot see the front side of the face.
// - returns
//     true if visible (ie, less than 90 <= dot product is not negative)
//     false if invisible (ie, over 90 <= dot product is negative)
bool is_visible(const vec3& p0, const vec3& p1, const vec3& p2)
{
    vec3 mid_point = (p0 + p1 + p2);
    mid_point /= 3;

    vec3 eyeV = cam - mid_point;
    vec3 normal = glm::cross(p1 - p0, p2 - p0);
    bool visible = glm::dot(eyeV, normal)>0;

    return visible;

    return true;
}

// is pixel (x, y) inside the triangle p0-p1-p2?
//   returns
//     true if it is inside, false otherwise
//   also returns
//     alpha, beta, gamma
//
// can be used by draw_model_flat_shading
//
bool is_inside(const int x, const int y,
               const vec3& p0, const vec3& p1, const vec3& p2,
               float& alpha, float& beta, float& gamma)
{

    vec3 v0(p0.x, p0.y, 0.0f);
    vec3 v1(p1.x, p1.y, 0.0f);
    vec3 v2(p2.x, p2.y, 0.0f);

    //std::cerr<<v0<<v1<<v2<<std::endl;

    vec3 n = glm::cross(v1 - v0, v2 - v0);
    float length = glm::length(n); length *= length;

    vec3 p(x, y, 0.0f);

    vec3 na = glm::cross(v2 - v1, p - v1);
    vec3 nb = glm::cross(v0 - v2, p - v2);
    vec3 nc = glm::cross(v1 - v0, p - v0);

    float local_alpha = glm::dot(n,na)/length;
    float local_beta = glm::dot(n,nb)/length;
    float local_gamma = glm::dot(n,nc)/length;



    bool inside = true;

    inside &= (local_alpha>=0);
    inside &= (local_beta>=0);
    inside &= (local_gamma>=0);

    if(inside){
        alpha = local_alpha;
        beta = local_beta;
        gamma = local_gamma;
    }

    return inside;
}


// calcuate the color at a point whose
//   - normal is n_hat and
//   - light vector is l_hat
// using Blinn-Phong illumiation model
// Note that the specular light should be added only when highlight is on
Color calculate_color(vec3 n_hat, vec3 l_hat)
{
    return c_diffuse * c_ambient + c_diffuse * c_light * std::max(0.0f, glm::dot(n_hat, l_hat));
}


// for debugging purpose,
// overload operator<< for vec3
std::ostream& operator<< (std::ostream& out, const vec3& v)
{
    out << "R: " << v.r << ", G: " << v.g << ", B: " << v.b << ")";
    return out;
}
