#include<iostream>
#include"tgaimage.h"
#include"geometry.h"
#include"model.h"

const TGAColor red = TGAColor(255,0,0,255);

const int width = 800;
const int height = 800;

void line (Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if(std::abs(p0.x-p1.x)<std::abs(p0.y-p1.y)){
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
        steep = true;
    }
    if (p0.x>p1.x) {
        std::swap(p0, p1);
    }

    int dx = p1.x-p0.x;
    int dy = p1.y-p0.y;
    int derror2 = std::abs(dy)*2;
    int error2 = 0;
    int y = p0.y;
    for(int x=p0.x;x<p1.x;x++){
        if(steep){
            image.set(y,x,color);
        }else{
            image.set(x,y,color);
        }
        error2 += derror2;
        if(error2>dx){
            y += (p1.y>p0.y?1:-1);
            error2 -= dx*2;
        }
    }
}

Vec3f barycentric(Vec2i *pts,Vec2i P) {
    Vec3f vec = cross(Vec3f(pts[1].x-pts[0].x,pts[2].x-pts[0].x,pts[0].x-P.x),Vec3f(pts[1].y-pts[0].y,pts[2].y-pts[0].y,pts[0].y-P.y));
    /* `pts` and `P` has integer value as coordinates
    so `abs(u[2])` < 1 means `u[2]` is 0, that means
    triangle is degenerate, in this case return something with negative coordinates */
    if(std::abs(vec.z) < 1) return Vec3f(-1,1,1);
    return Vec3f(1.0f-vec.x/vec.z-vec.y/vec.z,vec.x/vec.z,vec.y/vec.z);
}

void triangle(Vec2i *pts,TGAImage &image, TGAColor color){
    //bounding box
    Vec2i bboxmin(image.get_width()-1,image.get_height()-1);
    Vec2i bboxmax(0,0);
    Vec2i clamp(image.get_width()-1,image.get_height()-1); //backup
    //calcuate bounding box
    for(int i=0;i<3;i++){
        for(int j=0;j<2;j++){
            bboxmin[j]=std::max(0,std::min(bboxmin[j],pts[i][j]));
            bboxmax[j]=std::min(clamp[j],std::max(bboxmax[j],pts[i][j]));
        }
    }
    Vec2i P;
    for(P.x=bboxmin.x;P.x<=bboxmax.x;P.x++){
        for(P.y=bboxmin.y;P.y<=bboxmax.y;P.y++){
            Vec3f baryCoodinate = barycentric(pts,P);
            if(baryCoodinate.x<0 || baryCoodinate.y<0 || baryCoodinate.z<0) continue;
            image.set(P.x,P.y,color);
        }
    }
}

int main(){
    TGAImage image(width,height,TGAImage::RGB);
    image.flip_vertically();

    //test lines
    // line(Vec2i(13, 20), Vec2i(80, 40), image, red); 
    // line(Vec2i(20, 13), Vec2i(40, 80), image, red); 
    // image.write_tga_file("output.tga");

    //test triangles
    // Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)}; 
    // Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)}; 
    // Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)}; 
    // triangle(t0, image, red); 
    // triangle(t1, image, red); 
    // triangle(t2, image, red);
    // image.write_tga_file("output_triangles.tga");

    //test color_wireframes
    // Vec2i screen_coords[3];
    // Vec3f world_coords[3];
    // Vec3f v;
    // Model *model = new Model("obj/african_head.obj");
    // for(int i = 0; i<model->nfaces(); i++){
    //     std::vector<int> face = model->face(i);
    //     for(int j = 0; j<3; j++){
    //         v = model->vert(face[j]);
    //         world_coords[j] = v;
    //         screen_coords[j] = Vec2i((v.x+1.0f)*width/2.0f,(v.y+1.0f)*height/2.0f);
    //     }
    //     triangle(screen_coords,image,TGAColor(rand()%255, rand()%255, rand()%255, 255));
    // }
    // image.write_tga_file("output_color_wireframes.tga");

    //test illuminated_wireframes
    Vec2i screen_coords[3];
    Vec3f world_coords[3];
    Vec3f v;
    Vec3f light_dir = Vec3f(0,0,-1);
    Model *model = new Model("obj/african_head.obj");
    for(int i=0;i<model->nfaces();i++){
        std::vector<int> face = model->face(i);  //get every face
        for(int j=0;j<3;j++){
            v = model->vert(face[j]);
            screen_coords[j] = Vec2i((v.x+1.0f)*width/2.0f,(v.y+1.0f)*height/2.0f);
            world_coords[j] = v;
        }
        Vec3f n = cross(world_coords[2]-world_coords[0],world_coords[1]-world_coords[0]);
        n.normalize();
        float intensity = n * light_dir;
        if(intensity>0){
            triangle(screen_coords,image,TGAColor(intensity*255,intensity*255,intensity*255,255));
        }  
    }
    image.write_tga_file("output_illuminated_wireframe.tga");
    return 0;
}