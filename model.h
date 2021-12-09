#ifndef __MODEL_H
#define __MODEL_H

#include<vector>
#include"geometry.h"

class Model {
private:
    std::vector<Vec3f> verts_;
    std::vector<std::vector<Vec3i>> faces_; //vertex vertex_texture vertex_normal
public:
    Model(const char *filename);
    ~Model();
    int nverts();
    int nfaces();
    Vec3f vert(int i);
    std::vector<int> face(int i);
};

#endif