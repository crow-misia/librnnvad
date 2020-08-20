/*
 *  Copyright 2006 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_CHECKS_H_
#define RTC_BASE_CHECKS_H_

#include <assert.h>

#define RTC_CHECK(condition) assert(condition)
#define RTC_CHECK_EQ(val1, val2) assert((val1) == (val2))
#define RTC_CHECK_NE(val1, val2) assert((val1) != (val2))
#define RTC_CHECK_LE(val1, val2) assert((val1) <= (val2))
#define RTC_CHECK_LT(val1, val2) assert((val1) < (val2))
#define RTC_CHECK_GE(val1, val2) assert((val1) >= (val2))
#define RTC_CHECK_GT(val1, val2) assert((val1) > (val2))

#define RTC_DCHECK(condition) RTC_CHECK(condition)
#define RTC_DCHECK_EQ(v1, v2) RTC_CHECK_EQ(v1, v2)
#define RTC_DCHECK_NE(v1, v2) RTC_CHECK_NE(v1, v2)
#define RTC_DCHECK_LE(v1, v2) RTC_CHECK_LE(v1, v2)
#define RTC_DCHECK_LT(v1, v2) RTC_CHECK_LT(v1, v2)
#define RTC_DCHECK_GE(v1, v2) RTC_CHECK_GE(v1, v2)
#define RTC_DCHECK_GT(v1, v2) RTC_CHECK_GT(v1, v2)

namespace rtc {

// Performs the integer division a/b and returns the result. CHECKs that the
// remainder is zero.
template <typename T>
inline T CheckedDivExact(T a, T b) {
  RTC_CHECK_EQ(a % b, 0); // "a" is not evenly divisible by "b"
  return a / b;
}

}  // namespace rtc

#endif  // RTC_BASE_CHECKS_H_
