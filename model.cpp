#include<vector>
#include<iostream>
#include<fstream>
#include<sstream>
#include"model.h"

Model::Model(const char *filename){
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if(in.fail()) return;
    std::string line;
    while(!in.eof()){
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if(!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for(int i = 0; i<3; i++) iss >> v[i];
            verts_.push_back(v);
        } else if (!line.compare(0, 2, "f ")){
            std::vector<Vec3i> face;
            Vec3i temp;
            iss >> trash;
            while(iss >> temp[0] >> trash >> temp[1] >> trash >> temp[2]) {
                for(int i = 0; i<3; i++) temp[i]--;
                face.push_back(temp);
            }
            faces_.push_back(face);
        }
    }
    std::cerr << "# v " << verts_.size() << "f " << faces_.size() << std::endl;
}

Model::~Model(){ }

int Model::nverts(){
    return static_cast<int>(verts_.size());
}

int Model::nfaces(){
    return static_cast<int>(faces_.size());
}

Vec3f Model::vert(int i){
    return verts_[i];
}

std::vector<int> Model::face(int i){
    std::vector<int> vertexs;
    for(int k = 0; k<faces_[i].size(); k++) vertexs.push_back(faces_[i][k][0]);
    return vertexs;
}