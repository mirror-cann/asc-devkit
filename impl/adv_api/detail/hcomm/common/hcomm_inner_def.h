/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file hcomm_inner_def.h
 * \brief Hcomm inner definition
 */

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/adv_api/detail/hcomm/common/hcomm_inner_def.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"adv_api/activation/simplesoftmax.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCOMM_INNER_DEF_H__
#endif

#ifndef IMPL_ADV_API_DETAIL_HCOMM_COMMON_HCOMM_INNER_DEF_H
#define IMPL_ADV_API_DETAIL_HCOMM_COMMON_HCOMM_INNER_DEF_H

#include <cstdint>

namespace AscendC {

#pragma pack(push, 1)
struct UrmaWqeEntry {
    uint32_t odr : 3;       // ordering, request ordering strength (0=unordered, 5=strongly ordered)
    uint32_t fence : 1;     // fence, 1=current WQE waits for all prior WQEs to complete
    uint32_t se : 1;        // solicited event, 1=trigger remote event notification on completion
    uint32_t cqe : 1;       // completion queue entry, 1=generate CQE on completion
    uint32_t inlineEn : 1;  // inline enable, 1=data inlined in WQE instead of SGE reference
    uint32_t udfFlag : 1;   // user-defined flag
} __attribute__((packed));
#pragma pack(pop)

static constexpr UrmaWqeEntry URMA_DEFAULT_CFG = {
    .odr        = 5,
    .fence      = 1,
    .se         = 0,
    .cqe        = 1,
    .inlineEn   = 0,
    .udfFlag    = 0
};

constexpr int8_t HCOMM_INVALID_HANDLE_ID = -1;
constexpr uint32_t HCOMM_MAX_HANDLE_ID = 1024;

typedef enum {
    PROTECTION_TYPE_INVALID = -1,
    PROTECTION_TYPE_ROCE = 0,
    PROTECTION_TYPE_UB = 1,
} ProtectionType;

typedef enum {
    SQ_CONTEXT_TYPE_INVALID = -1,
    SQ_CONTEXT_TYPE_UB_JFS = 0,
    SQ_CONTEXT_TYPE_ROCE = 1,
} SqContextType;

typedef enum {
    CQ_CONTEXT_TYPE_INVALID = -1,
    CQ_CONTEXT_TYPE_UB_JFC = 0,
    CQ_CONTEXT_TYPE_ROCE = 1,
} CqContextType;

typedef enum {
    REGED_NOTIFY_INVALID = -1,
    REGED_NOTIFY_IPC_RT = 0,
    REGED_NOTIFY_IPC_MEM = 1,
    REGED_NOTIFY_RMA_RT = 2,
    REGED_NOTIFY_RMA_MEM = 3,
} RegedNotifyType;

typedef enum {
    REGED_BUFFER_INVALID = -1,
    REGED_BUFFER_IPC = 0,
    REGED_BUFFER_RMA = 1,
} RegedBufferType;

typedef struct {
    uint32_t version;
    uint32_t magicWord;
    uint32_t size;
    uint32_t reserved;
} CommAbiHeader;

typedef union {
    struct {
        uint32_t lkey;
        uint32_t rkey;
    } roce;
    struct {
        uint32_t tokenId;
        uint32_t tokenValue;
    } ub;
    uint8_t raws[24];
    struct {
        uint32_t lkey;
        uint32_t rkey;
    } rdmaMemProtectionInfo;
    struct {
        uint32_t tokenID;
        uint32_t tokenValue;
    } urmaMemProtectionInfo;
} HcommProtectionInfoData;

typedef struct {
    ProtectionType type;
    union {
        HcommProtectionInfoData memInfo;
        HcommProtectionInfoData pti;
    };
} ProtectionInfo;

typedef union {
    struct {
        uint64_t sqVa;
        uint64_t headAddr;
        uint64_t tailAddr;
        uint64_t dbVa;
        uint32_t jfsID;
        uint32_t wqeSize;
        uint32_t sqDepth;
        uint32_t tpID;
        uint8_t remoteEID[16];
    } ubJfs;
    struct {
        uint64_t sqVa;
        uint64_t headAddr;
        uint64_t tailAddr;
        uint64_t dbVa;
        uint32_t qpn;
        uint32_t wqeSize;
        uint32_t depth;
        int8_t dbMode;
        uint8_t sl;
    } roceSq;
    uint8_t raws[120];
    struct {
        uint32_t jfsID;
        uint64_t sqVa;
        uint32_t wqeSize;
        uint32_t sqDepth;
        uint32_t tpID;
        uint64_t headAddr;
        uint64_t tailAddr;
        uint8_t remoteEID[16];
        uint64_t dbVa;
    } jfsContext;
    struct {
        uint32_t qpn;
        uint64_t sqVa;
        uint32_t wqeSize;
        uint32_t depth;
        uint64_t headAddr;
        uint64_t tailAddr;
        uint8_t sl;
        uint64_t dbVa;
        int8_t dbMode;
    } rdmaSqContext;
} HcommSqContextInfo;

typedef struct {
    SqContextType type;
    union {
        HcommSqContextInfo contextInfo;
        HcommSqContextInfo ctx;
    };
} SqContext;

typedef union {
    struct {
        uint64_t scqVa;
        uint64_t headAddr;
        uint64_t tailAddr;
        uint64_t dbVa;
        uint32_t jfcID;
        uint32_t cqeSize;
        uint32_t cqDepth;
    } ubJfc;
    struct {
        uint64_t cqVa;
        uint64_t headAddr;
        uint64_t tailAddr;
        uint64_t dbVa;
        uint32_t cqn;
        uint32_t cqeSize;
        uint32_t cqDepth;
        int8_t dbMode;
    } roceCq;
    uint8_t raws[120];
    struct {
        uint32_t jfcID;
        uint64_t scqVa;
        uint32_t cqeSize;
        uint32_t cqDepth;
        uint64_t headAddr;
        uint64_t tailAddr;
        uint64_t dbVa;
    } jfcContext;
    struct {
        uint32_t cqn;
        uint64_t cqVa;
        uint32_t cqeSize;
        uint32_t cqDepth;
        uint64_t headAddr;
        uint64_t tailAddr;
        uint64_t dbVa;
        int8_t dbMode;
    } rdmaCqContext;
} HcommCqContextInfo;

typedef struct {
    CqContextType type;
    union {
        HcommCqContextInfo contextInfo;
        HcommCqContextInfo ctx;
    };
} CqContext;

typedef struct {
    uint32_t type;
    union {
        struct {
            uint64_t address;
            int32_t notifyId;
            uint32_t size; // 默认4Byte
        } hccsNotify;
        struct {
            uint64_t address; // 创建notify的时候申请的内存的地址（寄存器的虚地址）
            int32_t notifyId; // remote 时不用
            uint32_t size;    // 默认4Byte
            ProtectionInfo protectionInfo;
        } rmaNotify;
        int8_t reserve[64];
    };
} Notify;

typedef struct {
    RegedBufferType type;
    union {
        struct {
            uint64_t addr;
            uint64_t size;
        } ipc;
        struct {
            uint64_t addr;
            uint64_t size;
            ProtectionInfo protectionInfo;
        } rma;
        uint8_t raws[56];
    } bufferInfo;
} RegedBufferEntity;

typedef struct {
    RegedNotifyType type;
    union {
        struct {
            uint64_t addr;
            uint32_t size;
            int32_t notifyId;
        } ipcRt;
        struct {
            uint64_t addr;
            uint32_t size;
        } ipcMem;
        struct {
            uint64_t addr;
            uint32_t size;
            int32_t notifyId;
            ProtectionInfo protectionInfo;
        } rmaRt;
        struct {
            uint64_t addr;
            uint32_t size;
            ProtectionInfo protectionInfo;
        } rmaMem;
        uint8_t raws[56];
    } notifyInfo;
} RegedNotifyEntity;

typedef struct {
    CommAbiHeader abiHeader;
    int32_t engine;
    int32_t protocol;
    uint32_t localNotifyNum;
    uint32_t remoteNotifyNum;
    uint32_t localBufferNum;
    uint32_t remoteBufferNum;
    uint32_t sqNum;
    uint32_t cqNum;
    RegedNotifyEntity* localNotifyAddr;
    RegedNotifyEntity* remoteNotifyAddr;
    RegedBufferEntity* localBufferAddr;
    RegedBufferEntity* remoteBufferAddr;
    SqContext* sqContextAddr;
    CqContext* cqContextAddr;
    uint8_t reserve[160];
} ChannelEntity;

// AIV直驱RoCE，无需考虑notify
// write with notify的语义，地址是GM地址
typedef struct {   // channel 本身一片内存
    uint32_t type; // RDMA, SDMA, URMA
    // local notify
    uint32_t localNotifyNum; // 几个local notify
    Notify* localNotifyAddr; // local notify的数组，分别指向不同的内存片
    // remote notify
    uint32_t remoteNotifyNum;
    Notify* remoteNotifyAddr;
    // Local buffer
    uint32_t localBufferNum;
    ProtectionInfo* localBufferAddr;
    // Remote buffer
    uint32_t remoteBufferNum;
    ProtectionInfo* remoteBufferAddr;
    // SQ
    uint32_t sqNum;
    SqContext* sqContextAddr; // write/read/commit 操作这个字段
    // CQ
    uint32_t cqNum;
    CqContext* cqContextAddr; // wait 操作这个字段
    // AIV Status
    uint8_t aivStatus[64];
    // reserve
    uint8_t reserve[1024];
} Channel;

// 宏隔离
typedef struct {
    // Control Segment
    union {
        struct {
            uint32_t o : 1; // Owner
            uint32_t ctrlSl : 2;
            uint32_t csl : 2;
            uint32_t difSl : 3;
            uint32_t cr : 1;
            uint32_t df : 1;
            uint32_t va : 1;
            uint32_t tsl : 5;
            uint32_t cf : 1;
            uint32_t wf : 1;
            uint32_t rsvd0 : 4;
            uint32_t rrvSl : 2;
            uint32_t bdsLen : 8;
        } bs;
        uint32_t value;
    } dw0;
    union {
        struct {
            uint32_t cl : 4;
            uint32_t rsvd1 : 8;
            uint32_t maskPi : 20;
        } bs;
        uint32_t value;
    } dw1;
} RoceWqeCtrlSeg;

typedef struct {
    // Task Segment
    union {
        struct {
            uint32_t se : 1;
            uint32_t f : 1;
            uint32_t c : 1;
            uint32_t opType : 5;
            uint32_t so : 1;
            uint32_t rsvd0 : 3;
            uint32_t dif : 1;
            uint32_t ext : 1;
            uint32_t xrcSrqn : 18;
        } bs;
        uint32_t value;
    } dw0;
} RoceWqeTaskSeg;

typedef struct {
    uint32_t bufAddrHigh32;
    uint32_t bufAddrLow32;
    uint32_t rLen;
    uint32_t leKey;
} RoceWqeDataSeg;

typedef struct {
    RoceWqeCtrlSeg ctrl;
    uint64_t doorbell;
    RoceWqeTaskSeg task;
    uint32_t dataLen;
    uint32_t immeData;
    uint32_t firstLast : 1;
    uint32_t nxtEthHdr : 7;
    uint32_t cmdLen : 8;
    uint32_t rsvd0 : 8;
    uint32_t lastExtLen : 8;
    uint32_t vaHigh32;
    uint32_t vaLow32;
    uint32_t rKey;
    uint32_t rsvd1;
    RoceWqeDataSeg data;
} RoceWqeEntry;

typedef struct {
    uint32_t cqe0;
    uint32_t cqe1;
    uint32_t cqe2;
    uint32_t cqe3;
    uint32_t cqe4;
    uint32_t cqe5;
    uint32_t cqe6;
    uint32_t cqe7;
} RoceCqeEntry;

typedef struct {
    uint32_t sqeBbIdx : 16;
    uint32_t flag : 8;
    uint32_t rsv0 : 3;
    uint32_t nf : 1;
    uint32_t tokenEn : 1;
    uint32_t rmtJettyType : 2;
    uint32_t owner : 1;
    uint32_t targetHint : 8;
    uint32_t opcode : 8;
    uint32_t rsv1 : 6;
    uint32_t inlineMsgLen : 10;
    uint32_t tpId : 24;
    uint32_t sgeNum : 8;
    uint32_t rmtJettyOrSegId : 20;
    uint32_t rsv2 : 12;
    uint64_t rmtEidL;
    uint64_t rmtEidH;
    uint32_t rmtTokenValue;
    uint32_t udfType : 8;
    uint32_t reduceDataType : 4;
    uint32_t reduceOpcode : 4;
    uint32_t rsv3 : 16;
    uint32_t rmtAddrLOrTokenId;
    uint32_t rmtAddrHOrTokenValue;
} HcommUrmaSqeCtx;

typedef struct {
    uint32_t len;
    uint32_t tokenId;
    uint64_t va;
} HcommUrmaSgeCtx;

typedef struct {
    uint32_t notifyTokenId : 20;
    uint32_t rsv : 12;
    uint32_t notifyTokenValue;
    uint32_t notifyAddrL;
    uint32_t notifyAddrH;
    uint32_t notifyDataL;
    uint32_t notifyDataH;
    uint32_t rsv2[2];
} HcommUrmaNotifyCtx;

typedef struct {
    uint32_t sR : 1;
    uint32_t isJetty : 1;
    uint32_t owner : 1;
    uint32_t inlineEn : 1;
    uint32_t opcode : 3;
    uint32_t fd : 1;
    uint32_t rsv : 8;
    uint32_t substatus : 8;
    uint32_t status : 8;
    uint32_t entryIdx : 16;
    uint32_t localNumL : 16;
    uint32_t localNumH : 4;
    uint32_t rmtIdx : 20;
    uint32_t rsv1 : 8;
    uint32_t tpn : 24;
    uint32_t rsv2 : 8;
    uint32_t byteCnt;
    uint32_t userDataL;
    uint32_t userDataH;
    uint32_t rmtEid[4];
    uint32_t dataL;
    uint32_t dataH;
    uint32_t inlineData[3];
} HcommUrmaJfcCqeCtx;

typedef struct {
    union {
        struct {
            uint32_t pi : 8;
            uint32_t resv : 8;
            uint32_t xrcvld : 1;
            uint32_t vxlan : 1;
            uint32_t mtuShift : 3;
            uint32_t sgidIndex : 7;
            uint32_t queueId : 4;
            uint32_t qpn : 20;
            uint32_t cntxSize : 2;
            uint32_t n : 1;
            uint32_t c : 1;
            uint32_t cos : 3;
            uint32_t type : 5;
        } bs;
        uint64_t value;
    } dw0;
} RoceDbEntry;

#define HCOMM_WQE_BDSL_OFFSET 0
#define HCOMM_WQE_TSL_OFFSET 16
#define HCOMM_WQE_VA_OFFSET 21
#define HCOMM_WQE_CR_OFFSET 23
#define HCOMM_WQE_CTRLSL_OFFSET 29
#define HCOMM_WQE_CL_OFFSET 28
#define HCOMM_WQE_OWNER_OFFSET 31
#define HCOMM_WQE_OP_TYPE_OFFSET 24
#define HCOMM_WQE_C_OFFSET 29

constexpr int32_t HCOMM_FAILED = -1;
constexpr int32_t HCOMM_SUCCESS = 0;
constexpr uint32_t HCOMM_MEM_BLOCK_SIZE = 32;

} // namespace AscendC
#endif // IMPL_HCOMM_HCOMM_INNER_DEF_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCOMM_INNER_DEF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_HCOMM_INNER_DEF_H__
#endif
