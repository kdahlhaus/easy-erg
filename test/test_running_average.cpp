#include <iostream>
#include <assert.h>

/* this is a hack to avoid an import error regarding _ctype at runtime */
short ** _imp___ctype = 0; 

#include "Pm3Erg.h"

using namespace std;

int main()
{
    RunningAverage<unsigned short, unsigned long> hr;
    hr.add(100);
    hr.add(110);
    cout << "average hr: " << hr.aAverage() << "\n";
    assert( hr.aAverage()==105 );
    hr.add(120);
    hr.add(130);
    hr.add(140);
    cout << "average hr: " << hr.aAverage() << "\n";
    assert( hr.aAverage()==120 );
    hr.reset();
    cout << "average hr: " << hr.aAverage() << "\n";
    assert( hr.aAverage()==0 );

    RunningAverage<float, float> watts;
    watts.add(200);
    watts.add(201);
    cout << "average w: " << watts.aAverage() << "\n";
    assert(watts.aAverage()==200.5);
}
