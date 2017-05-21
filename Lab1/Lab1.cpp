// Lab1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "amp.h"
#include "amp_math.h"
#include <ctime>
#include <iostream>
using namespace concurrency;
using namespace std;

void printDevicesInfo();
double sumVectorsAMP(int n, vector<double> a, vector<double> b, vector<double> c);
double sumVectorsOMP(int n, vector<double> a, vector<double> b, vector<double> c);
double sumVectorsSEQ(int n, vector<double> a, vector<double> b, vector<double> c);

double mulMatrixOnConstantAMP(int n, vector<double> a, vector<double> b);
double mulMatrixOnConstantOMP(int n, vector<double> a, vector<double> b);
double mulMatrixOnConstantSEQ(int n, vector<double> a, vector<double> b);

double matrixTransposeAMP(int n, vector<double> a, vector<double> b);
double matrixTransposeOMP(int n, vector<double> a, vector<double> b);
double matrixTransposeSEQ(int n, vector<double> a, vector<double> b);

double mulMatriciesAMP(int n, vector<double> a, vector<double> b, vector<double> c);
double mulMatriciesOMP(int n, vector<double> a, vector<double> b, vector<double> c);
double mulMatriciesSEQ(int n, vector<double> a, vector<double> b, vector<double> c);

void sumVectors();
void mulMatrixOnConstant();
void matrixTranspose();
void mulMatricies();
vector<double> generateVector(int n, bool leaveEmpty = false);

int main()
{
	srand(time(nullptr));
	printDevicesInfo();
	sumVectors();
	mulMatrixOnConstant();
	matrixTranspose();
	mulMatricies();
	return 0;
}
void mulMatricies()
{
	wcout << "\nMul matricies\n";
	int sizes[] = {
		100,
		500,
		1000
	};
	for (int size : sizes)
	{
		wcout << "Size " << size << "x" << size << "\n";
		auto matrixA = generateVector(size * size);
		auto matrixB = generateVector(size * size);
		auto matrixResult = generateVector(size * size, true);
		double elapsedTime = 0;

		for (int i = 0;i < 5;i++)
			elapsedTime += mulMatriciesAMP(size, matrixA, matrixB, matrixResult);
		wcout << "AMP = " << elapsedTime / 5 << " ms\n";
		elapsedTime = 0;

		for (int i = 0;i < 5;i++)
			elapsedTime = mulMatriciesOMP(size, matrixA, matrixB, matrixResult);
		wcout << "OMP = " << elapsedTime / 5 << " ms\n";
		elapsedTime = 0;

		for (int i = 0;i < 5;i++)
			elapsedTime = mulMatriciesSEQ(size, matrixA, matrixB, matrixResult);
		wcout << "SEQ = " << elapsedTime / 5 << " ms\n";
	}
}
void matrixTranspose()
{
	wcout << "\nMatrix transpose\n";
	int sizes[] = {
		100,
		500,
		1000
	};
	for (int size : sizes)
	{
		wcout << "Size " << size << "x" << size << "\n";
		auto matrixA = generateVector(size * size);
		auto matrixResult = generateVector(size * size, true);
		double elapsedTime = 0;

		for (int i = 0;i < 5;i++)
			elapsedTime += matrixTransposeAMP(size, matrixA, matrixResult);
		wcout << "AMP = " << elapsedTime / 5 << " ms\n";
		elapsedTime = 0;

		for (int i = 0;i < 5;i++)
			elapsedTime = matrixTransposeOMP(size, matrixA, matrixResult);
		wcout << "OMP = " << elapsedTime / 5 << " ms\n";
		elapsedTime = 0;

		for (int i = 0;i < 5;i++)
			elapsedTime = matrixTransposeSEQ(size, matrixA, matrixResult);
		wcout << "SEQ = " << elapsedTime / 5 << " ms\n";
	}
}
void mulMatrixOnConstant()
{
	wcout << "\nMul matrix on constant\n";
	int sizes[] = {
		100,
		500,
		1000
	};
	for(int size : sizes)
	{
		wcout << "Size " << size << "x" << size << "\n";
		auto matrixA = generateVector(size * size);
		auto matrixResult = generateVector(size * size, true);
		double elapsedTime = 0;

		for (int i = 0;i < 5;i++)
			elapsedTime += mulMatrixOnConstantAMP(size,  matrixA, matrixResult);
		wcout << "AMP = " << elapsedTime / 5 << " ms\n";
		elapsedTime = 0;

		for (int i = 0;i < 5;i++)
			elapsedTime = mulMatrixOnConstantOMP(size, matrixA, matrixResult);
		wcout << "OMP = " << elapsedTime / 5 << " ms\n";
		elapsedTime = 0;

		for (int i = 0;i < 5;i++)
			elapsedTime = mulMatrixOnConstantSEQ(size, matrixA, matrixResult);
		wcout << "SEQ = " << elapsedTime / 5 << " ms\n";
	}
}

void sumVectors()
{
	wcout << "\nSum of 2 vectors\n";
	int nSizes[] = {1000000, 5000000, 10000000 };
	for (int n : nSizes)
	{
		wcout << "N = " << n << "\n";
		auto vectorA = generateVector(n);
		auto vectorB = generateVector(n);
		auto vectorResult = generateVector(n, true);
		double elapsedTime = 0;

		for (int i = 0;i < 5;i++)
			elapsedTime += sumVectorsAMP(n, vectorA, vectorB, vectorResult);
		wcout << "AMP = " << elapsedTime / 5 << " ms\n";
		elapsedTime = 0;

		for (int i = 0;i < 5;i++)
			elapsedTime = sumVectorsOMP(n, vectorA, vectorB, vectorResult);
		wcout << "OMP = " << elapsedTime / 5 << " ms\n";
		elapsedTime = 0;

		for (int i = 0;i < 5;i++)
			elapsedTime = sumVectorsSEQ(n, vectorA, vectorB, vectorResult);
		wcout << "SEQ = " << elapsedTime / 5 << " ms\n";
	}
}

void printDevicesInfo()
{
	auto devices = accelerator::get_all();
	wcout << "Devices:\n";
	for (auto device : devices)
		wcout << device.description << "\tmemory: " << device.dedicated_memory << "kb\n";
}



double sumVectorsAMP(int n, vector<double> a, vector<double> b, vector<double> c)
{
	array_view<const double> ob1(n, a);
	array_view<const double> ob2(n, b);
	array_view<double> sum(n, c);
	sum.discard_data();
	auto start = clock();
	parallel_for_each(sum.extent, [=](index<1>idx)restrict(amp)
	{
		sum(idx) = ob1[idx] + ob2[idx];
	});
	sum.synchronize();
	auto end = clock();
	return end - start;
}

double sumVectorsOMP(int n, vector<double> a, vector<double> b, vector<double> c)
{
	auto start = clock();
	#pragma omp parallel for
	for (int i = 0; i < n; ++i)
	{	
		c[i] = a[i] + b[i];
	}
	auto end = clock();
	return end - start;
}

double sumVectorsSEQ(int n, vector<double> a, vector<double> b, vector<double> c)
{
	auto start = clock();
	for (int i = 0; i < n; ++i)
	{
		c[i] = a[i] + b[i];
	}
	auto end = clock();
	return end - start;
}


#define CONSTANT 7
double mulMatrixOnConstantAMP(int n, vector<double> a, vector<double> b)
{
	array_view<const double, 2> ob1(n, n, a);
	array_view<double, 2> mul(n, n, b);
	mul.discard_data();
	auto start = clock();
	parallel_for_each(mul.extent, [=](index<2>idx)restrict(amp)
	{
		mul(idx) = ob1(idx) * CONSTANT;
	});
	mul.synchronize();
	auto end = clock();
	return end - start;
}

double mulMatrixOnConstantOMP(int n, vector<double> a, vector<double> b)
{
	auto start = clock();
    #pragma omp parallel for
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			b[i*n + j] = a[i*n + j] * CONSTANT;
		}
	}
	auto end = clock();
	return end - start;
}

double mulMatrixOnConstantSEQ(int n, vector<double> a, vector<double> b)
{
	auto start = clock();
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			b[i*n + j] = a[i*n + j] * CONSTANT;
		}
	}
	auto end = clock();
	return end - start;
}



double matrixTransposeAMP(int n, vector<double> a, vector<double> b)
{
	array_view<const double, 2> ob1(n, n, a);
	array_view<double, 2> transpose(n, n, b);
	transpose.discard_data(); 
	auto start = clock();
	parallel_for_each(transpose.extent, [=](index<2>idx)restrict(amp)
	{
		transpose(idx) = ob1(idx[1], idx[0]);
	});
	transpose.synchronize();
	auto end = clock();
	return end - start;
}

double matrixTransposeOMP(int n, vector<double> a, vector<double> b)
{
	auto start = clock();
	#pragma omp parallel for
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			b[i*n + j] = a[j*n + i];
		}
	}
	auto end = clock();
	return end - start;
}

double matrixTransposeSEQ(int n, vector<double> a, vector<double> b)
{
	auto start = clock();
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			b[i*n + j] = a[j*n + i];
		}
	}
	auto end = clock();
	return end - start;
}


double mulMatriciesAMP(int n, vector<double> a, vector<double> b, vector<double> c)
{
	array_view<const double, 2> ob1(n, n, a);
	array_view<const double, 2> ob2(n, n, b);
	array_view<double, 2> mul(n, n, c);
	mul.discard_data();
	auto start = clock();
	parallel_for_each(mul.extent, [=](index<2>idx)restrict(amp)
	{
		int i = idx[0];
		int j = idx[1];
		for(int k = 0; k < n; k++)
		{
			mul(i, j) += ob1(i, k) * ob2(k, j);
		}
	});
	mul.synchronize();
	auto end = clock();
	return end - start;
}

double mulMatriciesOMP(int n, vector<double> a, vector<double> b, vector<double> c)
{
	auto start = clock();
	#pragma omp parallel for
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			for (int k = 0; k < n; ++k)
			{
				c[i*n + j] += a[i*n + k] * b[k*n + j];
			}
		}
	}
	auto end = clock();
	return end - start;
}

double mulMatriciesSEQ(int n, vector<double> a, vector<double> b, vector<double> c)
{
	auto start = clock();
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			for(int k = 0; k < n; ++k)
			{
				c[i*n + j] += a[i*n + k] * b[k*n + j];
			}
		}
	}
	auto end = clock();
	return end - start;
}

vector<double> generateVector(int n, bool leaveEmpty)
{
	vector<double> vector(n);
	if (leaveEmpty) return vector;
	for (int i = 0;i < n;i++)
	{
		int value = rand() % 100000;
		vector[i] = value;
	}
	return vector;
}

