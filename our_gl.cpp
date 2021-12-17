#include<limits>
#include"our_gl.h"
#include"geometry.h"

Matrix4f ModelView;
Matrix4f Projection;
Matrix4f ViewPort;

void lookat(Vec3f eye, Vec3f center, Vec3f up){
    Vec3f z = (eye-center).normalize();
    Vec3f x = cross(up,z).normalize();
    Vec3f y = cross(z,x).normalize();
    Matrix4f rotateInv = Matrix4f::identity();
    Matrix4f translation = Matrix4f::identity();
    for (int i=0; i<3; i++) {
        rotateInv[0][i] = x[i];
        rotateInv[1][i] = y[i];
        rotateInv[2][i] = z[i];
        translation[i][3] = -center[i];
    }
    ModelView = rotateInv * translation;
}

void projection(float coff){
    Projection = Matrix4f::identity();
    Projection[3][2] = coff;
}

// void viewportDepth(int x, int y, int w, int h){
//     ViewPort = Matrix4f::identity();
//     ViewPort[0][3] = x+w/2.f;
//     ViewPort[1][3] = y+h/2.f;
//     ViewPort[2][3] = 255.f/2.f;

//     ViewPort[0][0] = w/2.f;
//     ViewPort[1][1] = h/2.f;
//     ViewPort[2][2] = 255.f/2.f;
// }

void viewport(int x, int y, int w, int h){
    ViewPort = Matrix4f::identity();
    ViewPort[0][3] = x+w/2.f;
    ViewPort[1][3] = y+h/2.f;
    ViewPort[2][3] = depth/2.f;

    ViewPort[0][0] = w/2.f;
    ViewPort[1][1] = h/2.f;
    ViewPort[2][2] = depth/2.f;
}

Vec3f barycentric(Vec3f *pts,Vec3f P) {
    Vec3f vec = cross(Vec3f(pts[1].x-pts[0].x,pts[2].x-pts[0].x,pts[0].x-P.x),Vec3f(pts[1].y-pts[0].y,pts[2].y-pts[0].y,pts[0].y-P.y));
    /* `pts` and `P` has integer value as coordinates
    so `abs(u[2])` < 1 means `u[2]` is 0, that means
    triangle is degenerate, in this case return something with negative coordinates */
    if(std::abs(vec.z) > 1e-2) 
        return Vec3f(1.0f-vec.x/vec.z-vec.y/vec.z,vec.x/vec.z,vec.y/vec.z);
    return Vec3f(-1,1,1);
}

void triangle(Vec4f *pts, IShader &shader, TGAImage &image, float *zbuffer) {
     //bounding box
    Vec2f bboxmin(std::numeric_limits<float>::max(),std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max());
    Vec2f clamp(image.get_width()-1,image.get_height()-1); //backup
    //calcuate bounding box
    for(int i=0;i<3;i++){
        for(int j=0;j<2;j++){
            bboxmin[j]=std::max(0.0f,std::min(bboxmin[j],pts[i][j] / pts[i][3]));
            bboxmax[j]=std::min(clamp[j],std::max(bboxmax[j], pts[i][j] / pts[i][3]));
        }
    }
    Vec3i P;
    TGAColor color;
    for(P.x=bboxmin.x;P.x<=bboxmax.x;P.x++){
        for(P.y=bboxmin.y;P.y<=bboxmax.y;P.y++){
            Vec3f pts_homogeneous[3] = {
                proj<3>(pts[0] / pts[0][3]),
                proj<3>(pts[1] / pts[1][3]),
                proj<3>(pts[2] / pts[2][3])
            };
            Vec3f baryCoordinate = barycentric(pts_homogeneous, P);

            float z = pts[0][2] * baryCoordinate.x  + pts[1][2] * baryCoordinate.y + pts[2][2] * baryCoordinate.z;
            float w = pts[0][3] * baryCoordinate.x + pts[1][3] * baryCoordinate.y + pts[2][3] * baryCoordinate.z;
            int depth = z / w;
            int idx = P.x+P.y*image.get_width();
            if(baryCoordinate.x<0 || baryCoordinate.y<0 || baryCoordinate.z<0 || zbuffer[P.x+P.y*image.get_width()] > depth) continue;
            
            bool discard = shader.fragmentShader(baryCoordinate, color); 
            if(!discard){
                image.set(P.x, P.y, color);
                zbuffer[P.x+P.y*image.get_width()] = depth;
            }
        }
    }
}
