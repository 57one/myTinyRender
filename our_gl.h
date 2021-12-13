#ifndef __OUR_GL_H
#define __OUR_GL_H

#include"geometry.h"
#include"tgaimage.h"

extern Matrix4f ModelView;
extern Matrix4f Projection;
extern Matrix4f ViewPort;

void lookat(Vec3f eye, Vec3f center, Vec3f up);
void projection(float coff);
void viewport(int x, int y, int w, int h);

class IShader{
public:
    virtual ~IShader() { };
    virtual Vec4f vertexShader(int iface, int nthvert) = 0;
    virtual bool fragmentShader(Vec3f baryCoordinate, TGAColor &color) = 0;
};

void triangle(Vec4f *pts, IShader &shader, TGAImage &image, TGAImage &zbuffer);
Vec3f barycentric(Vec3f *pts,Vec3f P);

#endif