/*
 * Copyright 2018 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO LICENSEE:
 *
 * This source code and/or documentation ("Licensed Deliverables") are
 * subject to NVIDIA intellectual property rights under U.S. and
 * international Copyright laws.
 *
 * These Licensed Deliverables contained herein is PROPRIETARY and
 * CONFIDENTIAL to NVIDIA and is being provided under the terms and
 * conditions of a form of NVIDIA software license agreement by and
 * between NVIDIA and Licensee ("License Agreement") or electronically
 * accepted by Licensee.  Notwithstanding any terms or conditions to
 * the contrary in the License Agreement, reproduction or disclosure
 * of the Licensed Deliverables to any third party without the express
 * written consent of NVIDIA is prohibited.
 *
 * NOTWITHSTANDING ANY TERMS OR CONDITIONS TO THE CONTRARY IN THE
 * LICENSE AGREEMENT, NVIDIA MAKES NO REPRESENTATION ABOUT THE
 * SUITABILITY OF THESE LICENSED DELIVERABLES FOR ANY PURPOSE.  IT IS
 * PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.
 * NVIDIA DISCLAIMS ALL WARRANTIES WITH REGARD TO THESE LICENSED
 * DELIVERABLES, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY,
 * NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * NOTWITHSTANDING ANY TERMS OR CONDITIONS TO THE CONTRARY IN THE
 * LICENSE AGREEMENT, IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY
 * SPECIAL, INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THESE LICENSED DELIVERABLES.
 *
 * U.S. Government End Users.  These Licensed Deliverables are a
 * "commercial item" as that term is defined at 48 C.F.R. 2.101 (OCT
 * 1995), consisting of "commercial computer software" and "commercial
 * computer software documentation" as such terms are used in 48
 * C.F.R. 12.212 (SEPT 1995) and is provided to the U.S. Government
 * only as a commercial end item.  Consistent with 48 C.F.R.12.212 and
 * 48 C.F.R. 227.7202-1 through 227.7202-4 (JUNE 1995), all
 * U.S. Government End Users acquire the Licensed Deliverables with
 * only those rights set forth herein.
 *
 * Any use of the Licensed Deliverables in individual and commercial
 * software must include, in the user documentation and internal
 * comments to the code, the above Disclaimer and U.S. Government End
 * Users Notice.
 */


#if !defined(__SANITIZER_CALLBACKS_H__)
#define __SANITIZER_CALLBACKS_H__

#include <sanitizer_result.h>

#include <cuda.h>

#include <stdint.h>

#ifndef SANITIZERAPI
#ifdef _WIN32
#define SANITIZERAPI __stdcall
#else
#define SANITIZERAPI
#endif
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * \defgroup SANITIZER_CALLBACK_API Sanitizer Callback API
 * Functions, types, and enums that implement the Sanitizer Callback API.
 * @{
 */

/**
 * \addtogroup SANITIZER_CALLBACK_API
 * @{
 */

/**
 * \brief Callback domains.
 *
 * Callback domain. Each domain represents callback points for a group of
 * related API functions or CUDA driver activity.
 */
typedef enum {
    /**
     * Invalid domain.
     */
    SANITIZER_CB_DOMAIN_INVALID     = 0,

    /**
     * Domain containing callback points for all driver API functions.
     */
    SANITIZER_CB_DOMAIN_DRIVER_API  = 1,

    /**
     * Domain containing callback points for all runtime API functions.
     */
    SANITIZER_CB_DOMAIN_RUNTIME_API = 2,

    /**
     * Domain containing callback points for CUDA resource tracking.
     */
    SANITIZER_CB_DOMAIN_RESOURCE    = 3,

    /**
     * Domain containing callback points for CUDA synchronization.
     */
    SANITIZER_CB_DOMAIN_SYNCHRONIZE = 4,

    /**
     * Domain containing callback points for CUDA grid launches.
     */
    SANITIZER_CB_DOMAIN_LAUNCH      = 5,

    /**
     * Domain containing callback points for CUDA memcpy operations.
     */
    SANITIZER_CB_DOMAIN_MEMCPY      = 6,

    /**
     * Domain containing callback points for CUDA memset operations.
     */
    SANITIZER_CB_DOMAIN_MEMSET      = 7,

    /**
     * Domain containing callback points for CUDA batch memop operations.
     */
    SANITIZER_CB_DOMAIN_BATCH_MEMOP = 8,

    /**
     * Domain containing callback points for CUDA managed memory operations.
     */
    SANITIZER_CB_DOMAIN_UVM         = 9,

    SANITIZER_CB_DOMAIN_SIZE,
    SANITIZER_CB_DOMAIN_FORCE_INT   = 0x7fffffff
} Sanitizer_CallbackDomain;

/**
 * \brief Specifies the point in an API call that a callback is issued.
 *
 * Specifies the point in an API that a callback is issued. This value is
 * communicated to the callback function via \ref
 * Sanitizer_CallbackData::CallbackSize.
 */
typedef enum {
    /**
     * This callback is at API entry.
     */
    SANITIZER_API_ENTER            = 0,

    /**
     * This callback is at API exit.
     */
    SANITIZER_API_EXIT             = 1,

    SANITIZER_API_CBSITE_FORCE_INT = 0x7fffffff
} Sanitizer_ApiCallbackSite;

/**
 * \brief Data passed into a runtime or driver API callback function.
 *
 * Data passed into a runtime or driver API callback function as the
 * \p cbdata argument to \ref Sanitizer_CallbackFunc. The \p cbdata will
 * be this type for \p domain equal to SANITIZER_CB_DOMAIN_DRIVER_API or
 * SANITIZER_CB_DOMAIN_RUNTIME_API. The callback data is valid only within
 * the invocation of the callback function that is passed the data. If
 * you need to retain some data for use outside of the callback, you
 * must make of a copy of that data. For example, if you make a shallow
 * copy of Sanitizer_CallbackData within a callback, you cannot
 * dereference \p functionParams outside of that callback to access
 * the function parameters. \p functionName is an exception: the
 * string pointed to by \p functionName is a global constant and so
 * may be accessed outside of the callback.
 */
typedef struct {
    /**
     * Point in the runtime or driver function from where the callback
     * was issued.
     */
    Sanitizer_ApiCallbackSite callbackSite;

    /**
     * Name of the runtime or driver API function which issued the
     * callback. This string is a global constant and so may be
     * accessed outside of the callback.
     */
    const char* functionName;

    /**
     * Pointer to the arguments passed to the runtime or driver API
     * call. See generated_cuda_runtime_api_meta.h and
     * generated_cuda_meta.h for structure definitions for the
     * parameters for each runtime and driver API function.
     */
    const void* functionParams;

    /**
     * Pointer to the return value of the runtime or driver API
     * call. This field is only valid within the SANITIZER_API_EXIT
     * callback. For a runtime API \p functionReturnValue points to a
     * \p cudaError_t. For a driver API \p functionReturnValue points
     * to a \p CUresult.
     */
    const void* functionReturnValue;

    /**
     * Name of the symbol operated on by the runtime or driver API
     * function which issued the callback. This entry is valid only for
     * driver and runtime launch callbacks, where it returns the name of
     * the kernel.
     */
    const char* symbolName;

    /**
     * Driver context current to the thread, or null if no context is
     * current. This value can change from the entry to exit callback
     * of a runtime API function if the runtime initialized a context.
     */
    CUcontext context;
} Sanitizer_CallbackData;

/**
 * \brief Callback IDs for resource domain.
 *
 * Callback IDs for resource domain SANITIZER_CB_DOMAIN_RESOURCE. This
 * value is communicated to the callback function via the \p cbid
 * parameter.
 */
typedef enum {
    /**
     * Invalid resource callback ID.
     */
    SANITIZER_CBID_RESOURCE_INVALID                   = 0,

    /**
     * Driver initialization is finished.
     */
    SANITIZER_CBID_RESOURCE_INIT_FINISHED             = 1,

    /**
     * A new context is about to be created.
     */
    SANITIZER_CBID_RESOURCE_CONTEXT_CREATION_STARTING = 2,

    /**
     * A new context was created.
     */
    SANITIZER_CBID_RESOURCE_CONTEXT_CREATION_FINISHED = 3,

    /**
     * A context is about to be destroyed.
     */
    SANITIZER_CBID_RESOURCE_CONTEXT_DESTROY_STARTING  = 4,

    /**
     * A context was destroyed.
     */
    SANITIZER_CBID_RESOURCE_CONTEXT_DESTROY_FINISHED  = 5,

    /**
     * A new stream was created.
     */
    SANITIZER_CBID_RESOURCE_STREAM_CREATED            = 6,

    /**
     * A stream is about to be destroyed.
     */
    SANITIZER_CBID_RESOURCE_STREAM_DESTROY_STARTING   = 7,

    /**
     * A module was loaded.
     */
    SANITIZER_CBID_RESOURCE_MODULE_LOADED             = 8,

    /**
     * A module is about to be unloaded.
     */
    SANITIZER_CBID_RESOURCE_MODULE_UNLOAD_STARTING    = 9,

    /**
     * Device memory was allocated.
     */
    SANITIZER_CBID_RESOURCE_DEVICE_MEMORY_ALLOC       = 10,

    /**
     * Device memory was freed.
     */
    SANITIZER_CBID_RESOURCE_DEVICE_MEMORY_FREE        = 11,

    /**
     * Pinned host memory was allocated.
     */
    SANITIZER_CBID_RESOURCE_HOST_MEMORY_ALLOC         = 12,

    /**
     * Pinned host memory was freed.
     */
    SANITIZER_CBID_RESOURCE_HOST_MEMORY_FREE          = 13,

    SANITIZER_CBID_RESOURCE_SIZE,
    SANITIZER_CBID_RESOURCE_FORCE_INT                 = 0x7fffffff
} Sanitizer_CallbackIdResource;

/**
 * \brief Data passed into a context resource callback function.
 *
 * Data passed into a context resource callback function as the
 * \p cbdata argument to \ref Sanitizer_CallbackFunc. The
 * \p cbdata will be this type for \p domain equal to
 * SANITIZER_CB_DOMAIN_RESOURCE and \p cbid equal to
 * SANITIZER_CBID_RESOURCE_CONTEXT_CREATION_STARTING,
 * SANITIZER_CBID_RESOURCE_CONTEXT_CREATION_FINISHED,
 * SANITIZER_CBID_RESOURCE_CONTEXT_DESTROY_STARTING or
 * SANITIZER_CBID_RESOURCE_CONTEXT_DESTROY_FINISHED.
 * The callback data is only valid within the invocation of the
 * callback function that is passed the data. If you need to
 * retain some data for use outside of the callback, you must
 * make a copy of it.
 */
typedef struct {
    /**
     * The context being created or destroyed.
     */
    CUcontext context;

    /**
     * The device on which the context is being created or destroyed.
     * This field is only valid for SANITIZER_CBID_RESOURCE_CONTEXT_CREATION_*
     * callbacks
     */
    CUdevice device;
} Sanitizer_ResourceContextData;

/**
 * \brief Data passed into a stream resource callback function.
 *
 * Data passed into a stream resource callback function as the
 * \p cbdata argument to \ref Sanitizer_CallbackFunc. The
 * \p cbdata will be this type for \p domain equal to
 * SANITIZER_CB_DOMAIN_RESOURCE and \p cbid equal to
 * SANITIZER_CBID_RESOURCE_STREAM_CREATED or
 * SANITIZER_CBID_RESOURCE_STREAM_DESTROY_STARTING.
 * The callback data is only valid within the invocation of the
 * callback function that is passed the data. If you need to
 * retain some data for use outside of the callback, you must
 * make a copy of it.
 */
typedef struct {
    /**
     * The context containing the stream being created or
     * destroyed.
     */
    CUcontext context;

    /**
     * The stream being created or destroyed.
     */
    CUstream stream;
} Sanitizer_ResourceStreamData;

/**
 * \brief Data passed into a module resource callback function.
 *
 * Data passed into a module resource callback function as the
 * \p cbdata argument to \ref Sanitizer_CallbackFunc. The
 * \p cbdata will be this type for \p domain equal to
 * SANITIZER_CB_DOMAIN_RESOURCE and \p cbid equal to
 * SANITIZER_CBID_RESOURCE_MODULE_LOADED or
 * SANITIZER_CBID_RESOURCE_MODULE_UNLOAD_STARTING.
 * The callback data is only valid within the invocation of the
 * callback function that is passed the data. If you need to
 * retain some data for use outside of the callback, you must
 * make a copy of it.
 */
typedef struct {
    /**
     * The context containing the module being loaded or
     * unloaded.
     */
    CUcontext context;

    /**
     * The module being loaded or unloaded.
     */
    CUmodule module;

    /**
     * The size of the cubin.
     */
    size_t cubinSize;

    /**
     * Pointer to the associated cubin.
     */
    const char* pCubin;
} Sanitizer_ResourceModuleData;

/**
 * \brief Flags describing a memory allocation.
 *
 * Flags describing a memory allocation. These values are to
 * be used in order to interpret the value of
 * \ref Sanitizer_ResourceMemoryData::flags
 */
typedef enum {
    /**
     * Empty flag.
     */
    SANITIZER_MEMORY_FLAG_NONE            = 0,

    /**
     * Specifies that the allocation is static scoped to a
     * module.
     */
    SANITIZER_MEMORY_FLAG_MODULE          = 0x1,

    /**
     * Specifies that the allocation is managed memory.
     */
    SANITIZER_MEMORY_FLAG_MANAGED         = 0x2,

    /**
     * Species that the allocation accessible from the
     * host.
     */
    SANITIZER_MEMORY_FLAG_HOST_MAPPED     = 0x4,

    /**
     * Specifies that the allocation is pinned on the host.
     */
    SANITIZER_MEMORY_FLAG_HOST_PINNED     = 0x8,

    /**
     * Specifies that the allocation is located on a peer GPU.
     */
    SANITIZER_MEMORY_FLAG_PEER            = 0x10,

    /**
     * Specifies that the allocation is located on a peer GPU
     * supporting native atomics. This implies that
     * SANITIZER_MEMORY_FLAG_PEER is set as well.
     */
    SANITIZER_MEMORY_FLAG_PEER_ATOMIC     = 0x20,

    SANITIZER_MEMORY_FLAG_FORCE_INT       = 0x7fffffff
} Sanitizer_ResourceMemoryFlags;

/**
 * \brief Specifies the visibility of an allocation
 *
 * Specifies the visibility of an allocation. This is typically GLOBAL on
 * allocations made via cudaMalloc, cudaHostAlloc and similar APIs. This can
 * be GLOBAL or HOST for cudaMallocManaged allocations depending on the flags
 * parameter. This can be changed after allocation time using cudaMemAttachSingle
 * API (see SANITIZER_CBID_UVM_ATTACH_MEM for the corresponding callback).
 */
typedef enum {
    /**
     * Invalid memory visibility
     */
    SANITIZER_MEMORY_VISIBILITY_INVALID   = 0,

    /**
     * Memory can be accessed by any stream on any device
     * (see cudaMemAttachGlobal)
     */
    SANITIZER_MEMORY_VISIBILITY_GLOBAL    = 1,

    /**
     * Memory cannot be accessed by any stream on any device
     * (see cudaMemAttachHost)
     */
    SANITIZER_MEMORY_VISIBILITY_HOST      = 2,

    /**
     * Memory can only be accessed by a single stream on the associated device
     * (see cudaMemAttachSingle)
     */
    SANITIZER_MEMORY_VISIBILITY_STREAM    = 3,

    SANITIZER_MEMORY_VISIBILITY_FORCE_INT = 0x7fffffff
} Sanitizer_MemoryVisibility;

/**
 * \brief Data passed into a memory resource callback function.
 *
 * Data passed into a memory resource callback function as the
 * \p cbdata argument to \ref Sanitizer_CallbackFunc. The
 * \p cbdata will be this type for \p domain equal to
 * SANITIZER_CB_DOMAIN_RESOURCE and \p cbid equal to
 * SANITIZER_CBID_RESOURCE_DEVICE_MEMORY_ALLOC,
 * SANITIZER_CBID_RESOURCE_DEVICE_MEMORY_FREE,
 * SANITIZER_CBID_RESOURCE_HOST_MEMORY_ALLOC,
 * SANITIZER_CBID_RESOURCE_HOST_MEMORY_FREE,
 * SANITIZER_CBID_RESOURCE_SURFACE_OBJECT_CREATED or
 * SANITIZER_CBID_RESOURCE_SURFACE_OBJECT_DESTROY.
 * The callback data is only valid within the invocation of the
 * callback function that is passed the data. If you need to
 * retain some data for use outside of the callback, you must
 * make a copy of it.
 */
typedef struct {
    /**
     * Address of the allocation being created or destroyed.
     */
    uint64_t address;

    /**
     * Size of the allocation being created or destroyed.
     */
    uint64_t size;

    /**
     * Context containing the allocation being created or
     * destroyed.
     */
    CUcontext context;

    /**
     * Allocation details: use Sanitizer_ResourceMemoryFlags
     * to interpret this field.
     */
    uint64_t flags;

    /**
     * Visibility of the allocation.
     */
    Sanitizer_MemoryVisibility visibility;
} Sanitizer_ResourceMemoryData;

/**
 * \brief Callback IDs for synchronization domain.
 *
 * Callback IDs for resource domain
 * SANITIZER_CB_DOMAIN_SYNCHRONIZE. This value is
 * communicated to the callback function via the \p cbid
 * parameter.
 */
typedef enum {
    /**
     * Invalid synchronize callback ID.
     */
    SANITIZER_CBID_SYNCHRONIZE_INVALID              = 0,

    /**
     * Stream synchronization has completed for a given stream.
     */
    SANITIZER_CBID_SYNCHRONIZE_STREAM_SYNCHRONIZED  = 1,

    /**
     * Context synchronization has completed for a given context.
     */
    SANITIZER_CBID_SYNCHRONIZE_CONTEXT_SYNCHRONIZED = 2,

    SANITIZER_CBID_SYNCHRONIZE_SIZE,
    SANITIZER_CBID_SYNCHRONIZE_FORCE_INT            = 0x7fffffff
} Sanitizer_CallackIdSync;

/**
 * \brief Data passed into a synchronization callback function.
 *
 * Data passed into a synchronization callback function as the
 * \p cbdata argument to \ref Sanitizer_CallbackFunc. The
 * \p cbdata will be this type for \p domain equal to
 * SANITIZER_CB_DOMAIN_SYNCHRONIZE. The callback data is
 * only valid within the invocation of the callback function
 * that is passed the data. If you need to retain some data
 * for use outside of the callback, you must make a copy of it.
 */
typedef struct {
    /**
     * For SANITIZER_CBID_SYNCHRONIZE_CONTEXT_SYNCHRONIZED, this
     * is the context being synchronized. For
     * SANITIZER_CBID_SYNCHRONIZE_STREAM_SYNCHRONIZED, this is
     * the context of the stream being synchronized.
     */
    CUcontext context;

    /**
     * This field is only valid for
     * SANITIZER_CBID_SYNCHRONIZE_STREAM_SYNCHRONIZED. This is
     * the stream being synchronized.
     */
    CUstream stream;

    /**
     * Set to 1 if the stream is a per-thread stream.
     * This field is only valid for
     * SANITIZER_CBID_SYNCHRONIZE_STREAM_SYNCHRONIZED
     */
    uint8_t perThreadStream;
} Sanitizer_SynchronizeData;

/**
 * \brief Callback IDs for launch domain.
 *
 * Callback IDs for resource domain SANITIZER_CB_DOMAIN_LAUNCH.
 * This value is communicated to the callback function via
 * the \p cbid parameter.
 */
typedef enum {
    /**
     * Invalid launch callback ID.
     */
    SANITIZER_CBID_LAUNCH_INVALID             = 0,

    /**
     * A grid launch was initiated.
     */
    SANITIZER_CBID_LAUNCH_BEGIN               = 1,

    /**
     * A grid launch has completed syscalls setup.
     */
    SANITIZER_CBID_LAUNCH_AFTER_SYSCALL_SETUP = 2,

    /**
     * The grid launch is complete.
     */
    SANITIZER_CBID_LAUNCH_END                 = 3,

    SANITIZER_CBID_LAUNCH_SIZE,
    SANITIZER_CBID_LAUNCH_FORCE_INT           = 0x7fffffff
} Sanitizer_CallbackIdLaunch;

/**
 * \brief Data passed into a launch callback function.
 *
 * Data passed into a launch callback function as the
 * \p cbdata argument to \ref Sanitizer_CallbackFunc. The
 * \p cbdata will be this type for \p domain equal to
 * SANITIZER_CB_DOMAIN_LAUNCH. The callback data is
 * only valid within the invocation of the callback function
 * that is passed the data. If you need to retain some data
 * for use outside of the callback, you must make a copy of it.
 */
typedef struct {
    /**
     * The context where the grid is launched.
     */
    CUcontext context;

    /**
     * The stream where the grid is launched.
     */
    CUstream stream;

    /**
     * The module containing the grid code.
     */
    CUmodule module;

    /**
     * The function of the grid launch.
     */
    CUfunction function;

    /**
     * The name of the launched function.
     */
    const char *functionName;

    /** @{
     * Launch properties of the grid.
     * These values are only valid for SANITIZER_CBID_LAUNCH_BEGIN callback
     */
    uint32_t gridDim_x;
    uint32_t gridDim_y;
    uint32_t gridDim_z;
    uint32_t blockDim_x;
    uint32_t blockDim_y;
    uint32_t blockDim_z;
    /** @} */

    /**
     * Unique identifier of the grid launch.
     */
    uint64_t gridId;

    /**
     * Set to 1 if the stream is a per-thread stream.
     */
    uint8_t perThreadStream;
} Sanitizer_LaunchData;

/**
 * \brief Callback IDs for memcpy domain.
 *
 * Callback IDs for resource domain SANITIZER_CB_DOMAIN_MEMCPY.
 * This value is communicated to the callback function via
 * the \p cbid parameter.
 */
typedef enum {
    /**
     * Invalid memcpy callback ID.
     */
    SANITIZER_CBID_MEMCPY_INVALID   = 0,

    /**
     * A memcpy operation was initiated.
     */
    SANITIZER_CBID_MEMCPY_STARTING  = 1,

    SANITIZER_CBID_MEMCPY_SIZE,
    SANITIZER_CBID_MEMCPY_FORCE_INT = 0x7fffffff
} Sanitizer_CallbackIdMemcpy;

/**
 * \brief Memcpy direction.
 *
 * Indicates the direction of a memcpy, passed inside \p Sanitizer_Memcpydata.
 */
typedef enum {
    /**
     * Unknown memcpy direction
     */
    SANITIZER_MEMCPY_DIRECTION_UNKNOWN          = 0,
    /**
     * Memcpy from host to host.
     */
    SANITIZER_MEMCPY_DIRECTION_HOST_TO_HOST     = 1,
    /**
     * Memcpy from host to device.
     */
    SANITIZER_MEMCPY_DIRECTION_HOST_TO_DEVICE   = 2,
    /**
     * Memcpy from device to host.
     */
    SANITIZER_MEMCPY_DIRECTION_DEVICE_TO_HOST   = 3,
    /**
     * Memcpy from device to device.
     */
    SANITIZER_MEMCPY_DIRECTION_DEVICE_TO_DEVICE = 4,

    SANITIZER_MEMCPY_DIRECTION_SIZE,
    SANITIZER_MEMCPY_DIRECTION_FORCE_INT        = 0x7fffffff
} Sanitizer_MemcpyDirection;

/**
 * \brief Data passed into a memcpy callback function.
 *
 * Data passed into a launch callback function as the
 * \p cbdata argument to \ref Sanitizer_CallbackFunc. The
 * \p cbdata will be this type for \p domain equal to
 * SANITIZER_CB_DOMAIN_MEMCPY. The callback data is
 * only valid within the invocation of the callback function
 * that is passed the data. If you need to retain some data
 * for use outside of the callback, you must make a copy of it.
 */
typedef struct {
    /**
     * The context where the source allocation is located
     */
    CUcontext srcContext;

    /**
     * The context where the destination allocation is located
     */
    CUcontext dstContext;

    /**
     * The stream where the memcpy is executed.
     */
    CUstream stream;

    /**
     * The source allocation address.
     */
    uint64_t srcAddress;

    /**
     * The destination allocation address.
     */
    uint64_t dstAddress;

    /**
     * Size of the transfer in bytes.
     */
    uint64_t size;

    /**
     * Boolean value indicating if the transfer is
     * asynchronous.
     */
    uint32_t isAsync;

    /**
     * The direction of the transfer
     */
    Sanitizer_MemcpyDirection direction;

    /**
     * Set to 1 if the stream is a per-thread stream.
     */
    uint8_t perThreadStream;
} Sanitizer_MemcpyData;

/**
 * \brief Callback IDs for memset domain.
 *
 * Callback IDs for resource domain SANITIZER_CB_DOMAIN_MEMSET.
 * This value is communicated to the callback function via
 * the \p cbid parameter.
 */
typedef enum {
    /**
     * Invalid memset callback ID.
     */
    SANITIZER_CBID_MEMSET_INVALID   = 0,

    /**
     * A memset operation was initiated.
     */
    SANITIZER_CBID_MEMSET_STARTING  = 1,

    SANITIZER_CBID_MEMSET_SIZE,
    SANITIZER_CBID_MEMSET_FORCE_INT = 0x7fffffff
} Sanitizer_CallbackIdMemset;

/**
 * \brief Data passed into a memset callback function.
 *
 * Data passed into a launch callback function as the
 * \p cbdata argument to \ref Sanitizer_CallbackFunc. The
 * \p cbdata will be this type for \p domain equal to
 * SANITIZER_CB_DOMAIN_MEMSET. The callback data is
 * only valid within the invocation of the callback function
 * that is passed the data. If you need to retain some data
 * for use outside of the callback, you must make a copy of it.
 */
typedef struct {
    /**
     * The context where the allocation is located.
     */
    CUcontext context;

    /**
     * The stream where the memset is executed.
     */
    CUstream stream;

    /**
     * The address of the memset start.
     */
    uint64_t address;

    /**
     * Memset size configuration.
     */
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint32_t elementSize;

    /**
     * Value to be written.
     */
    uint32_t value;

    /**
     * Boolean value indicating if the transfer is
     * asynchronous.
     */
    uint32_t isAsync;

    /**
     * Set to 1 if the stream is a per-thread stream.
     */
    uint8_t perThreadStream;
} Sanitizer_MemsetData;

/**
 * \brief Callback IDs for batch memop domain.
 *
 * Callback IDs for resource domain
 * SANITIZER_CB_DOMAIN_BATCH_MEMOP. This value is communicated
 * to the callback function via the \p cbid parameter.
 */
typedef enum {
    /**
     * Invalid batch memop callback ID.
     */
    SANITIZER_CBID_BATCH_MEMOP_INVALID   = 0,

    /**
     * A batch memory operation was initiated.
     */
    SANITIZER_CBID_BATCH_MEMOP_WRITE     = 1,

    SANITIZER_CBID_BATCH_MEMOP_SIZE,
    SANITIZER_CBID_BATCH_MEMOP_FORCE_INT = 0x7fffffff
} Sanitizer_CallbackIdBatchMemop;

/**
 * \brief Specifies the type of batch memory operation.
 *
 * Specifies the type of batch memory operation reported by a
 * callback in domain SANITIZER_CB_DOMAIN_BATCH_MEMOP. This
 * value is communicated to the callback function via \ref
 * Sanitizer_BatchMemopData::type.
 */
typedef enum {
    /**
     * Batch memory operation size is 32 bits.
     */
    SANITIZER_BATCH_MEMOP_TYPE_32B       = 0,

    /**
     * Batch memory operation size is 64 bits.
     */
    SANITIZER_BATCH_MEMOP_TYPE_64B       = 1,

    SANITIZER_BATCH_MEMOP_TYPE_FORCE_INT = 0x7fffffff
} Sanitizer_BatchMemopType;

/**
 * \brief Data passed into a batch memop callback function.
 *
 * Data passed into a batch memop callback function as the
 * \p cbdata argument to \ref Sanitizer_CallbackFunc. The
 * \p cbdata will be this type for \p domain equal to
 * SANITIZER_CB_DOMAIN_BATCH_MEMOP. The callback data is
 * only valid within the invocation of the callback function
 * that is passed the data. If you need to retain some data
 * for use outside of the callback, you must make a copy of it.
 */
typedef struct {
    /**
     * The context where the allocation is located
     */
    CUcontext context;

    /**
     * The stream where the batch memop is executed.
     */
    CUstream stream;

    /**
     * The address to be written.
     */
    uint64_t address;

    /**
     * The value to be written.
     */
    uint64_t value;

    /**
     * Type of batch memory operation.
     */
    Sanitizer_BatchMemopType type;

    /**
     * Set to 1 if the stream is a per-thread stream.
     */
    uint8_t perThreadStream;
} Sanitizer_BatchMemopData;

/**
 * \brief Callback IDs for managed memory domain.
 *
 * Callback IDs for resource domain SANITIZER_CB_DOMAIN_UVM.
 * This value is communicated to the callback function via the \p cbid parameter.
 */
typedef enum {
    /**
     * Invalid managed memory callback ID.
     */
    SANITIZER_CBID_UVM_INVALID    = 0,

    /**
     * Modify the stream association of an allocation
     * (see cudaStreamAttachMemAsync)
     */
    SANITIZER_CBID_UVM_ATTACH_MEM = 1,

    SANITIZER_CBID_UVM_SIZE,
    SANITIZER_CBID_UVM_FORCE_ITN  = 0x7fffffff
} Sanitizer_CallbackIdUvm;

/**
 * \brief Data passed into a managed memory callback function
 *
 * Data passed into a managed memory callback function as the
 * \p cbdata argument to \ref Sanitizer_CallbackFunc. The
 * \p cbdata will be this type for \p domain equal to
 * SANITIZER_CB_DOMAIN_UVM. The callback data
 * is only valid within the invocation of the callback function
 * that is passed the data. If you need to retain some data
 * for use outside of the callback, you must make a copy of it.
 */
typedef struct {
    /**
     * The context where the allocation is located.
     */
    CUcontext context;

    /**
     * New visibility for the allocation.
     */
    Sanitizer_MemoryVisibility visibility;

    /**
     * The stream on which the memory is attached.
     * This is only valid if visibility is SANITIZER_MEMORY_VISIBILITY_STREAM
     */
    CUstream stream;

    /**
     * The address of the allocation.
     */
    uint64_t address;

    /**
     * Set to 1 if the stream is a per-thread stream.
     */
    uint8_t perThreadStream;
} Sanitizer_UvmData;

/**
 * \brief Callback ID
 */
typedef uint32_t Sanitizer_CallbackId;

/**
 * \brief Function type for a callback.
 *
 * Function type for a callback. The type of the data passed to the callback
 * in \p cbdata depends on the domain.
 * If \p domain is SANITIZER_CB_DOMAIN_DRIVER_API or SANITIZER_CB_DOMAIN_RUNTIME_API
 * the type of \p cbdata will be Sanitizer_CallbackData.
 * If \p domain is SANITIZER_CB_DOMAIN_RESOURCE
 * the type of \p cbdata will be dependent on cbid.
 * Refer to \ref Sanitizer_ResourceContextData,
 * \ref Sanitizer_ResourceStreamData,
 * \ref Sanitizer_ResourceModuleData and
 * \ref Sanitizer_ResourceMemoryFlags documentations.
 * If \p domain is SANITIZER_CB_DOMAIN_SYNCHRONIZE
 * the type of \p cbdata will be Sanitizer_SynchronizeData.
 * If \p domain is SANITIZER_CB_DOMAIN_LAUNCH
 * the type of \p cbdata will be Sanitizer_LaunchData.
 * If \p domain is SANITIZER_CB_DOMAIN_MEMCPY
 * the type of \p cbdata will be Sanitizer_MemcpyData.
 * If \p domain is SANITIZER_CB_DOMAIN_MEMSET
 * the type of \p cbdata will be Sanitizer_MemsetData.
 * If \p domain is SANITIZER_CB_DOMAIN_BATCH_MEMOP
 * the type of \p cbdata will be Sanitizer_BatchMemopData.
 */
typedef void (SANITIZERAPI *Sanitizer_CallbackFunc)(
    void *userdata,
    Sanitizer_CallbackDomain domain,
    Sanitizer_CallbackId cbid,
    const void *cbdata);

/**
 * \brief A callback subscriber.
 */
typedef struct Sanitizer_Subscriber_st *Sanitizer_SubscriberHandle;

/**
 * \brief Initialize a callback subscriber with a callback function and
 * user data.
 *
 * Initialize a callback subscriber with a callback function and (optionally)
 * a pointer to user data. The returned subscriber handle can be used to enable
 * and disable the callback for specific domains and callback IDs.
 * \note Only one subscriber can be registered at a time.
 * \note This function does not enable any callbacks.
 * \note \b Thread-safety: this function is thread safe.
 *
 * \param subscriber Returns handle to initialize subscriber
 * \param callback The callback function
 * \param userdata A pointer to user data. This data will be passed to the
 * callback function via the \p userdata parameter
 *
 * \retval SANITIZER_SUCCESS on success
 * \retval SANITIZER_ERROR_NOT_INITIALIZED if unable to initialize the sanitizer
 * \retval SANITIZER_ERROR_MAX_LIMIT_RACHED if there is already a sanitizer
 * subscriber
 * \retval SANITIZER_ERROR_INVALID_PARAMETER if \p subscriber is NULL
 */
SanitizerResult SANITIZERAPI sanitizerSubscribe(Sanitizer_SubscriberHandle* subscriber,
                                                Sanitizer_CallbackFunc callback,
                                                void* userdata);

/**
 * \brief Unregister a callback subscriber.
 *
 * Removes a callback subscriber so that no future callback will be issued to
 * that subscriber.
 * \note \b Thread-safety: this function is thread safe.
 *
 * \param subscriber Handle to the initialized subscriber
 *
 * \retval SANITIZER_SUCCESS on success
 * \retval SANITIZER_ERROR_NOT_INITIALIZED if unable to initialize the sanitizer
 * \retval SANITIZER_ERROR_INVALID_PARAMETER if \p subscriber is NULL or
 * not initialized
 */
SanitizerResult SANITIZERAPI sanitizerUnsubscribe(Sanitizer_SubscriberHandle subscriber);

/**
 * \brief Get the current enabled/disabled state of a callback for a specific
 * domain and function ID.
 *
 * Returns non-zero in \p *enable if the callback for a domain and callback
 * ID is enabled, and zero if not enabled.
 *
 * \note \b Thread-safety: a subscriber must serialize access to
 * sanitizerGetCallbackState, sanitizerEnableCallback, sanitizerEnableDomain, and
 * sanitizerEnableAllDomains. For example, if sanitizerGetCallbackState(sub, d,
 * c) and sanitizerEnableCallback(sub, d, c) are called concurrently, the
 * results are undefined.
 *
 * \param enable Returns non-zero if callback enabled, zero if not enabled
 * \param subscriber Handle to the initialized subscriber
 * \param domain The domain of the callback
 * \param cbid The ID of the callback
 *
 * \retval SANITIZER_SUCCESS on success
 * \retval SANITIZER_ERROR_NOT_INITIALIZED if unable to initialize the sanitizer
 * \retval SANITIZER_ERROR_INVALID_PARAMETER if \p enabled is NULL, or if
 * \p subscriber, \p domain or \p cbid is invalid.
 */
SanitizerResult SANITIZERAPI sanitizerGetCallbackState(uint32_t* enable,
                                                       Sanitizer_SubscriberHandle subscriber,
                                                       Sanitizer_CallbackDomain domain,
                                                       Sanitizer_CallbackId cbid);

/**
 * \brief Enable or disable callbacks for a specific domain and callback ID
 *
 * Enable or disable callbacks for a subscriber for a specific domain and
 * callback ID.
 *
 * \note \b Thread-safety: a subscriber must serialize access to
 * sanitizerGetCallbackState, sanitizerEnableCallback, sanitizerEnableDomain, and
 * sanitizerEnableAllDomains. For example, if sanitizerGetCallbackState(sub, d,
 * c) and sanitizerEnableCallback(sub, d, c) are called concurrently, the
 * results are undefined.
 *
 * \param enable New enable state for the callback. Zero disables the callback,
 * non-zero enables the callback
 * \param subscriber - Handle of the initialized subscriber
 * \param domain The domain of the callback
 * \param cbid The ID of the callback
 *
 * \retval SANITIZER_SUCCESS on success
 * \retval SANITIZER_ERROR_NOT_INITIALIZED if unable to initialize the sanitizer
 * \retval SANITIZER_ERROR_INVALID_PARAMETER if \p subscriber, \p domain or
 * \p cbid is invalid
 */
SanitizerResult SANITIZERAPI sanitizerEnableCallback(uint32_t enable,
                                                     Sanitizer_SubscriberHandle subscriber,
                                                     Sanitizer_CallbackDomain domain,
                                                     Sanitizer_CallbackId cbid);

/**
 * \brief Enable or disable all callbacks for a specific domain.
 *
 * Enable or disable all callbacks for a specific domain.
 *
 * \note \b Thread-safety: a subscriber must serialize access to
 * sanitizerGetCallbackState, sanitizerEnableCallback, sanitizerEnableDomain, and
 * sanitizerEnableAllDomains. For example, if sanitizerGetCallbackEnabled(sub,
 * d, *) and sanitizerEnableDomain(sub, d) are called concurrently, the
 * results are undefined.
 *
 * \param enable New enable state for all callbacks in the domain. Zero
 * disables all callbacks, non-zero enables all callbacks
 * \param subscriber - Handle of the initialized subscriber
 * \param domain The domain of the callback
 *
 * \retval SANITIZER_SUCCESS on success
 * \retval SANITIZER_ERROR_NOT_INITIALIZED if unable to initialize the sanitizer
 * \retval SANITIZER_ERROR_INVALID_PARAMETER if \p subscriber or \p domain is
 * invalid
 */
SanitizerResult SANITIZERAPI sanitizerEnableDomain(uint32_t enable,
                                                   Sanitizer_SubscriberHandle subscriber,
                                                   Sanitizer_CallbackDomain domain);

/**
 * \brief Enable or disable all callbacks in all domains.
 *
 * Enable or disable all callbacks in all domains.
 *
 * \note \b Thread-safety: a subscriber must serialize access to
 * sanitizerGetCallbackState, sanitizerEnableCallback, sanitizerEnableDomain, and
 * sanitizerEnableAllDomains. For example, if sanitizerGetCallbackState(sub,
 * d, *) and sanitizerEnableAllDomains(sub) are called concurrently, the
 * results are undefined.
 *
 * \param enable New enable state for all callbacks in all domains. Zero
 * disables all callbacks, non-zero enables all callbacks.
 * \param subscriber - Handle of the initialized subscriber
 *
 * \retval SANITIZER_SUCCESS on success
 * \retval SANITIZER_ERROR_NOT_INITIALIZED if unable to initialize the sanitizer
 * \retval SANITIZER_ERROR_INVALID_PARAMETER if \p subscriber is invalid
 */
SanitizerResult SANITIZERAPI sanitizerEnableAllDomains(uint32_t enable,
                                                       Sanitizer_SubscriberHandle subscriber);

/** @} */ /* END SANITIZER_CALLBACK_API */

#if defined(__cplusplus)
}
#endif

#endif /* __SANITIZER_CALLBACKS_H__ */
