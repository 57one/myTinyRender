#include<iostream>
#include<limits>
#include"tgaimage.h"
#include"geometry.h"
#include"model.h"
#include"our_gl.h"

const int width = 800;
const int height = 800;
Model *model;
float *shadowbuffer;

Vec3f light_dir(1, 1, 0);
Vec3f eye(0, 0, 4);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);

class DepthShader:public IShader{
public:
    Mat<3,3,float> varying_tri;

    DepthShader() : varying_tri() {}

    virtual Vec4f vertexShader(int iface, int nthvert) {
        Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert)); // read the vertex from .obj file
        gl_Vertex = ViewPort*Projection*ModelView*gl_Vertex;          // transform it to screen coordinates
        varying_tri.set_col(nthvert, proj<3>(gl_Vertex/gl_Vertex[3]));
        return gl_Vertex;
    }

    virtual bool fragmentShader(Vec3f bar, TGAColor &color) {
        Vec3f p = varying_tri * bar;
        color = TGAColor(255, 255, 255)*(p.z/depth);
        return false;
    }
};

class GouraudShader : public IShader{
public:
    // Vec3f varying_intensity; //vertext shader interpolate intensity fragment shader use it
    Mat<3, 3, float> varing_uvs;
    Mat<3, 3, float> varing_tri;
    Matrix4f uniform_M; //projection * modelview
    Matrix4f uniform_MIT; //(projection * modelview).invert_transpose()
    Matrix4f uniform_Mshadow; //transform framebuffer screen coordinates to shadowbuffer screen coordinates
    bool t = false;

    GouraudShader(Matrix4f M, Matrix4f MIT, Matrix4f MS):uniform_M(M), uniform_MIT(MIT), uniform_Mshadow(uniform_Mshadow), varing_uvs(), varing_tri() { }

    virtual Vec4f vertexShader(int iface, int nthvert){
        Vec4f gl_vertex = embed<4>(model->vert(iface,nthvert));
        gl_vertex = ViewPort * Projection * ModelView * gl_vertex;
        // varying_intensity[nthvert] = std::max(0.0f, model->normal(iface, nthvert) * light_dir);
        varing_uvs.set_col(nthvert, model->uv(iface, nthvert));
        varing_tri.set_col(nthvert, proj<3>(gl_vertex / gl_vertex[3]));
        return gl_vertex;
    }

    virtual bool fragmentShader(Vec3f baryCoordinate, TGAColor &color){
        Vec4f shadowbuf_p = uniform_Mshadow * embed<4>(varing_tri * baryCoordinate);
        shadowbuf_p = shadowbuf_p / shadowbuf_p[3];
        int idx = int(shadowbuf_p[0]) + int(shadowbuf_p[1])*width; // index in the shadowbuffer array
        float shadow = .3+.7*(shadowbuffer[idx]<shadowbuf_p[2]+10); // magic coeff to avoid z-fighting

        Vec3f uv = varing_uvs * baryCoordinate;
        Vec3f n = proj<3>(uniform_MIT * embed<4>(model->normal(uv))).normalize(); // normal
        Vec3f l = proj<3>(uniform_M * embed<4>(light_dir)).normalize(); //light_dir
        Vec3f r = proj<3>(n * (n * l * 2.0f) - l).normalize();
        float spec = pow(std::max(r.z, 0.0f), model->specular(uv));  //sepcular //why r.z i still do not know
        float diff = std::max(0.0f, n * l); //diffuse
        TGAColor c = model->diffuse(uv); //this diffuse method get color from texture
        for (int i=0; i<3; i++) color[i] = std::min<float>(20 + c[i]*shadow*(1.2*diff + .6*spec), 255);
        // for (int i=0; i<3; i++) color[i] = std::min<float>(20 + c[i]*(diff + .6*spec), 255);;
        
        return false;
    }
};

int main(){
    model = new Model("obj/diablo/diablo3_pose.obj");

    float *zbuffer = new float[width*height];
    shadowbuffer = new float[width*height];
    for(int i = 0; i<width*height; i++) zbuffer[i] = shadowbuffer[i] = -std::numeric_limits<float>::max();

    light_dir.normalize();

    { //rendering the frame buffer
        TGAImage depth(width, height, TGAImage::RGB);
        depth.flip_vertically();
        lookat(light_dir, center, up);
        viewport(0, 0, width, height);
        projection(0);

        DepthShader depthShader;
        for(int i = 0; i<model->nfaces(); i++){
            Vec4f screen_coords[3];
            for(int j = 0; j<3; j++)
                screen_coords[j] = depthShader.vertexShader(i, j);
            triangle(screen_coords, depthShader, depth, shadowbuffer);
        }
        depth.write_tga_file("depth.tga");
    }

    Matrix4f M = ViewPort*Projection*ModelView; //look from light source

    {  //rendering the frame buffer / image
        TGAImage image(width, height, TGAImage::RGB);
        image.flip_vertically();
        lookat(eye, center, up);
        viewport(0, 0, width, height);
        projection(-1.0f / (eye-center).norm());
        
        GouraudShader gouraudShader(Projection * ModelView, (Projection * ModelView).transpose().invert(),  M * (ViewPort * Projection * ModelView).invert());

        for(int i = 0; i<model->nfaces(); i++){
            Vec4f screen_coords[3];
            for(int j = 0; j<3; j++)
                screen_coords[j] = gouraudShader.vertexShader(i, j);
            triangle(screen_coords, gouraudShader, image, zbuffer);
        }
        image.write_tga_file("output.tga");
    }

    delete model;
    delete[] zbuffer;
    delete[] shadowbuffer;
    
    return 0;
}