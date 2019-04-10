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
 *  This is an implementation of the class Model                         *
 *  - Currently the bottleneck is load_data()                            *
 *  - If you can improve the performance, please let me know             *
 *                                                                       *
 *************************************************************************/


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <regex>
#include <limits>
#include "model.h"

#include <ctime>


Model::Model(const char *filename)
    : verts{}, faces{}
{
    load_data(filename);
}

Model::~Model()
{ }


int Model::num_vertices()
{
    return static_cast<int>(verts.size());
}

int Model::num_faces()
{
    return static_cast<int>(faces.size());
}


// These 2 need range error checking

vec3 Model::vertex(int i)
{
    return verts[i];
}

std::vector<int> Model::face(int i)
{
    return faces[i];
}


// private method
void Model::load_data(const char *filename)
{
    std::cout << "\nloading data (v2)..." << std::flush;

    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail()) {
        std::cout << "failed to open \"" << filename << "\"...\n";
        return;
    }

    // read the whole file for efficiency
    std::string data;
    in.seekg(0, std::ios::end);
    data.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&data[0], data.size());
    in.close();

    std::istringstream str_in(data);
    
    
    std::string line;
    float x_max = -std::numeric_limits<float>::max();
    float y_max = -std::numeric_limits<float>::max();
    float z_max = -std::numeric_limits<float>::max();
    float x_min =  std::numeric_limits<float>::max();
    float y_min =  std::numeric_limits<float>::max();
    float z_min =  std::numeric_limits<float>::max();

    while (!str_in.eof()) {
        std::getline(str_in, line);
        std::istringstream ss(line.c_str());
        char ignore;
        if (!line.compare(0, 2, "v ")) {
            // .obj format for v: v x y z [w=1.0]
            vec3 v;
            ss >> ignore;               // consume "v "
            ss >> v[0] >> v[1] >> v[2]; // read x, y, z

                                        // find the bounding box for vertices
            x_max = std::max(x_max, v[0]);
            y_max = std::max(y_max, v[1]);
            z_max = std::max(z_max, v[2]);
            x_min = std::min(x_min, v[0]);
            y_min = std::min(y_min, v[1]);
            z_min = std::min(z_min, v[1]);

            verts.push_back(v);
        }
        else if (!line.compare(0, 2, "f ")) {
            // .obj format for f: f index1 index2 index3  
            std::vector<int> face;
            int v_idx, t_idx, n_idx;
            ss >> ignore;               // consume "f "
            if (regex_search(line, std::regex{ R"(\s\d+//\d+\s)" })) {
                // .obj format for f: "f 7//1 8//2 9//3"     <= vertex//normal
                // - currently only vertices are supported
                while (ss >> v_idx >> ignore >> ignore >> n_idx)
                    face.push_back(v_idx - 1);  // .obj index starts from 1, not 0
            }
            else if (regex_search(line, std::regex{ R"(\s\d+/\d+/\d+\s)" })) {
                // .obj format for f: "f 6/4/1 3/5/3 7/6/5"  <= vertex/texture/normal
                // - currently only vertices are supported
                while (ss >> v_idx >> ignore >> t_idx >> ignore >> n_idx)
                    face.push_back(v_idx - 1);  // .obj index starts from 1, not 0
            }
            else if (regex_search(line, std::regex{ R"(\s\d+/\d+\s)" })) {
                // .obj format for f: "f 3/1 4/2 5/3"        <= vertex/texture
                // - currently only vertices are supported
                while (ss >> v_idx >> ignore >> t_idx)
                    face.push_back(v_idx - 1);  // .obj index starts from 1, not 0
            }
            else if (regex_search(line, std::regex{ R"(\s\d+\s)" })) {
                // .obj format for f: "f 1 2 3"              <= vertex
                // - currently only vertices are supported
                while (ss >> v_idx)
                    face.push_back(v_idx - 1);  // .obj index starts from 1, not 0
            }

            //faces.push_back(face);
            
            if (face.size() == 3) {
                faces.push_back(face);
            }
            else if (face.size() > 3) {
                for (unsigned int i = 1; i < face.size() - 1; i++)
                    faces.push_back(std::vector<int>{ face[0], face[i], face[i + 1] });
            }
            else {
                std::cout << "face.size() = " << face.size() << std::endl;
            }
        }
    }

    // scale vertex coordinates to the canonical volume
    if (x_min < -1.0 || x_max > 1.0 ||
        y_min < -1.0 || y_max > 1.0 ||
        z_min < -1.0 || z_max > 1.0) {
        std::cout << "recentering and scaling...";
        float max_range = std::max({ x_max - x_min, y_max - y_min, z_max - z_min });
        float inv_max_range_2 = 2.0f / max_range;  // so that bounding box is [-1,1]^3

        float x_center = (x_max + x_min) * 0.5f;
        float y_center = (y_max + y_min) * 0.5f;
        float z_center = (z_max + z_min) * 0.5f;
        vec3 center{ x_center, y_center, z_center };

        for (auto v_it = verts.begin(); v_it != verts.end(); v_it++)
            *v_it = (*v_it - center) * inv_max_range_2;
    }
    else {
        std::cout << "nothing to scale...";
    }

    std::cout << "done" << std::endl;
    std::cout << "-- stats\n";
    std::cout << "     number of vertices: " << num_vertices() << std::endl;
    std::cout << "        number of faces: " << num_faces() << std::endl;
    std::cout << std::endl;
}
