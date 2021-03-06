#define PICOBENCH_IMPLEMENT_WITH_MAIN
#include "picobench/picobench.hpp"

#include <array>
#include <vector>
using namespace std;

#define COLUMNS 18 * 30
#define ROWS 16 * 30
#define LENGTH ROWS * COLUMNS

static void c_array_singledim(picobench::state& s)
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
PICOBENCH(c_array_singledim); 

static void c_array_rowmajor(picobench::state& s)
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
PICOBENCH(c_array_rowmajor); 

static void c_array_colmajor(picobench::state& s)
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
        for (auto x = 0; x < COLUMNS; x++)
        {
            for (auto y = 0; y < ROWS; y++)
            {
                sum += values[y][x];
            }
        }
    }

    volatile long barrier = sum;
}
PICOBENCH(c_array_colmajor); 

static void std_array(picobench::state& s)
{
    // init
    array<array<long, COLUMNS>, ROWS> values;
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
PICOBENCH(std_array); 

static void std_vector(picobench::state& s)
{
    // init
    vector<vector<long>> values;
    for (auto y = 0; y < ROWS; y++)
    {
        vector<long> row;
        for (auto x = 0; x < COLUMNS; x++)
        {
            row.push_back(y * COLUMNS + x);
        }
        values.push_back(row);
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
PICOBENCH(std_vector); 