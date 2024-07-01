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
#ifndef DFX_UNWIND_H
#define DFX_UNWIND_H

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextern-c-compat"
#endif

#include <memory>

#include <libunwind-ptrace.h>
#include <libunwind.h>
#include "dfx_define.h"
#include "dfx_process.h"
#include "dfx_symbols_cache.h"
#include "dfx_thread.h"
#include "nocopyable.h"

namespace OHOS {
namespace HiviewDFX {
class DfxUnwindRemote final {
public:
    static DfxUnwindRemote &GetInstance();

    bool UnwindProcess(std::shared_ptr<DfxProcess> process);
    bool UnwindThread(std::shared_ptr<DfxProcess> process, std::shared_ptr<DfxThread> thread);

    ~DfxUnwindRemote() = default;

private:
    bool DfxUnwindRemoteDoUnwindStep(size_t const & index,
        std::shared_ptr<DfxThread> & thread, unw_cursor_t & cursor, std::shared_ptr<DfxProcess> process);
    uint64_t DfxUnwindRemoteDoAdjustPc(unw_cursor_t & cursor, uint64_t pc);

private:
    DfxUnwindRemote();
    DISALLOW_COPY_AND_MOVE(DfxUnwindRemote);
    unw_addr_space_t as_;
    std::unique_ptr<DfxSymbolsCache> cache_;
};
}   // namespace HiviewDFX
}   // namespace OHOS

#endif  // DFX_UNWIND_H
