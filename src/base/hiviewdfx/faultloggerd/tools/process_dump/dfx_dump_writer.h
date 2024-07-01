/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef DFX_DUMP_WIRTTER_H
#define DFX_DUMP_WIRTTER_H

#include <cinttypes>
#include <memory>
#include "dfx_define.h"
#include "dfx_process.h"

namespace OHOS {
namespace HiviewDFX {
enum ProcessDumpType : int32_t {
    DUMP_TYPE_PROCESS,
    DUMP_TYPE_THREAD,
};

class ProcessDumpRequest {
public:
    ProcessDumpRequest();
    ~ProcessDumpRequest() = default;

    ProcessDumpType GetType() const;

    void SetType(ProcessDumpType type);

    int32_t GetTid() const;

    void SetTid(int32_t tid);

    int32_t GetPid() const;

    void SetPid(int32_t pid);

    int32_t GetUid() const;

    void SetUid(int32_t uid);

    uint64_t GetReserved() const;

    void SetReserved(uint64_t reserved);

    uint64_t GetTimeStamp() const;

    void SetTimeStamp(uint64_t timeStamp);

    siginfo_t GetSiginfo() const;

    void SetSiginfo(siginfo_t &siginfo);

    ucontext_t GetContext() const;

    void SetContext(ucontext_t const &context);

    std::string GetThreadNameString() const;

    std::string GetProcessNameString() const;
private:
    ProcessDumpType type_;
    int32_t tid_ = 0;
    int32_t pid_ = 0;
    int32_t uid_ = 0;
    uint64_t reserved_ = 0;
    uint64_t timeStamp_ = 0;
    siginfo_t siginfo_;
    ucontext_t context_;
    char threadName_[NAME_LEN];
    char processName_[NAME_LEN];
};

class DfxDumpWriter {
public:
    DfxDumpWriter() = default;
    virtual ~DfxDumpWriter() = default;
    DfxDumpWriter(std::shared_ptr<DfxProcess> process, int32_t fromSignalHandler);
private:
    std::shared_ptr<DfxProcess> process_ = nullptr;
    int32_t fromSignalHandler_ = 0;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif
