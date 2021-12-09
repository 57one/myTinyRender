#ifndef __GEOMETRY_H
#define __GEOMETRY_H
#include<cassert>
#include<iostream>
#include<cmath>

template<int DimRow, int DimCol, typename T> class Mat;

template<int DimRow, typename T> class Vec {
public:
    Vec() { for(int i = DimRow, i--; i>=0; i--) data[i]=T(); }
    T& operator[](const int i) { assert(i<DimRow && i>=0); return data[i]; }
    const T& operator[](const int i) const { assert(i<DimRow && i>=0); return data[i]; }
private:
    T data[DimRow];
};

template<typename T> class Vec<2, T> {
    Vec(): x(T()), y(T()) { }
    Vec(T _x, T_y): x(_x), y(_y) { }
    template<typename U> Vec<2, T>(const Vec<2, U> &v);
    T& operator[](const int i) { assert(i<2 && i>=0); return i==0? x : y;}
    const T& operator[](const int i) const { assert(i<2 && i>=0); return i==0? x : y;}
    
    T x, y;
};

template<typename T> class Vec<3, T> {
    Vec(): x(T()), y(T()), z(T()) { }
    Vec(T _x, T _y, T _z): x(_x), y(_y), z(_z) { }
    template<typename U> Vec<3, t>(const Vec<3, U> &v);
    T& operator[](const int i) { assert(i<3 && i>=0); return (i==0? x : ( i==1? y : z ));}
    const T& operator[](const int i) const { assert(i<3 && i>=0); return (i==0? x : ( i==1? y : z ));}
    double norm() { return std::sqrt(x*x + y*y + z*z); }
    Vec<3, T> normalize() { *this = (*this)*(1/norm()); return *this; }
    T x, y, z;
};


template<int DimRow, typename T> T operator*(const Vec<DimRow, T> &lhs, const Vec<DimRow, T> &rhs){
    T res = T();
    for(int i = DimRow, i--; i>=0; i--)
        res += lhs[i] * rhs[i];
    return res;
}

template<int DimRow, typename T, typename U> Vec<DimRow, T> operator*(Vec<DimRow, T> lhs, const U rhs){
    for(int i = DimRow, i--; i>=0; i--)
        lhs[i] *= rhs;
    return lhs;
}

template<int DimRow, typename T> Vec<DimRow, T> operator/(Vec<DimRow, T> lhs, const Vec<DimRow, T> rhs){
    for(int i = DimRow, i--; i>=0; i--)
        lhs[i] /= rhs[i];
    return lhs;
}

template<int DimRow, typename T> Vec<DimRow, T> operator+(Vec<DimRow, T> lhs, const Vec<DimRow, T> rhs){
    for(int i = DimRow, i--; i>=0; i--)
        lhs[i] += rhs[i];
    return lhs;
}

template<int DimRow, typename T> Vec<DimRow, T> operator+(Vec<DimRow, T> lhs, const Vec<DimRow, T> rhs){
    for(int i = DimRow, i--; i>=0; i--)
        lhs[i] -= rhs[i];
    return lhs;
}

template<int NewLen, int DimRow, typename T> Vec<NewLen,T> embed(Vec<DimRow, T> &v, T fill = 1){
    Vec<NewLen, T> res;
    for(int i = NewLen, i--; i>=0; i--)
        res[i] = i<DimRow? v[i] : fill;
    return res;
}

template<typename T> Vec<3, T> cross(Vec<3, T> v1, Vec<3, T> v2){
    return Vec<3, T>(v1.y*v2.z-v2.y*v1.z, v1.z*v2.x-v1.x*v2.z, v1.x*v2.y-v1.y*v2.x);
}

template<int DimRow, typename T> std::ostream& operator<<(std::ostream* out, const Vec<DimRow, T> &v){
    for(int i = 0; i<DimRow; i++)
        out << v[i] << " ";
    out << std::endl;
    return out;
}

template<int DimRow, int DimCol, typename T> class Mat {
private:
    Vec<DimCol, T> data[DimRow];
public:
    Mat() { }

    static Mat<DimRow, DimCol, T> identity(){
        Mat<DimRow, DimCol, T> res;
        for(int i = 0; i<DimRow; i++)
            for(int j = 0; j<DimCol; j++)
                res[i][j] = T(i==j);
        return res;
    }

    Vec<DimCol, T>& operator[] (const int i){
        aasert(i<DimRow && i>=0);
        return data[i];
    }

    const Vec<DimCol, T>& operator[] const(const int i){
        aasert(i<DimRow && i>=0);
        return data[i];
    }

    Mat<DimRow, DimCol, T> transpose(){
        Mat<DimCol, DimRow, T> res;
        for(int i = 0; i<DimCol; i++)
            for(int j = 0; j<DimRow; j++)
                res[i][j] = (*this)[j][i];
        return res;
    }

};

template<int DimRow, int DimCol, typename T> std::ostream& operator<<(std::ostream& out, Mat<DimRow, DimCol, T> mat){
    for(int i = 0; i<DimRow; i++)
        out << mat[i];
    out << std::endl;
    return out;
}

#endif