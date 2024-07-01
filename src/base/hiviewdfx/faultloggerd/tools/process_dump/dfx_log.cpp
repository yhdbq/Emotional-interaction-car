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
#include "dfx_log.h"

#include <cstdarg>
#include <cstdio>
#include <securec.h>
#include <unistd.h>

#include "faultloggerd_client.h"
#include "dfx_define.h"

#ifdef DFX_SIGNALHANDLER_TAG
#ifdef LOG_DOMAIN
#undef LOG_DOMAIN
#define LOG_DOMAIN 0x2D11
#endif

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "DfxSignalHandler"
#endif
#endif

enum class LOG_LEVEL_CLASS {
    LOG_LEVEL_ALL = 1,
    LOG_LEVEL_TRACE,
    LOG_LEVEL_DBG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERR,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_OFF
};

static const LOG_LEVEL_CLASS LOG_LEVEL = LOG_LEVEL_CLASS::LOG_LEVEL_INFO;
static const int32_t INVALID_FD = -1;
static int32_t g_DebugLogFilleDes = INVALID_FD;
static int32_t g_StdErrFilleDes = INVALID_FD;
#ifndef DFX_NO_PRINT_LOG
#ifndef DFX_LOG_USE_HILOG_BASE
static const OHOS::HiviewDFX::HiLogLabel g_LOG_LABEL = {LOG_CORE, 0xD002D20, "DfxFaultLogger"};
#endif
#endif

int DfxLogDebug(const char *format, ...)
{
#ifdef DFX_NO_PRINT_LOG
    return 0;
#else
    if (LOG_LEVEL_CLASS::LOG_LEVEL_DBG < LOG_LEVEL) {
        return 0;
    }

    int ret;
    char buf[LOG_BUF_LEN] = {0};
    va_list args;
    va_start(args, format);
    ret = vsnprintf_s(buf, sizeof(buf), sizeof(buf) - 1, format, args);
    va_end(args);
#ifdef DFX_LOG_USE_HILOG_BASE
    HILOG_BASE_DEBUG(LOG_CORE, "%{public}s", buf);
#else
    OHOS::HiviewDFX::HiLog::Debug(g_LOG_LABEL, "%{public}s", buf);
#endif

    if (g_DebugLogFilleDes != INVALID_FD) {
        fprintf(stderr, "%s\n", buf);
    }
    return ret;
#endif
}

int DfxLogInfo(const char *format, ...)
{
#ifdef DFX_NO_PRINT_LOG
    return 0;
#else
    if (LOG_LEVEL_CLASS::LOG_LEVEL_INFO < LOG_LEVEL) {
        return 0;
    }

    int ret;
    char buf[LOG_BUF_LEN] = {0};
    va_list args;
    va_start(args, format);
    ret = vsnprintf_s(buf, sizeof(buf), sizeof(buf) - 1, format, args);
    va_end(args);
#ifdef DFX_LOG_USE_HILOG_BASE
    HILOG_BASE_INFO(LOG_CORE, "%{public}s", buf);
#else
    OHOS::HiviewDFX::HiLog::Info(g_LOG_LABEL, "%{public}s", buf);
#endif

    if (g_DebugLogFilleDes != INVALID_FD) {
        fprintf(stderr, "%s\n", buf);
    }
    return ret;
#endif
}

int DfxLogWarn(const char *format, ...)
{
#ifdef DFX_NO_PRINT_LOG
    return 0;
#else
    if (LOG_LEVEL_CLASS::LOG_LEVEL_WARN < LOG_LEVEL) {
        return 0;
    }

    int ret;
    char buf[LOG_BUF_LEN] = {0};
    va_list args;
    va_start(args, format);
    ret = vsnprintf_s(buf, sizeof(buf), sizeof(buf) - 1, format, args);
    va_end(args);
#ifdef DFX_LOG_USE_HILOG_BASE
    HILOG_BASE_WARN(LOG_CORE, "%{public}s", buf);
#else
    OHOS::HiviewDFX::HiLog::Warn(g_LOG_LABEL, "%{public}s", buf);
#endif

    if (g_DebugLogFilleDes != INVALID_FD) {
        fprintf(stderr, "%s\n", buf);
    }
    return ret;
#endif
}

int DfxLogError(const char *format, ...)
{
#ifdef DFX_NO_PRINT_LOG
    return 0;
#else
    if (LOG_LEVEL_CLASS::LOG_LEVEL_ERR < LOG_LEVEL) {
        return 0;
    }

    int ret;
    char buf[LOG_BUF_LEN] = {0};
    va_list args;
    va_start(args, format);
    ret = vsnprintf_s(buf, sizeof(buf), sizeof(buf) - 1, format, args);
    va_end(args);
#ifdef DFX_LOG_USE_HILOG_BASE
    HILOG_BASE_ERROR(LOG_CORE, "%{public}s", buf);
#else
    OHOS::HiviewDFX::HiLog::Error(g_LOG_LABEL, "%{public}s", buf);
#endif

    if (g_DebugLogFilleDes != INVALID_FD) {
        fprintf(stderr, "%s\n", buf);
    }
    return ret;
#endif
}

int DfxLogFatal(const char *format, ...)
{
#ifdef DFX_NO_PRINT_LOG
    return 0;
#else
    if (LOG_LEVEL_CLASS::LOG_LEVEL_FATAL < LOG_LEVEL) {
        return 0;
    }

    int ret;
    char buf[LOG_BUF_LEN] = {0};
    va_list args;
    va_start(args, format);
    ret = vsnprintf_s(buf, sizeof(buf), sizeof(buf) - 1, format, args);
    va_end(args);
#ifdef DFX_LOG_USE_HILOG_BASE
    HILOG_BASE_FATAL(LOG_CORE, "%{public}s", buf);
#else
    OHOS::HiviewDFX::HiLog::Fatal(g_LOG_LABEL, "%{public}s", buf);
#endif
    if (g_DebugLogFilleDes != INVALID_FD) {
        fprintf(stderr, "%s\n", buf);
    }
    return ret;
#endif
}

int WriteLog(int32_t fd, const char *format, ...)
{
    int ret;
    char buf[FILE_WRITE_BUF_LEN] = {0};
    va_list args;
    va_start(args, format);
    ret = vsnprintf_s(buf, sizeof(buf), sizeof(buf) - 1, format, args);
    va_end(args);

    if ((LOG_LEVEL <= LOG_LEVEL_CLASS::LOG_LEVEL_DBG) || (fd == -1)) {
#ifndef DFX_NO_PRINT_LOG
#ifdef DFX_LOG_USE_HILOG_BASE
        HILOG_BASE_DEBUG(LOG_CORE, "%{public}s", buf);
#else
        OHOS::HiviewDFX::HiLog::Debug(g_LOG_LABEL, "%{public}s", buf);
#endif
#endif
    }

    if (g_DebugLogFilleDes != INVALID_FD) {
        fprintf(stderr, "%s", buf);
    }

    if (fd != -1) {
        ret = dprintf(fd, "%s", buf);
        if (ret < 0) {
            DfxLogError("WriteLog :: write msg(%s) to fd(%d) failed, ret(%d).", buf, fd, ret);
        } else {
            // if we write msg to normal file then try fsync, otherwise we needn't do this.
            if (fd != STDOUT_FILENO) {
                ret = fsync(fd);
                if (ret < 0) {
                    DfxLogError("WriteLog :: fsync failed, ret(%d).", ret);
                }
            }
        }
    }

    return ret;
}

void DfxLogToSocket(const char *msg)
{
    if (LOG_LEVEL_CLASS::LOG_LEVEL_DBG < LOG_LEVEL) {
        return;
    }

    size_t length = strlen(msg);
    if (length >= LOG_BUF_LEN) {
        return;
    }
    RequestPrintTHilog(msg, length);
}

#ifndef DFX_LOG_USE_HILOG_BASE
void InitDebugLog(int type, int pid, int tid, int uid, int isLogPersist)
{
    DfxLogInfo("InitDebugLog :: type(%d), pid(%d), tid(%d), uid(%d), isLogPersist(%d).",
        type, pid, tid, uid, isLogPersist);
    if (g_DebugLogFilleDes != INVALID_FD) {
        return;
    }
    if (!isLogPersist) {
        return;
    }

    struct FaultLoggerdRequest faultloggerdRequest;
    if (memset_s(&faultloggerdRequest, sizeof(faultloggerdRequest), 0, sizeof(struct FaultLoggerdRequest)) != 0) {
        return;
    }
    faultloggerdRequest.type = (int)type;
    faultloggerdRequest.pid = pid;
    faultloggerdRequest.tid = tid;
    faultloggerdRequest.uid = uid;

    g_DebugLogFilleDes = RequestLogFileDescriptor(&faultloggerdRequest);
    if (g_DebugLogFilleDes <= 0) {
        DfxLogError("InitDebugLog :: RequestLogFileDescriptor failed.");
        g_DebugLogFilleDes = INVALID_FD;
    } else {
        g_StdErrFilleDes = dup(STDERR_FILENO);

        if (dup2(g_DebugLogFilleDes, STDERR_FILENO) == -1) {
            DfxLogError("InitDebugLog :: dup2 failed.");
            close(g_DebugLogFilleDes);
            g_DebugLogFilleDes = INVALID_FD;
            g_StdErrFilleDes = INVALID_FD;
        }
    }
}

void CloseDebugLog()
{
    if (g_DebugLogFilleDes == INVALID_FD) {
        return;
    }

    if (g_StdErrFilleDes != INVALID_FD) {
        dup2(g_StdErrFilleDes, STDERR_FILENO);
    }
    close(g_DebugLogFilleDes);
    g_DebugLogFilleDes = INVALID_FD;
    g_StdErrFilleDes = INVALID_FD;
}
#endif