#define PICOBENCH_IMPLEMENT_WITH_MAIN
#include "picobench/picobench.hpp"

#include <array>
#include <vector>
using namespace std;

#define COLUMNS 18 * 30
#define ROWS 16 * 30
#define LENGTH ROWS * COLUMNS

static void c_array_malloc(picobench::state& s)
{
    auto use_c_array = [](long* xs) -> void
    {
        volatile long x = xs[30*COLUMNS + 30];
    }; 
    
    auto xs = (long*)malloc(sizeof(long) * LENGTH); // can be passed or returned, must be freed manually
    xs[30*COLUMNS + 30] = 0;

    for (auto _ : s)
    {
        use_c_array(xs);
    }

    free(xs);
}
PICOBENCH(c_array_malloc); 



static void c_array_stackalloc(picobench::state& s)
{
    auto use_c_array = [](long* xs) -> void
    {
        // cannot assign or cast to a C array! we have to keep using it as a pointer from now on
        volatile long x = xs[30*18 + 30];
    }; 
    
    long xs[LENGTH]; // can be passed while it is in scope, but not returned - the memory will be freed by returning from this stack frame
    xs[30*18 + 30] = 0;

    for (auto _ : s)
    {
        use_c_array(xs);
    }
}
PICOBENCH(c_array_stackalloc); 



static void c_array_multidim(picobench::state& s)
{
    auto use_c_array = [](long* xs) -> void
    {
        volatile long x = xs[30*18 + 30];
    }; 
    
    long xs[ROWS][COLUMNS]; // scoped as above, but cannot be passed directly, as [][] does not auto-decay 
    xs[30][30] = 0; // multidimensional c-array supports this syntax

    for (auto _ : s)
    {
        use_c_array((long*)xs);
    }
}
PICOBENCH(c_array_multidim); 



static void std_array_byval(picobench::state& s)
{
    auto use_std_array = [](array<array<long,COLUMNS>,ROWS> xs) -> void
    {
        volatile long x = xs[30][30]; 
    }; 
    
    array<array<long,COLUMNS>,ROWS> xs; // allocated on stack, can be passed but not returned

    xs[30][30] = 0; // nested std::array supports this syntax

    for (auto _ : s)
    {
        use_std_array(xs); // calls the copy constructor, fast due to http://en.cppreference.com/w/cpp/language/copy_elision
    }
}
PICOBENCH(std_array_byval); 



static void std_array_byptr(picobench::state& s)
{
    auto use_std_array = [](array<array<long,COLUMNS>,ROWS>* xs) -> void
    {
        volatile long x = (*xs)[30][30]; // dereference, then index
    }; 
    
    auto xs = new array<array<long,COLUMNS>,ROWS>(); // allocated on heap, could be returned and is cheap to pass
    
    (*xs)[30][30] = 0; // inconvenient to access, though

    for (auto _ : s)
    {
        use_std_array(xs); // passes pointer, copy elision not needed
    }

    delete(xs); // must delete objects created with new  (malloc:free :: new:delete)
}
PICOBENCH(std_array_byptr); 



static void std_vector_byref(picobench::state& s)
{
    auto use_std_vector = [](vector<vector<long>>& xs) -> void
    {
        volatile long x = xs[30][30]; // unlike all the others this is actually two separate dereferences. the first returns the inner vector<long>
    }; 
    
    vector<vector<long>> xs; // dynamically sized, no static dimensions required

    for (auto y = 0; y < ROWS; y++)
    {
        vector<long> row; // because it's dynamic, we have to create each row by hand!
        for (auto x = 0; x < COLUMNS; x++)
        {
            row.emplace_back<long>(0); // and each column! but at least we're initialising them to zero, which the others don't guarantee
        }
        xs.push_back(row);
    }

    for (auto _ : s)
    {
        use_std_vector(xs); // passed by reference, so it is fast and does not need copy elision 
    }
}
PICOBENCH(std_vector_byref); 



static void std_vector_byval_copy(picobench::state& s)
{
    auto use_std_vector = [](vector<vector<long>> xs) -> void
    {
        volatile long x = xs[30][30]; // because of the inner deref of a value copy, we are not able to use copy elision
    }; 
    
    auto xs = vector<vector<long>>(); // alternative form of declaration, equivalent to the last one

    xs.reserve(sizeof(vector<long>) * ROWS);
    for (auto y = 0; y < ROWS; y++)
    {
        vector<long> row; 
        row.reserve(sizeof(long) * COLUMNS);
        for (auto x = 0; x < COLUMNS; x++)
        {
            row.emplace_back<long>(0); 
        }
        xs.push_back(row);
    }

    for (auto _ : s)
    {
        use_std_vector(xs); // vector copy constructor cannot be elided, so this copies the entire vector - slow
    }
}
PICOBENCH(std_vector_byval_copy); 

static void std_vector_byval_move(picobench::state& s)
{
    auto use_std_vector = [](vector<vector<long>>&& xs) -> auto
    {
        volatile long x = xs[30][30]; 
        return move(xs); //return the vector rvalue ref to transfer ownership back to the calling function
    }; 
    
    auto xs = vector<vector<long>>(); // alternative form of declaration, equivalent to the last one

    xs.reserve(sizeof(vector<long>) * ROWS);
    for (auto y = 0; y < ROWS; y++)
    {
        vector<long> row; 
        row.reserve(sizeof(long) * COLUMNS);
        for (auto x = 0; x < COLUMNS; x++)
        {
            row.emplace_back<long>(0); 
        }
        xs.push_back(row);
    }

    for (auto _ : s)
    {
        xs = use_std_vector(move(xs)); // uses the move constructor, which can be elided
    }
}
PICOBENCH(std_vector_byval_move); 