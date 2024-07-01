/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef XCOLLIE_TIMEOUT_TEST_H
#define XCOLLIE_TIMEOUT_TEST_H

#include <gtest/gtest.h>

namespace OHOS {
namespace HiviewDFX {
class XCollieTimeoutTest : public testing::Test {
public:
    XCollieTimeoutTest();
    ~XCollieTimeoutTest();
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    void TimerCallback(void *data);
    int GetCallbackCnt() const
    {
        return callbackCnt_;
    }
    void SetCallbackCnt(int cnt);
private:
    int callbackCnt_;
};
} // end of namespace HiviewDFX
} // end of namespace OHOS
#endif
