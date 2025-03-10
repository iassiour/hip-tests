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

#include <hip_test_common.hh>

/**
 * @addtogroup hipEventCreateWithFlags hipEventCreateWithFlags
 * @{
 * @ingroup EventTest
 * `hipEventCreateWithFlags(hipEvent_t* event, unsigned flags)` -
 * Create an event with the specified flags to control event behaviour.
 */

/**
 * Test Description
 * ------------------------
 *  - Successfully create an event with all defined device flags.
 * Test source
 * ------------------------
 *  - unit/event/hipEventCreateWithFlags.cc
 * Test requirements
 * ------------------------
 *  - HIP_VERSION >= 5.2
 */
TEST_CASE("Unit_hipEventCreateWithFlags_Positive") {

#if HT_AMD
  const unsigned int flagUnderTest = GENERATE(hipEventDefault, hipEventBlockingSync, hipEventDisableTiming, hipEventInterprocess | hipEventDisableTiming, hipEventReleaseToDevice, hipEventReleaseToSystem);
#else
  // On Non-AMD platforms hipEventReleaseToDevice / hipEventReleaseToSystem are not defined
  const unsigned int flagUnderTest = GENERATE(hipEventDefault, hipEventBlockingSync, hipEventDisableTiming, hipEventInterprocess | hipEventDisableTiming);
#endif

  hipEvent_t event;
  HIP_CHECK(hipEventCreateWithFlags(&event, flagUnderTest));
  REQUIRE(event != nullptr);

  HIP_CHECK(hipEventDestroy(event));
}
