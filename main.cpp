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
    Vec3f varying_intensity; //vertext shader interpolate intensity fragment shader use it

    virtual Vec4f vertexShader(int iface, int nthvert){
        Vec4f gl_vertex = embed<4>(model->vert(iface,nthvert));
        gl_vertex = ViewPort * Projection * ModelView * gl_vertex;
        varying_intensity[nthvert] = std::max(0.0f, model->normal(iface, nthvert) * light_dir);
        return gl_vertex;
    }

    virtual bool fragmentShader(Vec3f baryCoordinate, TGAColor &color){
        float intensity = varying_intensity * baryCoordinate;
        color = TGAColor(255, 255, 255) * intensity;
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
    // Vec3f screen_coords[3];
    // Vec3f world_coords[3];
    // Vec3f text_coords[3];
    // Vec3f v;
    // Vec3f t;
    // Vec3f light_dir = Vec3f(0,0,-1);
    // float *zbuffer = new float[width*height];
    // for(int i=0;i<width*height;i++)zbuffer[i]=-std::numeric_limits<float>::max();
    

    // Matrix4f modelView = lookat(eye,center,Vec3f(0, 1, 0));
    // Matrix4f projection = Matrix4f::identity();
    // Matrix4f viewPort = viewport(0, 0, width, height);
    // projection[3][2] = -1.0f / (eye-center).norm();
    // for(int i=0;i<model->nfaces();i++){
    //     std::vector<int> face = model->face(i);  //get every face
    //     for(int j=0;j<3;j++){
    //         v = model->vert(face[j]);
    //         t = model->uv(i,j);

    //         world_coords[j] = v;
    //         v = embed<3>((viewPort * projection * modelView * embed<4>(v)).homogeneous());
    //         screen_coords[j] = v;
    //         screen_coords[j].x = int(screen_coords[j].x+0.5f);
    //         screen_coords[j].y = int(screen_coords[j].y+0.5f);
    //         // screen_coords[j] = Vec3f(int((v.x+1.0f)*width/2.0f+0.5f),int((v.y+1.0f)*height/2.0f+0.5f),v.z);
            
    //         t.x *= texture.get_width();
    //         t.y *= texture.get_height();
    //         text_coords[j] = t;
    //     }
    //     Vec3f n = cross(world_coords[2]-world_coords[0],world_coords[1]-world_coords[0]);
    //     n.normalize();
    //     float intensity = n * light_dir;
    //     if(intensity>0){
    //         // triangle(screen_coords, zbuffer, image, TGAColor(intensity*255,intensity*255,intensity*255,255));
    //         triangle(screen_coords, zbuffer, image, texture, text_coords, intensity);
    //     }  
    // }
    // image.write_tga_file("output_illuminated_wireframe.tga");
    return 0;
}