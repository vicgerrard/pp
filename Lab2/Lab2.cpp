// Lab2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "amp.h"
#include "amp_math.h"
#include <ctime>
#include <iostream>
using namespace concurrency;
using namespace std;

void blockTranspose();

int main()
{
	blockTranspose();
    return 0;
}

void blockTranspose()
{
	wcout << "\nBlock transpose\n";
	int sizes[] ={
		128,
		256,
		512
	};
	int blockSizes[] = {
		2,
		4,
		8,
		16,
		32,
		128
	};

	for(int size : blockSizes)
	{
		wcout << "Size " << size << "x" << size << "\n";
	}
}

