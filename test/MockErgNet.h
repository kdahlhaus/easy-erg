#include "gmock/gmock.h"  // Brings in Google Mock.
#ifndef MOCKERGNET_H
#define MOCKERGNET_H

#include "ErgNet.h"
#include "gmock/gmock.h"


class MockErgNet : public ErgNet {
 public:
  MOCK_METHOD3(callDDIDiscoverPm3s,
      ERRCODE_T(INT8_T *product_name, UINT16_T starting_address, UINT16_T *num_units));
};


#endif
