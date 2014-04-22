#include <iostream>

#include "matrix.h"
#include "vector.h"

//Adapted from RPI Graphics hw3

//4 x 4 matrices
//Includes useful operators and ways to make common types of Matrices

//copies data to new matrix
Matrix::Matrix(const Matrix &m){
	for(int i=0;i<16;++i){
		data[i]=m.data[i];
	}
}

//copies data to new matrix
Matrix& Matrix::operator=(const Matrix &m){
	for(int i=0;i<16;++i){
		data[i]=m.data[i];
	}
	return (*this);
}

//Sets to identity
void Matrix::setToIdentity() {
	for(int row = 0;row < 4;++row){
		for(int col = 0;col < 4;++col){
			set(row,col,(row==col));
		}
	}
}

//Sets to all zeros
void Matrix::clear() {
	for(int row = 0;row < 4;++row){
		for(int col = 0;col < 4;++col){
			set(row,col,0);
		}
	}
}

//Adds two matrices
Matrix operator+(const Matrix &m1,const Matrix &m2){
	Matrix ans;
	for(int i=0;i<16;++i){
		ans.data[i]=m1.data[i]+m2.data[i];
	}
	return ans;
}

//Takes difference between two differences
Matrix operator-(const Matrix &m1,const Matrix &m2){
	Matrix ans;
	for(int i=0;i<16;++i){
		ans.data[i]=m1.data[i]-m2.data[i];
	}
	return ans;
}

//Matrix multiplication between two matrices
Matrix operator*(const Matrix &m1,const Matrix &m2){
	Matrix ans;
	for(int row=0;row<4;++row){
		for(int col=0;col<4;++col){
			for(int i=0;i<4;++i){
				ans.set(row,col,ans.get(row,col)+m1.get(row,i)*m2.get(i,col));
			}
		}
	}
	return ans;
}

//Matrix multiplication with a vector in homogenous coordinates
Vec operator*(const Matrix &m1,const Vec &v){
	Vec ans = v;
	m1.Transform(ans);
	return ans;
}

//Scales the matrix by the constant
Matrix operator*(const Matrix &m1,double v){
	Matrix ans;
	for(int i=0;i<16;++i){
		ans.data[i]=m1.data[i]*v;
	}
	return ans;
}

//Makes a translation matrix in the direction of the vector
Matrix Matrix::MakeTranslation(const Vec &v){
	Matrix t;
	t.setToIdentity();
	t.set(0,3,v.x());
	t.set(1,3,v.y());
	t.set(2,3,v.z());
	return t;
}

//Makes a rotation around a given axis
Matrix Matrix::MakeAxisRotation(const Vec &v,double theta){
	Matrix r;
	r.setToIdentity();

	double x = v.x();
	double y = v.y();
	double z = v.z();

	double c= (double)cosf(float(theta));
	double s= (double)sinf(float(theta));
	double xx=x*x;
	double xy=x*y;
	double xz=x*z;
	double yy=y*y;
	double yz=y*z;
	double zz=z*z;

	r.set(0,0,(1-c)*xx+c);
	r.set(1,0,(1-c)*xy+z*s);
	r.set(2,0,(1-c)*xz-y*s);

	r.set(0,1,(1-c)*xy-z*s);
	r.set(1,1,(1-c)*yy+c);
	r.set(2,1,(1-c)*yz+x*s);

	r.set(0,2,(1-c)*xz+y*s);
	r.set(1,2,(1-c)*yz-x*s);
	r.set(2,2,(1-c)*zz+c);

	return r;
}

//Makes a matrix of the outerproduct of the vector
Matrix Matrix::MakeOuterProduct(const Vec &v){
	Matrix op;
	for(int i=0;i<3;++i){
		for(int j=0;j<3;++j){
			op.set(i,j,v[i]*v[j]);
		}
	}
	return op;
}

//Transforms the vector by the matrix
void Matrix::Transform(Vec &v) const {
	double vw[4]={v.x(),v.y(),v.z(),1.0};
	double ans[4]={0.0,0.0,0.0,0.0};
	for(int row = 0;row<4;++row){
		for(int col=0;col<4;++col){
			ans[row] += get(row,col) * vw[col];
		}
	}
	v=Vec(ans[0]/ans[3],ans[1]/ans[3],ans[2]/ans[3]);
}

//Outputs matrix
std::ostream& operator<<(std::ostream &ostr, const Matrix &m){
	for(int r=0;r<4;++r){
		for(int c=0;c<4;++c){
			ostr<<m.get(r,c);
			if(c!=3){
				ostr<<" ";
			}else{
				ostr<<"\n";
			}
		}
	}
	return ostr;
}

//Finds determinant by finding determinant of sections of matrix
double Matrix::det3x3(double a1,double a2,double a3,double b1,double b2,double b3,double c1,double c2,double c3){
	return a1 * det2x2(b2,b3,c2,c3) - b1 * det2x2(a2,a3,c2,c3) + c1 * det2x2(a2,a3,b2,b3);
}

//Finds determinant of a 2x2 matrix
double Matrix::det2x2(double a,double b,double c,double d){
	return a * d - b * c;
}
