/*************************************************************************
 *  COMP 597                                                             *
 *  Template code for HW 5                                               *
 *  March 21, 2019                                                       *
 *  Sukmoon Chang                                                        *
 *  sukmoon@psu.edu                                                      *
 *                                                                       *
 *                                                                       *
 *  Description:                                                         *
 *                                                                       *
 *  This is a class that represents the model to render.                 *
 *                                                                       *
 *  Constructor:                                                         *
 *    Loads the vertex and face (triangles) data from the given file     *
 *    The file must be Wavefront's .obj format                           *
 *                                                                       *
 *  Methods:                                                             *
 *  - num_vertices() : returns the number of vertices in the model       *
 *  - num_faces() : returns the number of faces (triangles) in the model *
 *  - vertex(i) : returns i-th vertex in vec3                            *
 *  - face(i) : returns i-th face (triangle) containing 3 vec3's         *
 *                                                                       *
 *************************************************************************/


#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <glm/vec3.hpp>


using vec3  = glm::vec3;
using Color = glm::vec3;


class Model
{
public:
    Model(const char *filename);
    ~Model();

    int num_vertices();   // number of vertices in the model
    int num_faces();      // number of triangles in the model

    vec3 vertex(int i);              // returns i-th vertex
    std::vector<int> face(int i);    // returns i-th triangle
    
private:
    std::vector<vec3> verts;
    std::vector<std::vector<int>> faces;

    void load_data(const char *filename);
};

#endif
