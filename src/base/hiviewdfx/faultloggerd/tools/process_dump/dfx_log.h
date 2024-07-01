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
#ifndef DFX_PROCESS_DUMP_LOG_H
#define DFX_PROCESS_DUMP_LOG_H

#ifndef DFX_NO_PRINT_LOG
#ifdef DFX_LOG_USE_HILOG_BASE
#include <hilog_base/log_base.h>
#else
#include <hilog/log.h>
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif
int DfxLogDebug(const char *format, ...);
int DfxLogInfo(const char *format, ...);
int DfxLogWarn(const char *format, ...);
int DfxLogError(const char *format, ...);
int DfxLogFatal(const char *format, ...);
int WriteLog(int fd, const char *format, ...);
void DfxLogToSocket(const char *msg);
#ifndef DFX_LOG_USE_HILOG_BASE
void InitDebugLog(int type, int pid, int tid, int uid, int isLogPersist);
void CloseDebugLog(void);
#endif
#ifdef __cplusplus
}
#endif

#endif
