#ifndef __MODEL_H
#define __MODEL_H

#include<vector>
#include"geometry.h"
#include"tgaimage.h"

class Model {
private:
    std::vector<Vec3f> verts_;
    std::vector<Vec3f> uv_;
    std::vector<Vec3f> norms_;
    std::vector<std::vector<Vec3i>> faces_; //vertex vertex_texture vertex_normal
    TGAImage diffusemap_;
    TGAImage normalmap_;
    TGAImage specularmap_;
    void load_texture(std::string filename, const char *suffix, TGAImage &img);
public:
    Model(const char *filename);
    ~Model();
    int nverts();
    int nfaces();
    Vec3f vert(int i);
    Vec3f vert(int iface, int nthvert);
    Vec3f uv(int iface, int nthvert);
    Vec3f normal(int iface, int nthvert);
    Vec3f normal(Vec3f uv);
    std::vector<int> face(int i);
    TGAColor diffuse(Vec3f uv);
    float specular(Vec3f uv);
};

#endif