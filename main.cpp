#include<iostream>
#include<limits>
#include"tgaimage.h"
#include"geometry.h"
#include"model.h"
#include"our_gl.h"

const int width = 800;
const int height = 800;
Model *model;

Vec3f light_dir(1, 1, 1);
Vec3f eye(1, 1, 3);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);

class GouraudShader : public IShader{
public:
    // Vec3f varying_intensity; //vertext shader interpolate intensity fragment shader use it
    Mat<3, 3, float> varing_uvs;
    Matrix4f uniform_M; //projection * modelview
    Matrix4f uniform_MIT; //(projection * modelview).invert_transpose()

    virtual Vec4f vertexShader(int iface, int nthvert){
        Vec4f gl_vertex = embed<4>(model->vert(iface,nthvert));
        gl_vertex = ViewPort * Projection * ModelView * gl_vertex;
        // varying_intensity[nthvert] = std::max(0.0f, model->normal(iface, nthvert) * light_dir);
        varing_uvs.set_col(nthvert, model->uv(iface, nthvert));
        return gl_vertex;
    }

    virtual bool fragmentShader(Vec3f baryCoordinate, TGAColor &color){
        // float intensity = varying_intensity * baryCoordinate;
        Vec3f uv = varing_uvs * baryCoordinate;
        Vec3f n = proj<3>(uniform_MIT * embed<4>(model->normal(uv))).normalize(); // normal
        Vec3f l = proj<3>(uniform_M * embed<4>(light_dir)).normalize(); //light_dir
        Vec3f r = proj<3>(n * (n * l * 2.0f) - l).normalize();
        float spec = pow(std::max(r.z, 0.0f), model->specular(uv));  //sepcular //why r.z i still do not know
        float diff = std::max(0.0f, n * l); //diffuse
        TGAColor c = model->diffuse(uv); //this diffuse method get color from texture
        color = c;
        for (int i=0; i<3; i++) color[i] = std::min<float>(5 + c[i]*(diff + .6*spec), 255);
        return false;
    }
};

int main(){
    model = new Model("obj/african_head.obj");

    TGAImage image(width, height, TGAImage::RGB);
    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);
    TGAImage texture;
    texture.read_tga_file("obj/african_head_diffuse.tga");
    texture.flip_vertically();
    image.flip_vertically();

    lookat(eye, center, up);
    viewport(0, 0, width, height);
    projection(-1.0f / (eye-center).norm());
    light_dir.normalize();

    GouraudShader gouraudShader;
    gouraudShader.uniform_M = Projection * ModelView;
    gouraudShader.uniform_MIT = (Projection * ModelView).transpose().invert();

    for(int i = 0; i<model->nfaces(); i++){
        Vec4f screen_coords[3];
        for(int j = 0; j<3; j++)
            screen_coords[j] = gouraudShader.vertexShader(i, j);
        triangle(screen_coords, gouraudShader, image, zbuffer);
    }

    image.  write_tga_file("output.tga");
    zbuffer.write_tga_file("zbuffer.tga");

    delete model;
    return 0;
}