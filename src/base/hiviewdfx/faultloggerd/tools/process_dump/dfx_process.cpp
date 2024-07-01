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

/* This files contains process module. */

#include "dfx_process.h"

#include <dirent.h>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <vector>

#include <sys/types.h>

#include <securec.h>

#include "dfx_define.h"
#include "dfx_log.h"
#include "dfx_maps.h"
#include "dfx_signal.h"
#include "dfx_thread.h"
#include "dfx_util.h"
#include "dfx_config.h"
#include "dfx_define.h"
#include "process_dumper.h"

namespace OHOS {
namespace HiviewDFX {
void DfxProcess::FillProcessName()
{
    DfxLogDebug("Enter %s.", __func__);
    char path[NAME_LEN] = "\0";
    if (snprintf_s(path, sizeof(path), sizeof(path) - 1, "/proc/%d/cmdline", pid_) <= 0) {
        return;
    }

    ReadStringFromFile(path, processName_, NAME_LEN);
    DfxLogDebug("Exit %s.", __func__);
}

void DfxProcess::UpdateProcessName(std::string processName)
{
    processName_ = processName;
}

std::shared_ptr<DfxProcess> DfxProcess::CreateProcessWithKeyThread(pid_t pid, std::shared_ptr<DfxThread> keyThread)
{
    DfxLogDebug("Enter %s.", __func__);
    auto dfxProcess = std::make_shared<DfxProcess>();
    if (dfxProcess != nullptr) {
        dfxProcess->SetPid(pid);
        dfxProcess->FillProcessName();
    }
    if (!dfxProcess->InitProcessMaps()) {
        DfxLogWarn("Fail to init process maps.");
        return nullptr;
    }
    if (!dfxProcess->InitProcessThreads(keyThread)) {
        DfxLogWarn("Fail to init threads.");
        return nullptr;
    }
    DfxLogWarn("Init process dump with pid:%d.", dfxProcess->GetPid());
    DfxLogDebug("Exit %s.", __func__);
    return dfxProcess;
}

bool DfxProcess::InitProcessMaps()
{
    DfxLogDebug("Enter %s.", __func__);
    auto maps = DfxElfMaps::Create(pid_);
    if (!maps) {
        return false;
    }

    SetMaps(maps);
    DfxLogDebug("Exit %s.", __func__);
    return true;
}

bool DfxProcess::InitProcessThreads(std::shared_ptr<DfxThread> keyThread)
{
    DfxLogDebug("Enter %s.", __func__);
    if (keyThread) {
        threads_.push_back(keyThread);
        return true;
    }

    keyThread = std::make_shared<DfxThread>(GetPid(), GetPid());
    if (!keyThread) {
        return false;
    }

    threads_.push_back(keyThread);
    DfxLogDebug("Exit %s.", __func__);
    return true;
}

bool DfxProcess::InitOtherThreads()
{
    DfxLogDebug("Enter %s.", __func__);
    char path[NAME_LEN] = {0};
    if (snprintf_s(path, sizeof(path), sizeof(path) - 1, "/proc/%d/task", GetPid()) <= 0) {
        return false;
    }

    char realPath[PATH_MAX];
    if (!realpath(path, realPath)) {
        return false;
    }

    DIR *dir = opendir(realPath);
    if (!dir) {
        return false;
    }

    struct dirent *ent;
    while ((ent = readdir(dir))) {
        if (strcmp(ent->d_name, ".") == 0) {
            continue;
        }

        if (strcmp(ent->d_name, "..") == 0) {
            continue;
        }

        pid_t tid = atoi(ent->d_name);
        if (tid == 0) {
            continue;
        }

        InsertThreadNode(tid);
    }
    closedir(dir);
    DfxLogDebug("Exit %s.", __func__);
    return true;
}

void DfxProcess::InsertThreadNode(pid_t tid)
{
    DfxLogDebug("Enter %s.", __func__);
    for (auto iter = threads_.begin(); iter != threads_.end(); iter++) {
        if ((*iter)->GetThreadId() == tid) {
            return;
        }
    }

    auto thread = std::make_shared<DfxThread>(GetPid(), tid);
    threads_.push_back(thread);
    DfxLogDebug("Exit %s.", __func__);
}

void DfxProcess::SetIsSignalHdlr(bool isSignalHdlr)
{
    isSignalHdlr_ = isSignalHdlr;
}

bool DfxProcess::GetIsSignalHdlr() const
{
    return isSignalHdlr_;
}

void DfxProcess::SetIsSignalDump(bool isSignalDump)
{
    isSignalDump_ = isSignalDump;
}

bool DfxProcess::GetIsSignalDump() const
{
    return isSignalDump_;
}

pid_t DfxProcess::GetPid() const
{
    return pid_;
}

pid_t DfxProcess::GetUid() const
{
    return uid_;
}

std::string DfxProcess::GetProcessName() const
{
    return processName_;
}

std::shared_ptr<DfxElfMaps> DfxProcess::GetMaps() const
{
    return maps_;
}

std::vector<std::shared_ptr<DfxThread>> DfxProcess::GetThreads() const
{
    return threads_;
}

void DfxProcess::SetPid(pid_t pid)
{
    pid_ = pid;
}

void DfxProcess::SetUid(pid_t uid)
{
    uid_ = uid;
}

void DfxProcess::SetProcessName(const std::string &processName)
{
    processName_ = processName;
}

void DfxProcess::SetMaps(std::shared_ptr<DfxElfMaps> maps)
{
    maps_ = maps;
}

void DfxProcess::SetThreads(const std::vector<std::shared_ptr<DfxThread>> &threads)
{
    threads_ = threads;
}

void DfxProcess::Detach()
{
    if (threads_.empty()) {
        return;
    }

    for (auto iter = threads_.begin(); iter != threads_.end(); iter++) {
        (*iter)->Detach();
    }
}

void DfxProcess::PrintProcessMapsByConfig()
{
    if (DfxConfig::GetInstance().GetDisplayMaps()) {
        if (GetMaps()) {
            OHOS::HiviewDFX::ProcessDumper::GetInstance().PrintDumpProcessMsg("\nMaps:\n");
        }
        auto mapsVector = maps_->GetValues();
        for (auto iter = mapsVector.begin(); iter != mapsVector.end(); iter++) {
            OHOS::HiviewDFX::ProcessDumper::GetInstance().PrintDumpProcessMsg((*iter)->PrintMap());
        }
    } else {
        DfxLogDebug("hidden Maps");
    }
}

void DfxProcess::PrintThreadsHeaderByConfig()
{
    if (DfxConfig::GetInstance().GetDisplayBacktrace()) {
        if (!isSignalDump_) {
            OHOS::HiviewDFX::ProcessDumper::GetInstance().PrintDumpProcessMsg("Other thread info:\n");
        }
    } else {
        DfxLogDebug("hidden thread info.");
    }
}
} // namespace HiviewDFX
} // namespace OHOS
