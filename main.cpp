#define PICOBENCH_IMPLEMENT_WITH_MAIN
#include "picobench/picobench.hpp"

#include <iostream>
using namespace std;

#define COLUMNS 18 * 10
#define ROWS 16 * 10
#define LENGTH ROWS * COLUMNS

static void c_array(picobench::state& s)
{
    // init
    long values[LENGTH];
    for (auto i = 0; i < LENGTH; i++)
    {
        values[i] = i;
    }

    // benchmark
    long sum = 0;
    for (auto _ : s)
    {
        for (auto i = 0; i < LENGTH; i++)
        {
            sum += values[i];
        }
    }

    volatile long barrier = sum;
}
PICOBENCH(c_array); 

static void c_array_multidim(picobench::state& s)
{
    // init
    long values[ROWS][COLUMNS];
    for (auto y = 0; y < ROWS; y++)
    {
        for (auto x = 0; x < COLUMNS; x++)
        {
            values[y][x] = y * COLUMNS + x;
        }
    }

    // benchmark
    long sum = 0;
    for (auto _ : s)
    {
        for (auto y = 0; y < ROWS; y++)
        {
            for (auto x = 0; x < COLUMNS; x++)
            {
                sum += values[y][x];
            }
        }
    }

    volatile long barrier = sum;
}
PICOBENCH(c_array_multidim); 

static void c_array_ignoredim(picobench::state& s)
{
    // init
    long values[ROWS][COLUMNS];
    for (auto y = 0; y < ROWS; y++)
    {
        for (auto x = 0; x < COLUMNS; x++)
        {
            values[y][x] = y * COLUMNS + x;
        }
    }

    // benchmark
    long sum = 0;
    for (auto _ : s)
    {
        for (auto i = 0; i < LENGTH; i++)
        {
            sum += ((long*)values)[i];
        }
    }

    volatile long barrier = sum;
}
PICOBENCH(c_array_ignoredim); 