// Lab4.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "amp.h"
#include "amp_math.h"
#include <ctime>
#include <iostream>
using namespace concurrency;
using namespace std;
#define ATTEMPTS 5

void getHistogram();
vector<int> generateInputData(long long n);
double getHistogramSEQ(vector<int> input);
double getHistogramAMP(vector<int> input);

int main()
{
	srand(time(nullptr));
	getHistogram();
    return 0;
}

void getHistogram()
{
	wcout << "\nHistogram\n";
	long long nSizes[] = {
		1000000,
		5000000,
		10000000 };
	for (long long n : nSizes)
	{
		wcout << "N = " << n << "\n";
		auto inputData = generateInputData(n);

		double elapsedTime = 0;

		for (int i = 0; i < ATTEMPTS; i++)
			elapsedTime += getHistogramSEQ(inputData);
		wcout << "SEQ = " << elapsedTime / ATTEMPTS << " ms\n";
		elapsedTime = 0;

		for (int i = 0; i < ATTEMPTS; i++)
			elapsedTime += getHistogramAMP(inputData);
		wcout << "AMP = " << elapsedTime / ATTEMPTS << " ms\n";
	}
}

double getHistogramSEQ(vector<int> input)
{
	
	int histogram[255];
	for (int i = 0; i < size(histogram); i++)
		histogram[i] = 0;
	auto start = clock();
	for (int i = 0; i < size(input); i++)
		histogram[input[i]]++;
	auto end = clock();
	return end - start;
}

double getHistogramAMP(vector<int> input)
{
	
	/*int histogram[255];
	for (int i = 0; i < size(histogram); i++)
		histogram[i] = 0;*/
	auto start = clock();
	array_view<int, 1> histogram(input.size());

	auto end = clock();
	return end - start;
}

vector<int> generateInputData(long long n)
{
	vector<int> vector(n);
	for (long long i = 0; i < n; i++)
	{
		int value = rand() % 255;
		vector[i] = value;
	}
	return vector;
}