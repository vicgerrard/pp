// Lab3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "amp.h"
#include "amp_math.h"
#include <ctime>
#include <iostream>

using namespace concurrency;
using namespace std;

#define N 1536000
#define PointsN 1

struct Point
{
	double X;
	double Y;
	int Class;
	double Distance[PointsN];
};


void knn();
double knnAMP(vector<Point>& trainPoints, vector<Point>& points, int k);
vector<Point> generateTrainData(int n);
Point generatePoint();
int getClass(double x, double y);

int main()
{
	srand(time(nullptr));
	knn();
	return 0;
}


void knn()
{

	wcout << "kNN" << endl;
	wcout << "WARM UP" << endl;
	auto trainPoints = generateTrainData(N);
	vector<Point> points;
	for (int i = 0; i < PointsN; i++)
		points.push_back(generatePoint());
	double elapsedTime = knnAMP(trainPoints, points, 2);

	int kSizes[] = {
		2,
		3,
		4,
		5
	};

	for (int k : kSizes)
	{
		wcout << "K = " << k << endl;
		auto trainPoints = generateTrainData(N);
		vector<Point> points;
		for (int i = 0; i < PointsN; i++)
			points.push_back(generatePoint());
		double elapsedTime = knnAMP(trainPoints, points, k);
		wcout << "AMP = " << elapsedTime << " ms\n";
	}
}

vector<Point> generateTrainData(int n)
{
	vector<Point> data(n);
	for (int i = 0; i < n; ++i)
	{
		Point point = generatePoint();
		point.Class = getClass(point.X, point.Y);
		data[i] = point;
	}
	return data;
}

int getClass(double x, double y)
{
	if (x >= 0 && y >= 0) return 0;
	if (x >= 0 && y < 0) return 1;
	if (x < 0 && y >= 0) return 2;
	return 3;
}

bool sortPredicate(Point a, Point b)
{
	return a.Distance < b.Distance;
}

double knnAMP(vector<Point>& trainPoints, vector<Point>& points, int k)
{
	auto start = clock();
	array_view<Point, 1> trainPointsView(trainPoints);
	array_view<const Point, 1> pointsView(points);
	parallel_for_each(trainPointsView.extent, [=](index<1> idx) restrict(amp)
                  {
	                  for (int i = 0; i < PointsN; i++)
		                  trainPointsView(idx).Distance[i] = fast_math::sqrt(pointsView[i].X * trainPointsView(idx).X + pointsView[i].Y * trainPointsView(idx).Y);
                  });
	trainPointsView.synchronize();
	sort(trainPoints.begin(), trainPoints.end(), sortPredicate);;
	int classes[4];
	for (int i = 0; i < size(classes); i++)
	{
		classes[i] = 0;
	}
	int pointClass = -1;
	int max = -1;
	for (int i = 0; i < k; i++)
	{
		int curClass = trainPointsView[i].Class;
		classes[curClass]++;
		if (classes[curClass] > max)
		{
			max = classes[curClass];
			pointClass = curClass;
		}
	}
	auto end = clock();
	return end - start;
}

Point generatePoint()
{
	Point point;
	point.X = -1 + static_cast<double>(rand()) / RAND_MAX * (1 - -1);
	point.Y = -1 + static_cast<double>(rand()) / RAND_MAX * (1 - -1);
	point.Class = 0;
	return point;
}
