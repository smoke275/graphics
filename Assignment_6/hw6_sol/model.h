/**************************************************************************
 *  COMP 597                                                             *
 *  Template code for HW 6                                               *
 *  March 28, 2019                                                       *
 *  Sukmoon Chang                                                        *
 *  sukmoon@psu.edu                                                      *
 *                                                                        *
 *                                                                        *
 *  Description:                                                          *
 *                                                                        *
 *  This is a class that represents the model to render.                  *
 *                                                                        *
 *  Constructor:                                                          *
 *    Loads the vertex, vertex normal, and face (triangles) data          *
 *      from the given file                                               *
 *    The file must be Wavefront's .obj format                            *
 *                                                                        *
 *  Methods:                                                              *
 *  - num_vertices() : returns the number of vertices in the model        *
 *  - num_normals() : returns the number of vertex normals in the model   *
 *  - num_faces() : returns the number of faces (triangles) in the model  *
 *  - vertex(i) : returns i-th vertex in vec3                             *
 *  - normal(i) : returns i-th vertex normal in vec3                      *
 *  - face(i) : returns vertex indices for i-th face (triangle)           *
 *  - face_vn(i) : returns vertex normal indices for i-th face (triangle) *
 *                                                                        *
 **************************************************************************/


#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <glm/glm.hpp>


using vec3 = glm::vec3;
using Color = glm::vec3;



class Model
{
public:
    Model(const char *filename);
    ~Model();

    int num_vertices();   // number of vertices in the model
    int num_normals();    // number of normal vectors in the model
    int num_faces();      // number of triangles in the model

    vec3 vertex(int i);               // returns i-th vertex
    vec3 normal(int i);               // returns i-th normal
    std::vector<int> face(int i);     // returns i-th triangle's vertices
    std::vector<int> face_vn(int i);  // returns i-th triangle's vertex normals
    
private:
    std::vector<vec3> verts;
    std::vector<vec3> norms;
    std::vector<std::vector<int>> faces;
    std::vector<std::vector<int>> face_vert_norms; // <====================

    void load_data(const char *filename);
};

#endif
