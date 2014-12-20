#include "MockErgNet.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"


using ::testing::AtLeast; 
using ::testing::_;
using ::testing::NotNull;
using ::testing::SetArgPointee;
using ::testing::Return;


class ConfigurableErgNet : public ErgNet
{
    int pm3s;
    int pm4s;
    int pm5s;
    UINT16_T number_found_so_far;

    virtual ERRCODE_T callDDIDiscoverPm3s( INT8_T *product_name, UINT16_T starting_address, UINT16_T *num_units)
    {
        if (!strcmp( TKCMDSET_PM3_PRODUCT_NAME2, product_name )) { *num_units=pm3s+number_found_so_far; }
        if (!strcmp( TKCMDSET_PM4_PRODUCT_NAME, product_name)) { *num_units=pm4s+number_found_so_far;; }
        if (!strcmp( TKCMDSET_PM5_PRODUCT_NAME, product_name)) { *num_units=pm5s+number_found_so_far; }
        if (*num_units==0) { return -10201; } // error for no units
        number_found_so_far = *num_units;
        return 0;
    }
    
    public:
        ConfigurableErgNet( int num_pm3s, int num_pm4s, int num_pm5s)
        { pm3s=num_pm3s; pm4s=num_pm4s; pm5s=num_pm5s; number_found_so_far=0; }

        virtual int discoverErgsOfType( ErgType ergType ) { return ErgNet::discoverErgsOfType(ergType); }

};


TEST( DiscoverErgs, NoErgs )
{
    MockErgNet ergnet;
    UINT16_T num_units;
    ON_CALL( ergnet, callDDIDiscoverPm3s(NotNull(), _,  &num_units)).WillByDefault(DoAll(SetArgPointee<2>(0), Return(0)));
    int numErgs = ergnet.discoverErgs(); 
    ASSERT_EQ(0, numErgs);
}

TEST( DiscoverErgs, NoErgsViaConfigurableErgNet )
{
    ConfigurableErgNet oen(0,0,0);
    ASSERT_EQ(0, oen.discoverErgs());
}


TEST( DiscoverErgs, OnePm3 )
{
    ConfigurableErgNet oen(1,0,0);
    ASSERT_EQ(1, oen.discoverErgs());
}

TEST( DiscoverErgs, OnePm4 )
{
    ConfigurableErgNet oen(0,1,0);
    ASSERT_EQ(1, oen.discoverErgs());
}

TEST( DiscoverErgs, OnePm5 )
{
    ConfigurableErgNet oen(0,0,1);
    ASSERT_EQ(1, oen.discoverErgs());
}

TEST( DiscoverErgs, OneOfEach )
{
    ConfigurableErgNet oen(1,1,1);
    ASSERT_EQ(3, oen.discoverErgs());
}


TEST( DiscoverErgs, TwoPm3s )
{
    ConfigurableErgNet oen(2,0,0);
    ASSERT_EQ(2, oen.discoverErgs());
}

TEST( DiscoverErgs, TwoPm4s )
{
    ConfigurableErgNet oen(0,2,0);
    ASSERT_EQ(2, oen.discoverErgs());
}

TEST( DiscoverErgs, TwoPm5s )
{
    ConfigurableErgNet oen(0,0,2);
    ASSERT_EQ(2, oen.discoverErgs());
}

TEST( DiscoverErgs, OnePm3OnePm5 )
{
    ConfigurableErgNet oen(1,0,1);
    ASSERT_EQ(2, oen.discoverErgs());
}




int main(int argc, char** argv) 
{
    //ErgNet ergNet;
    //ergNet.discoverErgs();

  // The following line must be executed to initialize Google Mock
  // (and Google Test) before running the tests.
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}

/*
    This was how the mocks are used:

    using ::testing::_;
    using ::testing::NotNull;

    MockErgNet ergnet;
    UINT16_T num_units;
    ON_CALL( ergnet, callDDIDiscoverPm3s(TKCMDSET_PM3_PRODUCT_NAME2, _,  &num_units)).WillByDefault(DoAll(SetArgPointee<2>(1), Return(0)));
    ON_CALL( ergnet, callDDIDiscoverPm3s(TKCMDSET_PM4_PRODUCT_NAME, _,  &num_units)).WillByDefault(DoAll(SetArgPointee<2>(0), Return(-10201))); // -10201 means no ergs found
    ON_CALL( ergnet, callDDIDiscoverPm3s(TKCMDSET_PM5_PRODUCT_NAME, _,  &num_units)).WillByDefault(DoAll(SetArgPointee<2>(0), Return(-10201)));

    ergnet.callDDIDiscoverPm3s( TKCMDSET_PM3_PRODUCT_NAME2, 0, &num_units); ASSERT_EQ(1, num_units);
    ergnet.callDDIDiscoverPm3s( TKCMDSET_PM4_PRODUCT_NAME, 0, &num_units); ASSERT_EQ(0, num_units);
    ergnet.callDDIDiscoverPm3s( TKCMDSET_PM5_PRODUCT_NAME, 0, &num_units); ASSERT_EQ(0, num_units);
*/
 
