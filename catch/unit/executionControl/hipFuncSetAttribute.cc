/*
Copyright (c) 2022 Advanced Micro Devices, Inc. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "execution_control_common.hh"

#include <hip_test_common.hh>
#include <hip/hip_runtime_api.h>

/**
 * @addtogroup hipFuncSetAttribute hipFuncSetAttribute
 * @{
 * @ingroup ExecutionTest
 * `hipFuncSetAttribute(const void* func, hipFuncAttribute attr, int value)` -
 * Set attribute for a specific function.
 */

/**
 * Test Description
 * ------------------------
 *  - Sets maximum dynamic shared memory size to the non-default value.
 *    - Expected output: return `hipSuccess`
 * Test source
 * ------------------------
 *  - unit/executionControl/hipFuncSetAttribute.cc
 * Test requirements
 * ------------------------
 *  - HIP_VERSION >= 5.2
 */
TEST_CASE("Unit_hipFuncSetAttribute_Positive_MaxDynamicSharedMemorySize") {
  HIP_CHECK(hipFuncSetAttribute(reinterpret_cast<void*>(kernel),
                                hipFuncAttributeMaxDynamicSharedMemorySize, 1024));

  hipFuncAttributes attributes;
  HIP_CHECK(hipFuncGetAttributes(&attributes, reinterpret_cast<void*>(kernel)));

  REQUIRE(attributes.maxDynamicSharedSizeBytes == 1024);
}

/**
 * Test Description
 * ------------------------
 *  - Sets preferred shared memory carveout to the non-default value.
 *    - Expected output: return `hipSuccess`
 * Test source
 * ------------------------
 *  - unit/executionControl/hipFuncSetAttribute.cc
 * Test requirements
 * ------------------------
 *  - HIP_VERSION >= 5.2
 */
TEST_CASE("Unit_hipFuncSetAttribute_Positive_PreferredSharedMemoryCarveout") {
  HIP_CHECK(hipFuncSetAttribute(reinterpret_cast<void*>(kernel),
                                hipFuncAttributePreferredSharedMemoryCarveout, 50));

  hipFuncAttributes attributes;
  HIP_CHECK(hipFuncGetAttributes(&attributes, reinterpret_cast<void*>(kernel)));

  REQUIRE(attributes.preferredShmemCarveout == 50);
}

/**
 * Test Description
 * ------------------------
 *  - Validates handling of valid arguments:
 *    -# When `hipFuncAttributeMaxDynamicSharedMemorySize == 0`
 *      - Expected output: return `hipSuccess`
 *    -# When `hipFuncAttributeMaxDynamicSharedMemorySize == maxSharedMemoryPerBlock - sharedSizeBytes`
 *      - Expected output: return `hipSuccess`
 *    -# When `hipFuncAttributePreferredSharedMemoryCarveout` is 0%
 *      - Expected output: return `hipSuccess`
 *    -# When `hipFuncAttributePreferredSharedMemoryCarveout` is 100%
 *      - Expected output: return `hipSuccess`
 *    -# When `hipFuncAttributePreferredSharedMemoryCarveout` is default (-1)
 *      - Expected output: return `hipSuccess`
 * Test source
 * ------------------------
 *  - unit/executionControl/hipFuncSetAttribute.cc
 * Test requirements
 * ------------------------
 *  - HIP_VERSION >= 5.2
 */
TEST_CASE("Unit_hipFuncSetAttribute_Positive_Parameters") {
  SECTION("hipFuncAttributeMaxDynamicSharedMemorySize == 0") {
    HIP_CHECK(hipFuncSetAttribute(reinterpret_cast<void*>(kernel),
                                  hipFuncAttributeMaxDynamicSharedMemorySize, 0));
  }

  SECTION(
      "hipFuncAttributeMaxDynamicSharedMemorySize == maxSharedMemoryPerBlock - sharedSizeBytes") {
    // The sum of this value and the function attribute sharedSizeBytes cannot exceed the device
    // attribute cudaDevAttrMaxSharedMemoryPerBlockOptin
    int max_shared;
    HIP_CHECK(hipDeviceGetAttribute(&max_shared, hipDeviceAttributeMaxSharedMemoryPerBlock, 0));

    hipFuncAttributes attributes;
    HIP_CHECK(hipFuncGetAttributes(&attributes, reinterpret_cast<void*>(kernel)));

    HIP_CHECK(hipFuncSetAttribute(reinterpret_cast<void*>(kernel),
                                  hipFuncAttributeMaxDynamicSharedMemorySize,
                                  max_shared - attributes.sharedSizeBytes));
  }

  SECTION("hipFuncAttributePreferredSharedMemoryCarveout == 0") {
    HIP_CHECK(hipFuncSetAttribute(reinterpret_cast<void*>(kernel),
                                  hipFuncAttributePreferredSharedMemoryCarveout, 0));
  }

  SECTION("hipFuncAttributePreferredSharedMemoryCarveout == 100") {
    HIP_CHECK(hipFuncSetAttribute(reinterpret_cast<void*>(kernel),
                                  hipFuncAttributePreferredSharedMemoryCarveout, 100));
  }

  SECTION("hipFuncAttributePreferredSharedMemoryCarveout == -1 (default)") {
    HIP_CHECK(hipFuncSetAttribute(reinterpret_cast<void*>(kernel),
                                  hipFuncAttributePreferredSharedMemoryCarveout, -1));
  }
}

/**
 * Test Description
 * ------------------------
 *  - Validates handling of invalid arguments:
 *    -# When pointer to the kernel function is `nullptr`
 *      - Expected output: return `hipErrorInvalidDeviceFunction`
 *    -# When the attribute is invalid
 *      - Expected output: return `hipErrorInvalidValue`
 *    -# When `hipFuncAttributeMaxDynamicSharedMemorySize < 0`
 *      - Expected output: return `hipErrorInvalidValue`
 *    -# When `hipFuncAttributeMaxDynamicSharedMemorySize > maxSharedMemoryPerBlock - sharedSizeBytes`
 *      - Expected output: return `hipErrorInvalidValue`
 *    -# When `hipFuncAttributePreferredSharedMemoryCarveout` is negative
 *      - Expected output: return `hipErrorInvalidValue`
 *    -# When `hipFuncAttributePreferredSharedMemoryCarveout` is above 100%
 *      - Expected output: return `hipErrorInvalidValue`
 * Test source
 * ------------------------
 *  - unit/executionControl/hipFuncSetAttribute.cc
 * Test requirements
 * ------------------------
 *  - HIP_VERSION >= 5.2
 */
TEST_CASE("Unit_hipFuncSetAttribute_Negative_Parameters") {
  SECTION("func == nullptr") {
    HIP_CHECK_ERROR(hipFuncSetAttribute(nullptr, hipFuncAttributePreferredSharedMemoryCarveout, 50),
                    hipErrorInvalidDeviceFunction);
  }

  SECTION("invalid attribute") {
    HIP_CHECK_ERROR(
        hipFuncSetAttribute(reinterpret_cast<void*>(kernel), static_cast<hipFuncAttribute>(-1), 50),
        hipErrorInvalidValue);
  }

  SECTION("hipFuncAttributeMaxDynamicSharedMemorySize < 0") {
    HIP_CHECK_ERROR(hipFuncSetAttribute(reinterpret_cast<void*>(kernel),
                                        hipFuncAttributeMaxDynamicSharedMemorySize, -1),
                    hipErrorInvalidValue);
  }

  SECTION(
      "hipFuncAttributeMaxDynamicSharedMemorySize > maxSharedMemoryPerBlock - sharedSizeBytes") {
    // The sum of this value and the function attribute sharedSizeBytes cannot exceed the device
    // attribute cudaDevAttrMaxSharedMemoryPerBlockOptin
    int max_shared;
    HIP_CHECK(hipDeviceGetAttribute(&max_shared, hipDeviceAttributeMaxSharedMemoryPerBlock, 0));

    hipFuncAttributes attributes;
    HIP_CHECK(hipFuncGetAttributes(&attributes, reinterpret_cast<void*>(kernel)));

    HIP_CHECK_ERROR(hipFuncSetAttribute(reinterpret_cast<void*>(kernel),
                                        hipFuncAttributeMaxDynamicSharedMemorySize,
                                        max_shared - attributes.sharedSizeBytes + 1),
                    hipErrorInvalidValue);
  }

  SECTION("hipFuncAttributePreferredSharedMemoryCarveout < -1") {
    HIP_CHECK_ERROR(hipFuncSetAttribute(reinterpret_cast<void*>(kernel),
                                        hipFuncAttributePreferredSharedMemoryCarveout, -2),
                    hipErrorInvalidValue);
  }

  SECTION("hipFuncAttributePreferredSharedMemoryCarveout > 100") {
    HIP_CHECK_ERROR(hipFuncSetAttribute(reinterpret_cast<void*>(kernel),
                                        hipFuncAttributePreferredSharedMemoryCarveout, 101),
                    hipErrorInvalidValue);
  }
}

/**
 * Test Description
 * ------------------------
 *  - Sets `hipFuncAttributeMaxDynamicSharedMemorySize` to the non-supported value
 *    - Expected output: return `hipErrorNotSupported`
 * Test source
 * ------------------------
 *  - unit/executionControl/hipFuncSetAttribute.cc
 * Test requirements
 * ------------------------
 *  - Platform specific (AMD)
 *  - HIP_VERSION >= 5.2
 */
TEST_CASE("Unit_hipFuncSetAttribute_Positive_MaxDynamicSharedMemorySize_Not_Supported") {
  hipFuncAttributes old_attributes;
  HIP_CHECK(hipFuncGetAttributes(&old_attributes, reinterpret_cast<void*>(kernel)));

  HIP_CHECK_ERROR(hipFuncSetAttribute(reinterpret_cast<void*>(kernel),
                                      hipFuncAttributeMaxDynamicSharedMemorySize, 1024),
                  hipErrorNotSupported);

  hipFuncAttributes new_attributes;
  HIP_CHECK(hipFuncGetAttributes(&new_attributes, reinterpret_cast<void*>(kernel)));

  REQUIRE(old_attributes.maxDynamicSharedSizeBytes == new_attributes.maxDynamicSharedSizeBytes);
}

/**
 * Test Description
 * ------------------------
 *  - Sets `hipFuncAttributePreferredSharedMemoryCarveout` to the non-supported value
 *    - Expected output: return `hipErrorNotSupported`
 * Test source
 * ------------------------
 *  - unit/executionControl/hipFuncSetAttribute.cc
 * Test requirements
 * ------------------------
 *  - Platform specific (AMD)
 *  - HIP_VERSION >= 5.2
 */
TEST_CASE("Unit_hipFuncSetAttribute_Positive_PreferredSharedMemoryCarveout_Not_Supported") {
  hipFuncAttributes old_attributes;
  HIP_CHECK(hipFuncGetAttributes(&old_attributes, reinterpret_cast<void*>(kernel)));

  HIP_CHECK_ERROR(hipFuncSetAttribute(reinterpret_cast<void*>(kernel),
                                      hipFuncAttributePreferredSharedMemoryCarveout, 50),
                  hipErrorNotSupported);

  hipFuncAttributes new_attributes;
  HIP_CHECK(hipFuncGetAttributes(&new_attributes, reinterpret_cast<void*>(kernel)));

  REQUIRE(old_attributes.preferredShmemCarveout == new_attributes.preferredShmemCarveout);
}
