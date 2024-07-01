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

/* This files contains process dump elf module. */

#include "dfx_elf.h"

#include <cerrno>
#include <fcntl.h>
#include <cinttypes>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "dfx_define.h"
#include "dfx_log.h"

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

namespace OHOS {
namespace HiviewDFX {
static const int MAX_MAP_SIZE = 65536;

std::shared_ptr<DfxElf> DfxElf::Create(const std::string path)
{
    DfxLogDebug("Enter %s.", __func__);
    char realPaths[PATH_MAX] = {0};
    if (!realpath(path.c_str(), realPaths)) {
        DfxLogWarn("Fail to do realpath(%s).", path.c_str());
        return nullptr;
    }

    auto dfxElf = std::make_shared<DfxElf>();

    dfxElf->SetFd(open(realPaths, O_RDONLY | O_CLOEXEC));

    if (dfxElf->fd_ < 0) {
        DfxLogWarn("Fail to open elf file(%s).", realPaths);
        return nullptr;
    }

    struct stat elfStat;
    if (fstat(dfxElf->fd_, &elfStat) != 0) {
        DfxLogWarn("Fail to get elf size.");
        close(dfxElf->fd_);
        dfxElf->SetFd(-1);
        return nullptr;
    }

    dfxElf->SetSize((uint64_t)elfStat.st_size);
    if (!dfxElf->ParseElfHeader()) {
        DfxLogWarn("Fail to parse elf header.");
        close(dfxElf->fd_);
        dfxElf->SetFd(-1);
        return nullptr;
    }

    if (!dfxElf->ParseElfProgramHeader()) {
        DfxLogWarn("Fail to parse elf program header.");
        close(dfxElf->GetFd());
        dfxElf->SetFd(-1);
        return nullptr;
    }

    close(dfxElf->GetFd());
    dfxElf->SetFd(-1);
    DfxLogDebug("Exit %s.", __func__);
    return dfxElf;
}

bool DfxElf::ParseElfHeader()
{
    DfxLogDebug("Enter %s.", __func__);
    ssize_t nread = read(fd_, &(header_), sizeof(header_));
    if (nread < 0 || nread != static_cast<long>(sizeof(header_))) {
        DfxLogWarn("Failed to read elf header.");
        return false;
    }
    DfxLogDebug("Exit %s.", __func__);
    return true;
}

bool DfxElf::ParseElfProgramHeader()
{
    DfxLogDebug("Enter %s.", __func__);
    size_t size = header_.e_phnum * sizeof(ElfW(Phdr));
    if (size > MAX_MAP_SIZE) {
        DfxLogWarn("Exceed max mmap size.");
        return false;
    }

    size_t offset = header_.e_phoff;
    size_t startOffset = offset & static_cast<size_t>(getpagesize() - 1);
    size_t alignedOffset = offset & (~(static_cast<size_t>(getpagesize() - 1)));
    uint64_t endOffset;
    if (__builtin_add_overflow(static_cast<uint64_t>(size), static_cast<uint64_t>(offset), &endOffset) ||
        __builtin_add_overflow(static_cast<uint64_t>(endOffset), static_cast<uint64_t>(startOffset), &endOffset)) {
        DfxLogWarn("Offset calculate error.");
        return false;
    }

    size_t mapSize = static_cast<size_t>(endOffset - offset);
    if (mapSize > MAX_MAP_SIZE) {
        DfxLogWarn("Exceed max mmap size.");
        return false;
    }

    void *map = mmap(nullptr, mapSize, PROT_READ, MAP_PRIVATE, fd_, static_cast<off_t>(alignedOffset));
    if (map == static_cast<void *>MAP_FAILED) {
        DfxLogWarn("Failed to mmap elf.");
        return false;
    }

    ElfW(Phdr) *phdrTable = (ElfW(Phdr) *)((uint8_t *)map + startOffset);
    for (size_t i = 0; i < header_.e_phnum; i++) {
        ElfW(Phdr) * phdr = &(phdrTable[i]);
        if (phdr->p_type != PT_LOAD) {
            continue;
        }
        CreateLoadInfo(phdr->p_vaddr, phdr->p_offset);
    }
    munmap(map, mapSize);
    DfxLogDebug("Exit %s.", __func__);
    return true;
}

uint64_t DfxElf::FindRealLoadOffset(uint64_t offset) const
{
    DfxLogDebug("Enter %s.", __func__);
    for (auto iter = infos_.begin(); iter != infos_.end(); iter++) {
        if ((iter->offset & -PAGE_SIZE) == offset) {
            return offset + (iter->vaddr - iter->offset);
        }
    }
    DfxLogDebug("Exit %s.", __func__);
    return offset;
}

void DfxElf::CreateLoadInfo(uint64_t vaddr, uint64_t offset)
{
    DfxLogDebug("Enter %s.", __func__);
    std::unique_ptr<ElfLoadInfo> info(new ElfLoadInfo());
    if (info == nullptr) {
        return;
    }
    info->vaddr = vaddr;
    info->offset = offset;

    infos_.push_back(*info);

    DfxLogDebug("Exit %s.", __func__);
}

std::string DfxElf::GetName() const
{
    return name_;
}

void DfxElf::SetName(const std::string &name)
{
    name_ = name;
}

std::string DfxElf::GetPath() const
{
    return path_;
}

void DfxElf::SetPath(const std::string &path)
{
    path_ = path;
}

ElfW(Ehdr) DfxElf::GetHeader() const
{
    return header_;
}

void DfxElf::SetHeader(ElfW(Ehdr) header)
{
    header_ = header;
}

std::vector<ElfLoadInfo> DfxElf::GetInfos() const
{
    return infos_;
}
void DfxElf::SetInfos(const std::vector<ElfLoadInfo> &infos)
{
    infos_ = infos;
}

int32_t DfxElf::GetFd() const
{
    return fd_;
}

void DfxElf::SetFd(int32_t fdValue)
{
    fd_ = fdValue;
}

size_t DfxElf::GetLoadBias() const
{
    return loadBias_;
}

void DfxElf::SetLoadBias(size_t loadBias)
{
    loadBias_ = loadBias;
}

uint64_t DfxElf::GetSize() const
{
    return size_;
}

void DfxElf::SetSize(uint64_t size)
{
    size_ = size;
}
} // namespace HiviewDFX
} // namespace OHOS
