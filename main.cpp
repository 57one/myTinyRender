#include<iostream>
#include<limits>
#include"tgaimage.h"
#include"geometry.h"
#include"model.h"

const TGAColor red = TGAColor(255,0,0,255);

const int width = 800;
const int height = 800;

Vec3f barycentric(Vec3f *pts,Vec3f P) {
    Vec3f vec = cross(Vec3f(pts[1].x-pts[0].x,pts[2].x-pts[0].x,pts[0].x-P.x),Vec3f(pts[1].y-pts[0].y,pts[2].y-pts[0].y,pts[0].y-P.y));
    /* `pts` and `P` has integer value as coordinates
    so `abs(u[2])` < 1 means `u[2]` is 0, that means
    triangle is degenerate, in this case return something with negative coordinates */
    if(std::abs(vec.z) > 1e-2) 
        return Vec3f(1.0f-vec.x/vec.z-vec.y/vec.z,vec.x/vec.z,vec.y/vec.z);
    return Vec3f(-1,1,1);
}

void triangle(Vec3f *pts, float *zbuffer, TGAImage &image, TGAColor color){
    //bounding box
    Vec2f bboxmin(std::numeric_limits<float>::max(),std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max());
    Vec2f clamp(image.get_width()-1,image.get_height()-1); //backup
    //calcuate bounding box
    for(int i=0;i<3;i++){
        for(int j=0;j<2;j++){
            bboxmin[j]=std::max(0.0f,std::min(bboxmin[j],pts[i][j]));
            bboxmax[j]=std::min(clamp[j],std::max(bboxmax[j],pts[i][j]));
        }
    }
    Vec3f P;
    for(P.x=bboxmin.x;P.x<=bboxmax.x;P.x++){
        for(P.y=bboxmin.y;P.y<=bboxmax.y;P.y++){
            Vec3f baryCoordinate = barycentric(pts,P);
            if(baryCoordinate.x<0 || baryCoordinate.y<0 || baryCoordinate.z<0) continue;
            P.z = 0.0f;
            for(int i = 0; i<3; i++) {
                P.z += baryCoordinate[i] * pts[i].z;
            }
            if(zbuffer[int(P.x + P.y*width)] < P.z){
                zbuffer[int(P.x+P.y*width)] = P.z;
                image.set(P.x,P.y,color);
            }  
        }
    }
}

void triangle(Vec3f *pts, float *zbuffer, TGAImage &image, TGAImage &texture, Vec3f *text_coords, float intensity){
    //bounding box
    Vec2f bboxmin(std::numeric_limits<float>::max(),std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max());
    Vec2f clamp(image.get_width()-1,image.get_height()-1); //backup
    //calcuate bounding box
    for(int i=0;i<3;i++){
        for(int j=0;j<2;j++){
            bboxmin[j]=std::max(0.0f,std::min(bboxmin[j],pts[i][j]));
            bboxmax[j]=std::min(clamp[j],std::max(bboxmax[j],pts[i][j]));
        }
    }
    Vec3f P;
    Vec3f T;
    for(P.x=bboxmin.x;P.x<=bboxmax.x;P.x++){
        for(P.y=bboxmin.y;P.y<=bboxmax.y;P.y++){
            Vec3f baryCoordinate = barycentric(pts,P);
            if(baryCoordinate.x<0 || baryCoordinate.y<0 || baryCoordinate.z<0) continue;
            P.z = 0.0f;
            T.x = T.y = T.z = 0.0f;
            for(int i = 0; i<3; i++) {
                P.z += baryCoordinate[i] * pts[i].z;
                T.x += baryCoordinate[i] * text_coords[i].x;
                T.y += baryCoordinate[i] * text_coords[i].y;
            }
            if(zbuffer[int(P.x + P.y*width)] < P.z){
                zbuffer[int(P.x+P.y*width)] = P.z;
                image.set(P.x,P.y,texture.get(T.x, T.y)*intensity);
            }  
        }
    }
}

int main(){
    TGAImage image(width,height,TGAImage::RGB);
    TGAImage texture;
    texture.read_tga_file("obj/african_head_diffuse.tga");
    texture.flip_vertically();
    image.flip_vertically();

    Vec3f screen_coords[3];
    Vec3f world_coords[3];
    Vec3f text_coords[3];
    Vec3f v;
    Vec3f t;
    Vec3f light_dir = Vec3f(0,0,-1);
    float *zbuffer = new float[width*height];
    for(int i=0;i<width*height;i++)zbuffer[i]=-std::numeric_limits<float>::max();
    Model *model = new Model("obj/african_head.obj");
    for(int i=0;i<model->nfaces();i++){
        std::vector<int> face = model->face(i);  //get every face
        for(int j=0;j<3;j++){
            v = model->vert(face[j]);
            t = model->uv(i,j);

            screen_coords[j] = Vec3f(int((v.x+1.0f)*width/2.0f+0.5f),int((v.y+1.0f)*height/2.0f+0.5f),v.z);
            world_coords[j] = v;
            
            t.x *= texture.get_width();
            t.y *= texture.get_height();
            text_coords[j] = t;
        }
        Vec3f n = cross(world_coords[2]-world_coords[0],world_coords[1]-world_coords[0]);
        n.normalize();
        float intensity = n * light_dir;
        if(intensity>0){
            // triangle(screen_coords, zbuffer, image, TGAColor(intensity*255,intensity*255,intensity*255,255));
            triangle(screen_coords, zbuffer, image, texture, text_coords, intensity);
        }  
    }
    image.write_tga_file("output_illuminated_wireframe.tga");
    return 0;
}