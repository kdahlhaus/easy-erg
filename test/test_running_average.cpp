#include <iostream>
#include <assert.h>

#include "gtest/gtest.h"

/* this is a hack to avoid an import error regarding _ctype at runtime */
short ** _imp___ctype = 0; 

#include "Pm3Erg.h"

using namespace std;

TEST( RunningAverage, HearRate )
{
    RunningAverage<unsigned short, unsigned long> hr;
    hr.add(100);
    hr.add(110);
    cout << "average hr: " << hr.aAverage() << "\n";
    ASSERT_EQ( 105, hr.aAverage() );

    hr.add(120);
    hr.add(130);
    hr.add(140);
    cout << "average hr: " << hr.aAverage() << "\n";
    ASSERT_EQ( 120, hr.aAverage() );

    hr.reset();
    cout << "average hr: " << hr.aAverage() << "\n";
    ASSERT_EQ( 0, hr.aAverage() );
    assert( hr.aAverage()==0 ); 
    hr.add(100);
    hr.add(110);
    cout << "average hr: " << hr.aAverage() << "\n";
    ASSERT_EQ( 105, hr.aAverage() );

}

TEST( RunningAverage, Watts)
{
    RunningAverage<float, float> watts;
    watts.add(200);
    watts.add(201);
    cout << "average w: " << watts.aAverage() << "\n";
    ASSERT_EQ( 200.5, watts.aAverage() );
}

int main(int argc, char **argv) 
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
