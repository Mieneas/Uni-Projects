#include "TestHelper.h"

using namespace std;

void print(string s)
{
    cout << "[" << sc_time_stamp() << " / " << sc_delta_count()
         << "](TestBench): " << s << endl;
}
