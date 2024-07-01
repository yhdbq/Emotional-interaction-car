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

#ifndef HILOGTOOL_MSG_H
#define HILOGTOOL_MSG_H

#include <cstdint>
#include <string>
#include <stdint.h>
#include <time.h>
#include "hilog_common.h"

#define FILE_PATH_MAX_LEN 100
#define JOB_ID_ALL 0xffffffff
typedef enum {
    LOG_QUERY_REQUEST = 0x01,
    LOG_QUERY_RESPONSE,
    NEXT_REQUEST,
    NEXT_RESPONSE,
    NEW_DATA_NOTIFY,
    MC_REQ_LOG_PERSIST_START,
    MC_RSP_LOG_PERSIST_START,
    MC_REQ_LOG_PERSIST_STOP,
    MC_RSP_LOG_PERSIST_STOP,
    MC_REQ_LOG_PERSIST_QUERY,
    MC_RSP_LOG_PERSIST_QUERY,
    MC_REQ_BUFFER_RESIZE,        // set buffer request
    MC_RSP_BUFFER_RESIZE,        // set buffer response
    MC_REQ_BUFFER_SIZE,          // query buffer size
    MC_RSP_BUFFER_SIZE,          // query buffer size
    MC_REQ_STATISTIC_INFO_QUERY, // statistic info query request
    MC_RSP_STATISTIC_INFO_QUERY, // statistic info query response
    MC_REQ_STATISTIC_INFO_CLEAR, // statistic info clear request
    MC_RSP_STATISTIC_INFO_CLEAR, // statistic info clear response
    MC_REQ_FLOW_CONTROL,         // set flow control request
    MC_RSP_FLOW_CONTROL,         // set flow control response
    MC_REQ_LOG_CLEAR,            // clear log request
    MC_RSP_LOG_CLEAR             // clear log response
} OperationCmd;

/*
 * property operation, such as private switch ,log level, flow switch
 */
typedef enum {
    OT_PRIVATE_SWITCH = 0x01,
    OT_LOG_LEVEL,
    OT_FLOW_SWITCH,
} OperateType;


typedef enum {
    CREATE_SUCCESS = 1,
    CREATE_DUPLICATE = 2,
    CREATE_DENIED = 3,
    QUERY_SUCCESS = 4,
    QUERY_NOT_EXIST = 5,
    DELETE_SUCCESS = 6,
    DELETE_DENIED = 7,
} PersisterResponse;

typedef enum {
    COMPRESS_TYPE_NONE = 0,
    COMPRESS_TYPE_ZSTD,
    COMPRESS_TYPE_ZLIB,
} CompressAlg;

typedef enum {
    OFF_SHOWFORMAT = 0,
    COLOR_SHOWFORMAT,
    TIME_SHOWFORMAT,
    TIME_USEC_SHOWFORMAT,
    YEAR_SHOWFORMAT,
    ZONE_SHOWFORMAT,
    EPOCH_SHOWFORMAT,
    MONOTONIC_SHOWFORMAT,
    TIME_NSEC_SHOWFORMAT,
} HilogShowFormat;

typedef struct {
    uint8_t version;
    uint8_t msgType;  // hilogtool-hilogd message type
    uint16_t msgLen;  // message length
} MessageHeader;

typedef struct {
    MessageHeader header;
    uint8_t nPid;
    uint8_t nNoPid;
    uint8_t nDomain;
    uint8_t nNoDomain;
    uint8_t nTag;
    uint8_t nNoTag;
    uint8_t levels;
    uint16_t types;
    uint32_t pids[MAX_PIDS];
    uint32_t domains[MAX_DOMAINS];
    char tags[MAX_TAGS][MAX_TAG_LEN];
    uint16_t logCount;
    uint8_t noLevels;
    uint16_t noTypes;
    uint32_t noPids[MAX_PIDS];
    uint32_t noDomains[MAX_DOMAINS];
    char noTags[MAX_TAGS][MAX_TAG_LEN];
} LogQueryRequest;

typedef struct {
    uint16_t sendId;
    uint16_t length; /* data len, equals tag_len plus content length, include '\0' */
    uint16_t level;
    uint16_t type;
    uint16_t tag_len; /* include '\0' */
    uint32_t pid;
    uint32_t tid;
    uint32_t domain;
    uint32_t tv_sec;
    uint32_t tv_nsec;
    char data[]; /* tag and content, include '\0' */
} HilogDataMessage;

typedef struct {
    MessageHeader header;
    HilogDataMessage data;
} LogQueryResponse;

typedef struct {
    MessageHeader header;
} NewDataNotify;

typedef struct {
    MessageHeader header;
    uint16_t sendId;
} NextRequest;

typedef struct {
    MessageHeader header;
    uint16_t sendId;
    char data[];
} NextResponce;

typedef struct {
    uint16_t logType;
} BuffSizeMsg;

typedef struct {
    MessageHeader msgHeader;
    BuffSizeMsg buffSizeMsg[];
} BufferSizeRequest;

typedef struct {
    uint16_t logType;
    uint64_t buffSize;
    int32_t result;
} BuffSizeResult;

typedef struct {
    MessageHeader msgHeader;
    BuffSizeResult buffSizeRst[];
} BufferSizeResponse;

typedef struct {
    uint16_t logType;
    uint64_t buffSize;
} BuffResizeMsg;

typedef struct {
    MessageHeader msgHeader;
    BuffResizeMsg buffResizeMsg[];
} BufferResizeRequest;

typedef struct {
    uint16_t logType;
    uint64_t buffSize;
    int32_t result;
} BuffResizeResult;

typedef struct {
    MessageHeader msgHeader;
    BuffResizeResult buffResizeRst[];
} BufferResizeResponse;

typedef struct {
    MessageHeader msgHeader;
    uint16_t logType;
    uint32_t domain;
} StatisticInfoQueryRequest;

typedef struct {
    MessageHeader msgHeader;
    int32_t result;
    uint16_t logType;
    uint32_t domain;
    uint64_t printLen;
    uint64_t cacheLen;
    int32_t dropped;
} StatisticInfoQueryResponse;

typedef struct {
    MessageHeader msgHeader;
    uint16_t logType;
    uint32_t domain;
} StatisticInfoClearRequest;

typedef struct {
    MessageHeader msgHeader;
    int32_t result;
    uint16_t logType;
    uint32_t domain;
} StatisticInfoClearResponse;

typedef struct {
    uint16_t logType;
} LogClearMsg;

typedef struct {
    MessageHeader msgHeader;
    LogClearMsg logClearMsg[];
} LogClearRequest;

typedef struct {
    uint16_t logType;
    int32_t result;
} LogClearResult;

typedef struct {
    MessageHeader msgHeader;
    LogClearResult logClearRst[];
} LogClearResponse;

typedef struct {
    std::string logTypeStr;
    std::string compressAlgStr;
    std::string fileSizeStr;
    std::string fileNumStr;
    std::string fileNameStr;
    std::string jobIdStr;
} LogPersistParam;
typedef struct {
    uint16_t logType; // union logType
    uint16_t compressAlg;
    char filePath[FILE_PATH_MAX_LEN];
    uint32_t fileSize;
    uint32_t fileNum;
    uint32_t jobId;
} LogPersistStartMsg;
typedef struct {
    MessageHeader msgHeader;
    LogPersistStartMsg logPersistStartMsg;
} LogPersistStartRequest;

typedef struct {
    int32_t result;
    uint32_t jobId;
} LogPersistStartResult;

typedef struct {
    MessageHeader msgHeader;
    LogPersistStartResult logPersistStartRst;
} LogPersistStartResponse;

typedef struct {
    uint32_t jobId;
} LogPersistStopMsg;
typedef struct {
    MessageHeader msgHeader;
    LogPersistStopMsg logPersistStopMsg[];
} LogPersistStopRequest;

typedef struct {
    int32_t result;
    uint32_t jobId;
} LogPersistStopResult;
typedef struct {
    MessageHeader msgHeader;
    LogPersistStopResult logPersistStopRst[];
} LogPersistStopResponse;

typedef struct {
    uint16_t logType;
} LogPersistQueryMsg;
typedef struct {
    MessageHeader msgHeader;
    LogPersistQueryMsg logPersistQueryMsg;
} LogPersistQueryRequest;

typedef struct {
    int32_t result;
    uint32_t jobId;
    uint16_t logType;
    uint16_t compressAlg;
    char filePath[FILE_PATH_MAX_LEN];
    uint32_t fileSize;
    uint32_t fileNum;
} LogPersistQueryResult;
typedef struct {
    MessageHeader msgHeader;
    LogPersistQueryResult logPersistQueryRst[];
} LogPersistQueryResponse;

typedef struct {
    std::string privateSwitchStr;
    std::string flowSwitchStr;
    std::string logLevelStr;
    std::string domainStr;
    std::string tagStr;
    std::string pidStr;
} SetPropertyParam;

#endif /* HILOGTOOL_MSG_H */
