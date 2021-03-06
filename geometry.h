#ifndef __GEOMETRY_H
#define __GEOMETRY_H
#include<cassert>
#include<iostream>
#include<cmath>

template<int DimRow, int DimCol, typename T> class Mat;

template<int DimRow, typename T> class Vec {
public:
    Vec() { for(int i = DimRow-1; i>=0; i--) data[i]=T(); }
    T& operator[](const int i) { assert(i<DimRow && i>=0); return data[i]; }
    const T& operator[](const int i) const { assert(i<DimRow && i>=0); return data[i]; }
    double norm() { 
        double res = 0.0;
        for(int i = 0; i<DimRow ;i++) res += data[i] * data[i];
        return std::sqrt(res); 
    }
    Vec<DimRow, T> normalize() { *this = (*this)*(1/norm()); return *this; }
    // attention this method is just get the last value to 1 (data[DimRow-1] = 1)
    // i cant think of a better name =,=
    Vec<DimRow, T> homogeneous() { 
        Vec<DimRow, T> res;
        for(int i=0; i<DimRow-1; i++) res[i] = data[i] / data[DimRow-1];
        res[DimRow-1] = T(1);
        return res;
    }
private:
    T data[DimRow];
};

template<typename T> class Vec<2, T> {
public:
    Vec(): x(T()), y(T()) { }
    Vec(T _x, T _y): x(_x), y(_y) { }
    template<typename U> Vec<2, T>(const Vec<2, U> &v);
    T& operator[](const int i) { assert(i<2 && i>=0); return i==0? x : y;}
    const T& operator[](const int i) const { assert(i<2 && i>=0); return i==0? x : y;}
    T x, y;
};

template<typename T> class Vec<3, T> {
public:
    Vec(): x(T()), y(T()), z(T()) { }
    Vec(T _x, T _y, T _z): x(_x), y(_y), z(_z) { }
    template<typename U> Vec<3, T>(const Vec<3, U> &v);
    T& operator[](const int i) { assert(i<3 && i>=0); return (i==0? x : ( i==1? y : z ));}
    const T& operator[](const int i) const { assert(i<3 && i>=0); return (i==0? x : ( i==1? y : z ));}
    double norm() { return std::sqrt(x*x + y*y + z*z); }
    Vec<3, T> normalize() { *this = (*this)*(1/norm()); return *this; }
    T x, y, z;
};

template<int DimRow, typename T> T operator*(const Vec<DimRow, T> &lhs, const Vec<DimRow, T> &rhs){
    T res = T();
    for(int i = DimRow-1; i>=0; i--)
        res += lhs[i] * rhs[i];
    return res;
}

template<int DimRow, typename T, typename U> Vec<DimRow, T> operator*(Vec<DimRow, T> lhs, const U rhs){
    for(int i = DimRow-1; i>=0; i--)
        lhs[i] *= rhs;
    return lhs;
}

template<int DimRow, typename T> Vec<DimRow, T> operator/(Vec<DimRow, T> lhs, const Vec<DimRow, T> rhs){
    for(int i = DimRow-1; i>=0; i--)
        lhs[i] /= rhs[i];
    return lhs;
}

template<int DimRow, typename T> Vec<DimRow, T> operator/(Vec<DimRow, T> lhs, T rhs){
    for(int i = DimRow-1; i>=0; i--)
        lhs[i] /= rhs;
    return lhs;
}

template<int DimRow, typename T> Vec<DimRow, T> operator+(Vec<DimRow, T> lhs, const Vec<DimRow, T> rhs){
    for(int i = DimRow-1; i>=0; i--)
        lhs[i] += rhs[i];
    return lhs;
}

template<int DimRow, typename T> Vec<DimRow, T> operator-(Vec<DimRow, T> lhs, const Vec<DimRow, T> rhs){
    for(int i = DimRow-1; i>=0; i--)
        lhs[i] -= rhs[i];
    return lhs;
}

template<int NewLen, int DimRow, typename T> Vec<NewLen, T> embed(const Vec<DimRow, T> &v, T fill = 1){
    Vec<NewLen, T> res;
    for(int i = NewLen-1; i>=0; i--)
        res[i] = i<DimRow? v[i] : fill;
    return res;
}

template<int NewLen, int DimRow, typename T> Vec<NewLen, T> proj(const Vec<DimRow, T> &v){
    assert(NewLen <= DimRow);
    Vec<NewLen, T> res;
    for(int i = NewLen-1; i>=0; i--) res[i] = v[i];
    return res;
}

template<typename T> Vec<3, T> cross(Vec<3, T> v1, Vec<3, T> v2){
    return Vec<3, T>(v1.y*v2.z-v2.y*v1.z, v1.z*v2.x-v1.x*v2.z, v1.x*v2.y-v1.y*v2.x);
}

template<int DimRow, typename T> std::ostream& operator<<(std::ostream& out, const Vec<DimRow, T> &v){
    for(int i = 0; i<DimRow; i++)
        out << v[i] << " ";
    out << std::endl;
    return out;
}

//c++ template is useful for calculating
//calculate determinant iteratively,more concisely,it is recursive
template<int Dim, typename T> class dt{  //determinant
public:
    static T det(const Mat<Dim, Dim, T> &mat){
        T res = 0;
        for(int i = 0; i<Dim; i++) res += mat[0][i] * mat.cofactor(0, i);
        return res;
    }
};

//define the base case
template<typename T> class dt<1, T>{
public:
    static T det(const Mat<1, 1, T> &mat){
        return mat[0][0];
    }
};

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
        assert(i<DimRow && i>=0);
        return data[i];
    }

    const Vec<DimCol, T>& operator[] (const int i) const{
        assert(i<DimRow && i>=0);
        return data[i];
    }

    const Vec<DimRow, T> col(const int j) const{
        assert(j<DimCol && j>=0);
        Vec<DimRow, T> res;
        for(int i = 0; i<DimRow; i++) res[i] = data[i][j];
        return res;
    }

    const void set_col(const int col, const Vec<DimRow, T> &v){
        assert(col<DimCol && col>=0);
        for(int i = 0; i<DimRow; i++) data[i][col] = v[i];
    }

    T det() const { //determinant
        return dt<DimCol, T>::det(*this);
    }

    Mat<DimRow, DimCol, T> transpose(){
        Mat<DimCol, DimRow, T> res;
        for(int i = 0; i<DimCol; i++)
            for(int j = 0; j<DimRow; j++)
                res[i][j] = (*this)[j][i];
        return res;
    }

    Mat<DimRow-1, DimCol-1, T> get_minor(int row, int col) const { // delete one row and one column from matrix
        Mat<DimRow-1, DimCol-1, T> res;
        for(int i = 0; i<DimRow-1; i++)
            for(int j = 0; j<DimCol-1; j++)
                res[i][j] = data[i<row? i : i+1][j<col? j : j+1];
        return res;
    }

    T cofactor(int row, int col) const { // M_{ij} Algebraic cofactor
        return get_minor(row, col).det() * ((row + col) % 2? -1 : 1);
    }

    // T cofactor(size_t row, size_t col) const {
    //     return get_minor(row,col).det()*((row+col)%2 ? -1 : 1);
    // }

    Mat<DimRow, DimCol, T> adjugate() const { //A* adjugate matrix
        Mat<DimRow, DimCol, T> res;
        for(int i = 0; i<DimRow; i++)
            for(int j = 0; j<DimCol; j++)
                res[i][j] = cofactor(j, i);
        return res;
    }

    // mat<DimRows,DimCols,T> adjugate() const {  // A*
    //     mat<DimRows,DimCols,T> ret;
    //     for (size_t i=DimRows; i--; )
    //         for (size_t j=DimCols; j--; ret[i][j]=cofactor(i,j));
    //     return ret;
    // }

    Mat<DimRow, DimCol, T> invert() {   // A.invert = A* / |A|
        assert(DimRow == DimCol);
        Mat<DimRow, DimCol, T> res = adjugate();
        res = res / (res.col(0) * data[0]);
        return res;
    }

    // mat<DimRows,DimCols,T> invert_transpose() { // A.invert = A* / |A|
    //     mat<DimRows,DimCols,T> ret = adjugate();
    //     T tmp = ret[0]*rows[0];
    //     return ret/tmp;
    // }
};

template<int DimRow, int DimCol, typename T> Vec<DimRow, T> operator*(const Mat<DimRow, DimCol, T> &matrix, const Vec<DimCol, T> &vec){
    Vec<DimRow, T> res;
    for(int i = 0; i<DimRow; i++) res[i] = matrix[i] * vec;
    return res;
}

template<int m, int n, int t, typename T> Mat<m, t, T> operator*(const Mat<m, n, T> &lhs, const Mat<n, t, T> &rhs){
    Mat<m, t, T> res;
    for(int i = 0; i<m; i++)
        for(int j = 0; j<t; j++)
            res[i][j] = lhs[i] * rhs.col(j);
    return res;
}

template<int DimRow, int DimCol, typename T> std::ostream& operator<<(std::ostream& out, Mat<DimRow, DimCol, T> &mat){
    for(int i = 0; i<DimRow; i++)
        out << mat[i];
    out << std::endl;
    return out;
}

template<int DimRow, int DimCol, typename T> Mat<DimRow, DimCol, T> operator/(Mat<DimRow, DimCol, T> lhs, const T& rhs){
    for(int i = 0; i<DimRow; i++)
        lhs[i] = lhs[i] / rhs;
    return lhs;
}

typedef Vec<2, float> Vec2f;
typedef Vec<2, int> Vec2i;
typedef Vec<3, float> Vec3f;
typedef Vec<3, int> Vec3i;
typedef Vec<4, float> Vec4f;
typedef Mat<4, 4, float> Matrix4f;

#endif