/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* This files contains process dump write log to file module. */

#ifndef CPP_CRASH_REPORTER_H
#define CPP_CRASH_REPORTER_H

#include <cinttypes>
#include <map>
#include <memory>
#include <string>

#include "dfx_process.h"

namespace OHOS {
namespace HiviewDFX {
class CppCrashReporter {
public:
    CppCrashReporter(uint64_t time, int32_t signo, std::shared_ptr<DfxProcess> process) \
        : time_(time), signo_(signo), process_(process)
    {
        pid_ = 0;
        uid_ = 0;
        cmdline_ = "";
        reason_ = "";
        stack_ = "";
    };
    virtual ~CppCrashReporter() {};

    void SetCrashReason(const std::string& reason)
    {
        reason_ = reason;
    };

    void AppendCrashStack(const std::string& frame) {
        stack_.append(frame).append("\n");
    };

    void SetValue(const std::string& key, const std::string& value)
    {
        if (key.empty() || value.empty()) {
            return;
        }

        kvPairs_[key] = value;
    };
    void ReportToHiview();

private:
    bool Format();

private:
    uint64_t time_;
    int32_t signo_;
    int32_t pid_;
    int32_t uid_;
    std::string cmdline_;
    std::string reason_;
    std::string stack_;
    std::map<std::string, std::string> kvPairs_;
    std::shared_ptr<DfxProcess> process_;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif 
