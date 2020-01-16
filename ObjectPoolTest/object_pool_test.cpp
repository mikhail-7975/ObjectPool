#include "gtest/gtest.h"
#include"Object_Pool.h"

struct Point {
	int _x, _y;
public:
	Point() : _x(0), _y(0) {}   // конструктор без параметров
		
	Point(int x, int y) : _x(x), _y(y) {}   // конструктор с параметрами
		
	~Point() {
		_x = 0;
		_y = 0;
	}
	int x() {
		return _x;
	}
	int y() {
		return _y;
	}
};

const size_t size = 10;

TEST(alloc,with_default_constructor) {
	ObjectPool<Point> pointPool(10);    
	Point* p1 = pointPool.alloc();
	EXPECT_EQ(pointPool.usedElementsCount(), 1);
}

TEST(alloc, with_constructor) {
	ObjectPool<Point> pointPool(10); 
	Point* p2 = pointPool.alloc(0, 1);
	EXPECT_EQ(pointPool.usedElementsCount(), 1);
}

TEST(alloc, add_extra_el) {
	ObjectPool<Point> pointPool(1);
	Point* p2 = pointPool.alloc(0, 1);
	EXPECT_ANY_THROW(pointPool.alloc());
}

TEST(free, free_exist_element) {
	ObjectPool<Point> pointPool(10);
	Point* p2 = pointPool.alloc(0, 1);
	pointPool.free(p2);
	EXPECT_EQ(pointPool.usedElementsCount(), 0);
}

TEST(free, alloc_2_free_exist_element) {
	ObjectPool<Point> pointPool(10);
	Point* p1 = pointPool.alloc(2, 1);
	Point* p2 = pointPool.alloc(0, 1);
	pointPool.free(p2);
	EXPECT_EQ(pointPool.usedElementsCount(), 1);
}

TEST(free, free_deleted_element) {
	ObjectPool<Point> pointPool(10);
	Point* p1= pointPool.alloc(2, 1);
	Point* p2 = pointPool.alloc(0, 1);
	pointPool.free(p2); 
	pointPool.free(p2);
	EXPECT_EQ(pointPool.usedElementsCount(), 1);
}

TEST(alloc_2, alloc_2_elements) {
	ObjectPool<Point> pointPool(10);
	Point* p1 = pointPool.alloc(2, 1);
	Point* p2 = pointPool.alloc(0, 1);
	EXPECT_EQ(pointPool.usedElementsCount(), 2);
}

TEST(alloc_2, alloc_Free_alloc_elements) {
	ObjectPool<Point> pointPool(2);
	Point* p1 = pointPool.alloc(2, 1);
	Point* p2 = pointPool.alloc(0, 1);
	pointPool.free(p2);
	Point* p3 = pointPool.alloc();
	EXPECT_EQ(pointPool.usedElementsCount(), 2);
}

TEST(alloc_2, alloc_2_Free_2_alloc_elements) {
	ObjectPool<Point> pointPool(2);
	Point* p1 = pointPool.alloc(2, 1);
	Point* p2 = pointPool.alloc(0, 1);
	pointPool.free(p2);
	pointPool.free(p1);
	Point* p3 = pointPool.alloc();
	EXPECT_EQ(pointPool.usedElementsCount(), 1);
}

TEST(alloc_10, alloc_10) {
	ObjectPool<Point> pointPool(10);
	Point* pointArray[size];
	for (size_t i = 0; i < size; i++) {
		pointArray[i] = pointPool.alloc(2, 1);
	}
	EXPECT_EQ(pointPool.usedElementsCount(), 10);
}

TEST(alloc_10, alloc_10_allloc_1) {
	ObjectPool<Point> pointPool(10);
	Point* pointArray[size];
	for (size_t i = 0; i < size; i++) {
		pointArray[i] = pointPool.alloc(2, 1);
	}
	EXPECT_ANY_THROW(pointPool.alloc());
}

TEST(alloc_10, alloc_10_free_10) {
	ObjectPool<Point> pointPool(10);
	Point* pointArray[size];
	for (size_t i = 0; i < size; i++) {
		pointArray[i]= pointPool.alloc(2, 1);
	}
	for (size_t i = 0; i < size; i++) {
		pointPool.free(pointArray[i]);
	}
	EXPECT_EQ(pointPool.usedElementsCount(), 0);
}

TEST(free, deleted_medium_el) {
	ObjectPool<Point> pointPool(10);
	Point* pointArray[size];
	for (size_t i = 0; i < size; i++) {
		pointArray[i] = pointPool.alloc(2, 1);
	}
	for (size_t i = size/2; i < size; i++) {
		pointPool.free(pointArray[i]);
	}
	EXPECT_EQ(pointPool.usedElementsCount(), size-size/2);
}

TEST(alloc_10, alloc_10_free_10_alloc_10) {
	ObjectPool<Point> pointPool(10);
	Point* pointArray[size];
	for (size_t i = 0; i < size; i++) {
		pointArray[i] = pointPool.alloc(2, 1);
		EXPECT_EQ((*pointArray[i]).x(), 2);
		EXPECT_EQ((*pointArray[i]).y(), 1);
	}
	for (size_t i = 0; i < size; i++) {
		pointPool.free(pointArray[i]);
	}
	for (size_t i = 0; i < size; i++) {
		pointArray[i] = pointPool.alloc(0, 1);
		EXPECT_EQ((*pointArray[i]).x(),0);
		EXPECT_EQ((*pointArray[i]).y(), 1);
	}
	EXPECT_EQ(pointPool.usedElementsCount(), 10);
}

TEST(free, free_alien_elements) {
	ObjectPool<Point> pointPool(1);
	ObjectPool<Point> pointPool2(1);
	Point* p1 = pointPool.alloc(2, 1);
	Point* p2 = pointPool2.alloc(0, 1);
	pointPool.free(p2);
	EXPECT_EQ(pointPool.usedElementsCount(),1);
}

class Matrix final {
private:
	double* matrix;
	size_t Cols;
	size_t Rows;
public:
	Matrix()
	{
		Cols = 0;
		Rows = 0;
		matrix = nullptr;
	};
	Matrix(size_t rows, size_t cols) {
		Rows = rows;
		Cols = cols;
		matrix = new double[Cols * Rows];

	};
	Matrix(const Matrix& mat)
	{
		Cols = mat.Cols+1;
		Rows = mat.Rows+2;
		matrix = new double[Cols * Rows];
		memcpy(matrix, mat.matrix, (Cols * Rows) * sizeof(double));
	}
	~Matrix() {
		delete[] matrix;
	};
	size_t Matrix::rows() const {
		return Rows;
	}
	size_t Matrix::cols() const {
		return Cols;
	}
};

TEST(matrix, alloc_default_constr_bad_copy) {
	ObjectPool<Matrix> matrixPool(size);
	Matrix* my_matrix[size];
	for (size_t i = 0; i < size; i++) {
		my_matrix[i] = matrixPool.alloc();
	}
	EXPECT_EQ((*my_matrix[5]).rows(), (*my_matrix[5]).cols());
	EXPECT_EQ(matrixPool.usedElementsCount(), 10);
}
TEST(matrix, alloc_constr_bad_copy) {
	ObjectPool<Matrix> matrixPool(size);
	Matrix* my_matrix[size];
	for (size_t i = 0; i < size; i++) {
		my_matrix[i] = matrixPool.alloc(2,2);
	}
	EXPECT_EQ((*my_matrix[5]).rows(), (*my_matrix[5]).cols());
	EXPECT_EQ(matrixPool.usedElementsCount(), 10);
}
TEST(matrix, alloc_default_constr) {
	ObjectPool<Matrix> matrixPool(size);
	Matrix* my_matrix[size];
	for (size_t i = 0; i < size; i++) {
		my_matrix[i] = matrixPool.alloc();
	}
	EXPECT_EQ(matrixPool.usedElementsCount(), 10);
}
TEST(matrix, alloc_free_default_constr) {
	ObjectPool<Matrix> matrixPool(size);
	Matrix* my_matrix[size];
	for (size_t i = 0; i < size; i++) {
		my_matrix[i] = matrixPool.alloc();
	}
	for (size_t i = 0; i < size; i++) {
		matrixPool.free(my_matrix[i]);
	}
	EXPECT_EQ(matrixPool.usedElementsCount(), 0);
}
TEST(matrix, alloc_free_alloc_default_constr) {
	ObjectPool<Matrix> matrixPool(size);
	Matrix* my_matrix[size];
	for (size_t i = 0; i < size; i++) {
		my_matrix[i] = matrixPool.alloc();
	}
	for (size_t i = 0; i < size; i++) {
		matrixPool.free(my_matrix[i]);
	}
	for (size_t i = 0; i < size; i++) {
		my_matrix[i] = matrixPool.alloc();
	}
	EXPECT_EQ(matrixPool.usedElementsCount(), 10);
}

TEST(matrix, alloc_free_alloc_constr) {
	ObjectPool<Matrix> matrixPool(size);
	Matrix* my_matrix[size];
	for (size_t i = 0; i < size; i++) {
		my_matrix[i] = matrixPool.alloc(2,5);
	}
	for (size_t i = 0; i < size; i++) {
		matrixPool.free(my_matrix[i]);
	}
	for (size_t i = 0; i < size; i++) {
		my_matrix[i] = matrixPool.alloc(2,5);
	}
	EXPECT_EQ(matrixPool.usedElementsCount(), 10);
}
TEST(matrix, alloc_free_alloc_constr_dif_size) {
	ObjectPool<Matrix> matrixPool(size);
	Matrix* my_matrix[size];
	for (size_t i = 0; i < size; i++) {
		my_matrix[i] = matrixPool.alloc(2, 5);
	}
	for (size_t i = 0; i < size; i++) {
		matrixPool.free(my_matrix[i]);
	}
	for (size_t i = 0; i < size; i++) {
		my_matrix[i] = matrixPool.alloc(1, 5);
	}
	EXPECT_EQ(matrixPool.usedElementsCount(), 10);
}

class defaultNotConstructedObject final {
	size_t size;
public:
	defaultNotConstructedObject() = delete;
	defaultNotConstructedObject(size_t  size) :size(size) {};
	size_t ret_size() {
		return size;
	}
};

TEST(without_default_constr, deleted_medium_el) {
	ObjectPool<defaultNotConstructedObject> noDefCtorObj(10);
	defaultNotConstructedObject* object[size];
	for (size_t i = 0; i < size; i++) {
		object[i] = noDefCtorObj.alloc(2);
	}
	EXPECT_EQ((*object[8]).ret_size(), 2);
	for (size_t i = size / 2; i < size; i++) {
		noDefCtorObj.free(object[i]);
	}
	EXPECT_EQ(noDefCtorObj.usedElementsCount(), size - size / 2);
}
TEST(without_default_constr, alloc_free_alloc_constr) {
	ObjectPool<defaultNotConstructedObject> pointPool(size);
	defaultNotConstructedObject* my_matrix[size];
	for (size_t i = 0; i < size; i++) {
		my_matrix[i] = pointPool.alloc(5);
	}
	for (size_t i = 0; i < size; i++) {
		pointPool.free(my_matrix[i]);
	}
	for (size_t i = 0; i < size; i++) {
		my_matrix[i] = pointPool.alloc(8);
	}
	EXPECT_EQ(pointPool.usedElementsCount(), 10);
}

class with_default_constr final {
	size_t size;
public:
	with_default_constr() :size(0) {};
	with_default_constr(size_t  size)=delete;

};

TEST(with_default_constr, deleted_medium_el_) {
	ObjectPool<with_default_constr> pointPool(10);
	with_default_constr* pointArray[size];
	for (size_t i = 0; i < size; i++) {
		pointArray[i] = pointPool.alloc();
	}
	for (size_t i = size / 2; i < size; i++) {
		pointPool.free(pointArray[i]);
	}
	EXPECT_EQ(pointPool.usedElementsCount(), size - size / 2);
}

TEST(with_default_constr, alloc_free_alloc_constr) {
	ObjectPool<with_default_constr> pointPool(size);
	with_default_constr* my_matrix[size];
	for (size_t i = 0; i < size; i++) {
		my_matrix[i] = pointPool.alloc();
	}
	for (size_t i = 0; i < size; i++) {
		pointPool.free(my_matrix[i]);
	}
	for (size_t i = 0; i < size; i++) {
		my_matrix[i] = pointPool.alloc();
	}
	EXPECT_EQ(pointPool.usedElementsCount(), 10);
}

class Point_1 final {
	int m_x, m_y;
public:
	Point_1()                
		: m_x(0), m_y(0) {};
	Point_1(Point_1&& a) {
		m_x = a.m_x + 2;
		m_y = a.m_y + 2;
	}
	Point_1(const Point_1 & a) {
		m_x = a.m_x + 1;
		m_y = a.m_y + 1;
	}
	int x() const  {
		return m_x;
	}
	int y() const{
		return m_y;
	}
};

TEST(bad_copy, alloc_10_free_10) {
	ObjectPool<Point_1> pointPool(10);
	Point_1* pointArray[size];
	for (size_t i = 0; i < size; i++) {
		pointArray[i] = pointPool.alloc();
	}
	for (size_t i = 0; i < size; i++) {
		pointPool.free(pointArray[i]);
	}
	EXPECT_EQ((*pointArray[5]).x(), 0);
	EXPECT_EQ((*pointArray[5]).y(), 0);
	EXPECT_EQ(pointPool.usedElementsCount(), 0);
}
TEST(cpoint, move_check)
{
	ObjectPool<Point_1> pointPool(10);
	const Point_1 b;
	Point_1* a = pointPool.alloc(Point_1());
	EXPECT_EQ(a->x(), 2);
	EXPECT_EQ(a->y(), 2);
	EXPECT_EQ(b.x(), 0);
	EXPECT_EQ(b.y(), 0);
}

TEST(cpoint, copy_check)
{
	ObjectPool<Point_1> pointPool(10);
	const Point_1 b;
	Point_1* a = pointPool.alloc(b);
	EXPECT_EQ(a->x(), 1);
	EXPECT_EQ(a->y(), 1);
	EXPECT_EQ(b.x(), 0);
	EXPECT_EQ(b.y(), 0);
}
