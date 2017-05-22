// Lab2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "amp.h"
#include "amp_math.h"
#include <ctime>
#include <iostream>

#define ATTEMPTS 5	
//2 4 8 16 32
#define BLOCK_SIZE 2
using namespace concurrency;
using namespace std;

void blockTranspose();
void mulMatriciesWithoutSharedMemory();
void mulMatriciesWithSharedMemory();
void mulMatriciesWithDecomposition();

vector<double> generateVector(long long n, bool leaveEmpty = false);

double blockTransposeAMP(int n, vector<double> a, vector<double> b, vector<double> c);
double mulMatriciesWithoutSharedMemoryAMP(int n, vector<double> a, vector<double> b, vector<double> c);
double mulMatriciesWithSharedMemoryAMP(int n, vector<double> a, vector<double> b, vector<double> c);
double mulMatriciesWithDecompositionAMP(int n, vector<double> a, vector<double> b, vector<double> c);

int main()
{
	srand(time(nullptr));
	wcout << "WARM UP";
	blockTransposeAMP(32, generateVector(32 * 32), generateVector(32 * 32), generateVector(32 * 32, true));
	blockTranspose();
	mulMatriciesWithoutSharedMemory();
	mulMatriciesWithSharedMemory();
	//mulMatriciesWithDecomposition();
	return 0;
}

void mulMatriciesWithDecomposition()
{
	wcout << "\nMul matricies with decomposition\n";
	long long sizes[] = {
		128,
		256,
		512,
		1024
	};
	for (long long size : sizes)
	{
		wcout << "Size " << size << "x" << size << "\n";
		auto matrixA = generateVector(size * size);
		auto matrixB = generateVector(size * size);
		auto matrixResult = generateVector(size * size, true);
		double elapsedTime = 0;
		for (int i = 0;i < ATTEMPTS;i++)
		{
			//wcout << i;
			elapsedTime += mulMatriciesWithDecompositionAMP(size, matrixA, matrixB, matrixResult);
		}
		wcout << "AMP = " << elapsedTime / ATTEMPTS << " ms\n";
	}
}

void mulMatriciesWithSharedMemory()
{
	wcout << "\nMul matricies with shared memory\n";
	long long sizes[] = {
		128,
		256,
		512,
		1024
	};
	wcout << "Block Size " << BLOCK_SIZE << "\n";
	for (long long size : sizes)
	{
		wcout << "Size " << size << "x" << size << "\n";
		auto matrixA = generateVector(size * size);
		auto matrixB = generateVector(size * size);
		auto matrixResult = generateVector(size * size, true);
		double elapsedTime = 0;
		for (int i = 0;i < ATTEMPTS;i++)
			elapsedTime += mulMatriciesWithSharedMemoryAMP(size, matrixA, matrixB, matrixResult);
		wcout << "AMP = " << elapsedTime / ATTEMPTS << " ms\n";
	}
}

void mulMatriciesWithoutSharedMemory()
{
	wcout << "\nMul matricies without shared memory\n";
	long long sizes[] = {
		128,
		256,
		512,
		1024
	};
	wcout << "Block Size " << BLOCK_SIZE << "\n";
	for (long long size : sizes)
	{
		wcout << "Size " << size << "x" << size << "\n";
		auto matrixA = generateVector(size * size);
		auto matrixB = generateVector(size * size);
		auto matrixResult = generateVector(size * size, true);
		double elapsedTime = 0;
		for (int i = 0;i < ATTEMPTS;i++)
		{
			//wcout << i;
			elapsedTime += mulMatriciesWithoutSharedMemoryAMP(size, matrixA, matrixB, matrixResult);
		}
		wcout << "AMP = " << elapsedTime / ATTEMPTS << " ms\n";
	}
}

void blockTranspose()
{
	wcout << "\nBlock transpose\n";
	long long sizes[] = {
		128,
		256,
		512,
		1024
	};
	wcout << "Block Size " << BLOCK_SIZE << "\n";
	for (long long size : sizes)
	{
		wcout << "Size " << size << "x" << size << "\n";

		auto matrixA = generateVector(size * size);
		auto matrixB = generateVector(size * size);
		auto matrixResult = generateVector(size * size, true);
		double elapsedTime = 0;
		for (int i = 0;i < ATTEMPTS;i++)
		{
			//wcout << i;
			elapsedTime += blockTransposeAMP(size, matrixA, matrixB, matrixResult);
		}
		wcout << "AMP = " << elapsedTime / ATTEMPTS << " ms\n";
	}
}

double mulMatriciesWithDecompositionAMP(int n, vector<double> a, vector<double> b, vector<double> c)
{
	auto start = clock();
	array_view<const double, 2> ob1(n, n, a);
	array_view<const double, 2> ob2(n, n, b);
	array_view<double, 2> res(n, n, c);
	res.discard_data();
	concurrency::extent<1> e(n);
	parallel_for_each(e, [=](index<1> tidx)restrict(amp)
	{
		int row = tidx[0];
		double sum = 0;
		for (int i = 0;i < n;i++)
		{
			for (int k = 0; k < n; k++)
			{
				sum += ob1[row][k] * ob2[k][row + i];
			}
			res[row][row + i] = sum;
		}
	});
	res.synchronize();
	auto end = clock();
	return end - start;
}

double mulMatriciesWithSharedMemoryAMP(int n, vector<double> a, vector<double> b, vector<double> c)
{
	auto start = clock();
	array_view<const double, 2> ob1(n, n, a);
	array_view<const double, 2> ob2(n, n, b);
	array_view<double, 2> res(n, n, c);
	res.discard_data();
	parallel_for_each(ob1.extent.tile<BLOCK_SIZE, BLOCK_SIZE>(), [=](tiled_index<BLOCK_SIZE, BLOCK_SIZE> tidx)restrict(amp)
	{
		tile_static float sA[BLOCK_SIZE][BLOCK_SIZE];
		tile_static float sB[BLOCK_SIZE][BLOCK_SIZE];
		int row = tidx.local[0];
		int col = tidx.local[1];
		float sum = 0.0f;
		for (int i = 0; i < n; i += BLOCK_SIZE) {
			sA[row][col] = ob1(tidx.global[0], col + i);
			sB[row][col] = ob2(row + i, tidx.global[1]);
			tidx.barrier.wait();
			for (int k = 0; k < BLOCK_SIZE; k++) { sum += sA[row][k] * sB[k][col]; }
			tidx.barrier.wait();
		}
		res[tidx.global] = sum;
	});
	res.synchronize();
	auto end = clock();
	return end - start;
}

double mulMatriciesWithoutSharedMemoryAMP(int n, vector<double> a, vector<double> b, vector<double> c)
{
	auto start = clock();
	array_view<const double, 2> ob1(n, n, a);
	array_view<const double, 2> ob2(n, n, b);
	array_view<double, 2> res(n, n, c);
	res.discard_data();
	parallel_for_each(ob1.extent.tile<BLOCK_SIZE, BLOCK_SIZE>(), [=](tiled_index<BLOCK_SIZE, BLOCK_SIZE> tidx)restrict(amp)
	{
		int row = tidx.global[0];
		int col = tidx.global[1];
		double sum = 0;
		for (int i = 0;i < n; i += BLOCK_SIZE)
		{
			for (int k = 0; k < BLOCK_SIZE; k++)
			{
				sum += ob1[row][k] * ob2[k][col];
				res[tidx.global] = sum;
			}
		}
	});
	res.synchronize();
	auto end = clock();
	return end - start;
}

double blockTransposeAMP(int n, vector<double> a, vector<double> b, vector<double> c)
{
	auto start = clock();
	array_view<const double, 2> ob1(n, n, a);
	array_view<double, 2> res(n, n, c);
	res.discard_data();
	parallel_for_each(ob1.extent.tile<BLOCK_SIZE, BLOCK_SIZE>(), [=](tiled_index<BLOCK_SIZE, BLOCK_SIZE> tidx)restrict(amp)
	{
		tile_static double localData[BLOCK_SIZE][BLOCK_SIZE];
		localData[tidx.local[1]][tidx.local[0]] = ob1[tidx.global];
		tidx.barrier.wait();
		index<2> resIdx(index<2>(tidx.tile_origin[1], tidx.tile_origin[0]) + tidx.local);
		res[resIdx] = localData[tidx.local[0]][tidx.local[1]];
	});
	res.synchronize();
	auto end = clock();
	return end - start;
}

vector<double> generateVector(long long n, bool leaveEmpty)
{
	vector<double> vector(n);
	if (leaveEmpty) return vector;
	for (long long i = 0;i < n;i++)
	{
		int value = rand() % 100000;
		vector[i] = value;
	}
	return vector;
}

