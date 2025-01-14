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

#if !defined(__SANITIZER_PATCHING_H__)
#define __SANITIZER_PATCHING_H__

#include <sanitizer_memory.h>
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
 * \defgroup SANITIZER_PATCHING_API Sanitizer Patching API
 * Functions, types, and enums that implement the Sanitizer Patching API.
 * @{
 */

/**
 * \addtogroup SANITIZER_PATCHING_API
 * @{
 */

/**
 * \brief Load a module containing patches that can be used by the
 * patching API.
 *
 * \note \b Thread-safety: an API user must serialize access to
 * sanitizerAddPatchesFromFile, sanitizerAddPatches, sanitizerPatchInstructions,
 * and sanitizerPatchModule. For example if sanitizerAddPatchesFromFile(filename)
 * and sanitizerPatchInstruction(*, *, cbName) are called concurrently and
 * cbName is intended to be found in the loaded module, the results are
 * undefined.
 *
 * \note The patches loaded are only valid for the specified CUDA context.
 *
 * \param filename Path to the module file. This API supports the same module
 * formats as the cuModuleLoad function from the CUDA driver API.
 * \param ctx CUDA context in which to load the patches. If ctx is NULL, the
 * current context will be used.
 *
 * \retval SANITIZER_SUCCESS on success
 * \retval SANITIZER_ERROR_NOT_INITIALIZED if unable to initialize the sanitizer
 * \retval SANITIZER_ERROR_INVALID_PARAMETER if \p filename is not a path to
 * a valid CUDA module.
 */
SanitizerResult SANITIZERAPI sanitizerAddPatchesFromFile(const char* filename,
                                                         CUcontext ctx);

/**
 * \brief Load a module containing patches that can be used by the
 * patching API.
 *
 * \note \b Thread-safety: an API user must serialize access to
 * sanitizerAddPatchesFromFile, sanitizerAddPatches, sanitizerPatchInstructions,
 * and sanitizerPatchModule. For example if sanitizerAddPatches(image) and
 * sanitizerPatchInstruction(*, *, cbName) are called concurrently and cbName
 * is intended to be found in the loaded image, the results are undefined.
 *
 * \note The patches loaded are only valid for the specified CUDA context.
 *
 * \param image Pointer to module data to load. This API supports the same
 * module formats as the cuModuleLoadData and cuModuleLoadFatBinary functions
 * from the CUDA driver API.
 * \param ctx CUDA context in which to load the patches. If ctx is NULL, the
 * current context will be used.
 *
 * \retval SANITIZER_SUCCESS on success
 * \retval SANITIZER_ERROR_NOT_INITIALIZED if unable to initialize the sanitizer
 * \retval SANITIZER_ERROR_INVALID_PARAMETER if \p image does not point to a
 * valid CUDA module.
 */
SanitizerResult SANITIZERAPI sanitizerAddPatches(const void* image,
                                                 CUcontext ctx);

/**
 * \brief Sanitizer patch result codes
 *
 * Error and result codes returned by Sanitizer patches.
 * If a patch returns with an error code different than
 * SANITIZER_PATCH_SUCCESS, the thread will be exited.
 */
typedef enum {
    /**
     * No error.
     */
    SANITIZER_PATCH_SUCCESS                 = 0,

    /**
     * An invalid memory access was performed.
     */
    SANITIZER_PATCH_INVALID_MEMORY_ACCESS   = 1,

    /**
     * An invalid synchronization operation was performed.
     */
    SANITIZER_PATCH_INVALID_SYNCHRONIZATION = 2,

    SANITIZER_PATCH_FORCE_INT               = 0x7fffffff
} SanitizerPatchResult;

/**
 * \brief Function type for a CUDA block enter callback.
 *
 * \p userdata is a pointer to user data. See \ref sanitizerPatchModule
 * \p pc is the program counter of the entry point of the block
 */
typedef SanitizerPatchResult (SANITIZERAPI *SanitizerCallbackBlockEnter)(void* userdata, uint64_t pc);

/**
 * \brief Function type for a CUDA block exit callback.
 *
 * \p userdata is a pointer to user data. See \ref sanitizerPatchModule
 * \p pc is the program counter of the patched instruction
 */
typedef SanitizerPatchResult (SANITIZERAPI *SanitizerCallbackBlockExit)(void* userdata, uint64_t pc);

/**
 * \brief Flags describing a memory access
 *
 * Flags describing a memory access. These values are to be used in order to
 * interpret the value of \b flags for a SanitizerCallbackMemoryAccess callback.
 *
 * If neither LOCAL or SHARED flag is set, the access is made to global memory.
 */
typedef enum {
    /**
     * Empty flag.
     */
    SANITIZER_MEMORY_DEVICE_FLAG_NONE      = 0,

    /**
     * Specifies that the access is a read.
     */
    SANITIZER_MEMORY_DEVICE_FLAG_READ      = 0x1,

    /**
     * Specifies that the access is a write.
     */
    SANITIZER_MEMORY_DEVICE_FLAG_WRITE     = 0x2,

    /**
     * Specifies that the access is a system-scoped atomic.
     */
    SANITIZER_MEMORY_DEVICE_FLAG_ATOMSYS   = 0x4,

    /**
     * Specifies that the memory accessed is local memory.
     */
    SANITIZER_MEMORY_DEVICE_FLAG_LOCAL     = 0x8,

    /**
     * Specifies that the memory accessed is shared memory.
     */
    SANITIZER_MEMORY_DEVICE_FLAG_SHARED    = 0x10,

    SANITIZER_MEMORY_DEVICE_FLAG_FORCE_INT = 0x7fffffff
} Sanitizer_DeviceMemoryFlags;

/**
 * \brief Function type for a memory access callback.
 *
 * \p userdata is a pointer to user data. See \ref sanitizerPatchModule
 * \p pc is the program counter of the patched instruction
 * \p ptr is the address of the memory being accessed
 * \p accessSize is the size of the access in bytes. Valid values are 1, 2, 4,
 * 8, and 16.
 * \p flags contains information about the type of access. See
 * Sanitizer_DeviceMemoryFlags to interpret this value.
 * \p newValue is a pointer to the new value being written if the acces is a
 * write. If the access is a read or an atomic, the pointer will be NULL.
 */
typedef SanitizerPatchResult (SANITIZERAPI *SanitizerCallbackMemoryAccess)(void* userdata, uint64_t pc, void* ptr, uint32_t accessSize, uint32_t flags, const void* newValue);

/**
 * \brief Function type for a barrier callback.
 *
 * \p userdata is a pointer to user data. See \ref sanitizerPatchModule
 * \p pc is the program counter of the patched instruction
 * \p barIndex is the barrier index.
 * \p threadCount is the number of expected threads (must be a multiple of the warp size).
 * 0 means that all threads are participating in the barrier.
 */
typedef SanitizerPatchResult (SANITIZERAPI *SanitizerCallbackBarrier)(void* userdata, uint64_t pc, uint32_t barIndex, uint32_t threadCount);

/**
 * \brief Function type for a syncwarp callback.
 *
 * \p userdata is a pointer to user data. See \ref sanitizerPatchModule
 * \p pc is the program counter of the patched instruction
 * \p mask is the thread mask passed to __syncwarp().
 */
typedef SanitizerPatchResult (SANITIZERAPI *SanitizerCallbackSyncwarp)(void* userdata, uint64_t pc, uint32_t mask);

/**
 * \brief Function type for a shfl callback.
 *
 * \p userdata is a pointer to user data. See \ref sanitizerPatchModule
 * \p pc is the program counter of the patched instruction
 *
 */
typedef SanitizerPatchResult (SANITIZERAPI *SanitizerCallbackShfl)(void* userdata, uint64_t pc);

/**
 * \brief Function type for a function call callback.
 *
 * \p userdata is a pointer to user data. See \ref sanitizerPatchModule
 * \p pc is the program counter of the patched instruction
 * \p targetPc is the PC where the called function is located.
 */
typedef SanitizerPatchResult (SANITIZERAPI *SanitizerCallbackCall)(void* userdata, uint64_t pc, uint64_t targetPc);

/**
 * \brief Function type for a function return callback.
 *
 * \p userdata is a pointer to user data. See \ref sanitizerPatchModule
 * \p pc is the program counter of the patched instruction
 *
 */
typedef SanitizerPatchResult (SANITIZERAPI *SanitizerCallbackRet)(void* userdata, uint64_t pc);

/**
 * \brief Function type for a device-side malloc call.
 *
 * \note This is called after the call has completed.
 *
 * \p userdata is a pointer to user data. See \ref sanitizerPatchModule
 * \p pc is the program counter of the patched instruction
 * \p allocatedPtr is the pointer returned by device-side malloc
 * \p allocatedSize is the size requested by the user to device-side malloc.
 */
typedef SanitizerPatchResult (SANITIZERAPI *SanitizerCallbackDeviceSideMalloc)(void* userdata, uint64_t pc, void* allocatedPtr, uint64_t allocatedSize);

/**
 * \brief Function type for a device-side free call.
 *
 * \note This is called prior to the actual call.
 *
 * \p userdata is a pointer to user data. See \ref sanitizerPatchModule
 * \p pc is the program counter of the patched instruction
 * \p ptr is the pointer passed to device-side free.
 */
typedef SanitizerPatchResult (SANITIZERAPI *SanitizerCallbackDeviceSideFree)(void* userdata, uint64_t pc, void* ptr);

/**
 * \brief Instrumentation.
 *
 * Instrumentation. Every entry represent an instruction type or a function
 * call where a callback patch can be inserted.
 */
typedef enum {
    /**
     * Invalid instruction ID.
     */
    SANITIZER_INSTRUCTION_INVALID            = 0,

    /**
     * CUDA block enter. This is called prior to any user code. The type of the
     * callback must be SanitizerCallbackBlockEnter.
     */
    SANITIZER_INSTRUCTION_BLOCK_ENTER        = 1,

    /**
     * CUDA block exit. This is called after all user code has executed. The type of
     * the callback must be SanitizerCallbackBlockExit.
     */
    SANITIZER_INSTRUCTION_BLOCK_EXIT         = 2,

    /**
     * Memory Access. This can be a store, load or atomic operation. The type
     * of the callback must be SanitizerCallbackMemoryAccess.
     */
    SANITIZER_INSTRUCTION_MEMORY_ACCESS      = 3,

    /**
     * Barrier. The type of the callback must be SanitizerCallbackBarrier.
     */
    SANITIZER_INSTRUCTION_BARRIER            = 4,

    /**
     * Syncwarp. The type of the callback must be SanitizerCallbackSyncwarp.
     */
    SANITIZER_INSTRUCTION_SYNCWARP           = 5,

    /**
     * Shfl. The type of the callback must be SanitizerCallbackShfl.
     */
    SANITIZER_INSTRUCTION_SHFL               = 6,

    /**
     * Function call. The type of the callback must be SanitizerCallbackCall.
     */
    SANITIZER_INSTRUCTION_CALL               = 7,

    /**
     * Function return. The type of the callback must be SanitizerCallbackRet.
     */
    SANITIZER_INSTRUCTION_RET                = 8,

    /**
     * Device-side malloc. The type of the callback must be
     * SanitizerCallbackDeviceSideMalloc.
     */
    SANITIZER_INSTRUCTION_DEVICE_SIDE_MALLOC = 9,

    /**
     * Device-side free. The type of the callback must be
     * SanitizerCallbackDeviceSideFree.
     */
    SANITIZER_INSTRUCTION_DEVICE_SIDE_FREE   = 10,

    SANITIZER_INSTRUCTION_FORCE_INT          = 0x7fffffff
} Sanitizer_InstructionId;

/**
 * \brief Set instrumentation points and patches to be applied in a module.
 *
 * Mark that all instrumentation points matching instructionId are to be
 * patched in order to call the device function identified by
 * deviceCallbackName. It is up to the API client to ensure that this
 * device callback exists and match the correct callback format for
 * this instrumentation point.
 * \note \b Thread-safety: an API user must serialize access to
 * sanitizerAddPatchesFromFile, sanitizerAddPatches, sanitizerPatchInstructions,
 * and sanitizerPatchModule. For example if sanitizerAddPatches(fileName) and
 * sanitizerPatchInstruction(*, *, cbName) are called concurrently and cbName
 * is intended to be found in the loaded module, the results are undefined.
 *
 * \param instructionId Instrumentation point for which to insert patches
 * \param module CUDA module to instrument
 * \param deviceCallbackName Name of the device function callback that the
 * inserted patch will call at the instrumented points. This function is
 * expected to be found in code previously loaded by sanitizerAddPatchesFromFile
 * or sanitizerAddPatches.
 *
 * \retval SANITIZER_SUCCESS on success
 * \retval SANITIZER_ERROR_NOT_INITIALIZED if unable to initialize the sanitizer
 * \retval SANITIZER_ERROR_INVALID_PARAMETER if \p module is not a CUDA module
 * or if \p deviceCallbackName function cannot be located.
 */
SanitizerResult SANITIZERAPI sanitizerPatchInstructions(const Sanitizer_InstructionId instructionId,
                                                        CUmodule module,
                                                        const char* deviceCallbackName);

/**
 *
 * \brief Perform the actual instrumentation of a module.
 *
 * Perform the instrumentation of a CUDA module based on previous calls to
 * sanitizerPatchInstructions. This function also specifies the device memory
 * buffer to be passed in as userdata to all callback functions.
 * \note \b Thread-safety: an API user must serialize access to
 * sanitizerAddPatchesFromFile, sanitizerAddPatches, sanitizerPatchInstructions,
 * and sanitizerPatchModule. For example if sanitizerPatchModule(mod, *) and
 * sanitizerPatchInstruction(*, mod, *) are called concurrently, the results
 * are undefined.
 *
 * \param module CUDA module to instrument
 *
 * \retval SANITIZER_SUCCESS on success
 * \retval SANITIZER_ERROR_INVALID_PARAMETER if \p module is not a CUDA module
 */
SanitizerResult SANITIZERAPI sanitizerPatchModule(CUmodule module);

/**
 * \brief Specifies the user data pointer for callbacks
 *
 * Mark all subsequent launches on \p stream to use \p userdata
 * pointer as the device memory buffer to pass in to callback functions.
 * \note \b Thread-safety: an API user must serialize access to
 * sanitizerSetCallbackData and kernel launches on the same stream.
 *
 * \param stream CUDA stream to link to user data. Callbacks in subsequent
 * launches on this stream will use \p userdata as callback data. Can be 0
 * to specify the NULL stream.
 * \param userdata Device memory buffer. This data will be passed to callback
 * functions via the \p userdata parameter.
 *
 * \retval SANITIZER_SUCCESS on success
 * \retval SANITIZER_ERROR_INVALID_PARAMETER if \p stream is not a CUDA stream
 */
SanitizerResult SANITIZERAPI sanitizerSetCallbackData(CUstream stream,
                                                      const void* userdata);

/**
 * \brief Specifies the user data pointer for callbacks
 *
 * Mark all subsequent launches on \p stream to use \p userdata
 * pointer as the device memory buffer to pass in to callback functions.
 * \note \b Thread-safety: an API user must serialize access to
 * sanitizerSetCallbackData and kernel launches on the same stream.
 *
 * \param stream CUDA stream to link to user data. Callbacks in subsequent
 * launches on this stream will use \p userdata as callback data. Can be 0
 * to specify the NULL stream.
 * \param userdata Device memory buffer. This data will be passed to callback
 * functions via the \p userdata parameter.
 * \param memoryData Extra parameters for the callback data setting
 *
 * \retval SANITIZER_SUCCESS on success
 * \retval SANITIZER_ERROR_INVALID_PARAMETER if \p stream is not a CUDA stream
 */
SanitizerResult SANITIZERAPI sanitizerSetCallbackDataEx(CUstream stream,
                                                        const void* userdata,
                                                        Sanitizer_MemoryData* memoryData);

/**
 *
 * \brief Remove existing instrumentation of a module
 *
 * Remove any instrumentation of a CUDA module performed by previous calls
 * to sanitizerPatchModule.
 * \note \b Thread-safety: an API user must serialize access to
 * sanitizerPatchModule and sanitizerUnpatchModule on the same module.
 * For example, if sanitizerPatchModule(mod) and sanitizerUnpatchModule(mod)
 * are called concurrently, the results are undefined.
 *
 * \param module CUDA module on which to remove instrumentation
 *
 * \retval SANITIZER_SUCCESS on success
 */
SanitizerResult SANITIZERAPI sanitizerUnpatchModule(CUmodule module);

/**
 *
 * \brief Get PC and size of a CUDA function
 *
 * \param[in] module CUDA module containing the function
 * \param[in] deviceCallbackName CUDA function name
 * \param[out] pc Function start program counter (PC) returned
 * \param[out] size Function size in bytes returned
 *
 * \retval SANITIZER_SUCCESS on success
 * \retval SANITIZER_ERROR_INVALID_PARAMETER if \p functionName function
 * cannot be located, if pc is NULL or if size is NULL.
 *
 */
SanitizerResult SANITIZERAPI sanitizerGetFunctionPcAndSize(CUmodule module,
                                                           const char* functionName,
                                                           uint64_t* pc,
                                                           uint64_t* size);

/**
 *
 * \brief Get PC and size of a device callback
 *
 * \param[in] ctx CUDA context in which the patches were loaded.
 * If ctx is NULL, the current context will be used.
 * \param[in] deviceCallbackName device function callback name
 * \param[out] pc Callback PC returned
 * \param[out] size Callback size returned
 *
 * \retval SANITIZER_SUCCESS on success
 * \retval SANITIZER_ERROR_INVALID_PARAMETER if \p deviceCallbackName function
 * cannot be located, if pc is NULL or if size is NULL.
 *
 */
SanitizerResult SANITIZERAPI sanitizerGetCallbackPcAndSize(CUcontext ctx,
                                                           const char* deviceCallbackName,
                                                           uint64_t* pc,
                                                           uint64_t* size);


/** @} */ /* END SANITIZER_PATCHING_API */

#if defined(__cplusplus)
}
#endif

#endif /* __SANITIZER_PATCHING_H__ */
