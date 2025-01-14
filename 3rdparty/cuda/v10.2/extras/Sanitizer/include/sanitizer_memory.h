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

#if !defined(__SANITIZER_MEMORY_H__)
#define __SANITIZER_MEMORY_H__

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
 * \defgroup SANITIZER_MEMORY_API Sanitizer Memory API
 * Functions, types, and enums that implement the Sanitizer Memory API.
 * @{
 */

/**
 * \addtogroup SANITIZER_MEMORY_API
 * @{
 */

/**
 * \brief Data passed to extended versions of Sanitizer memory APIs
 */
typedef struct {
    /**
     * CUDA context in which to perform the memcpy/memset.
     * If ctx is NULL, the current context will be used.
     */
    CUcontext ctx;

    /**
     * Set to 1 if the API should use per-thread stream.
     * This field is only used for memcpy/memset APIs.
     */
    uint8_t perThreadStream;
} Sanitizer_MemoryData;

/**
 * \brief Allocate memory on the device
 *
 * Equivalent of cudaMalloc that can be called within a callback function.
 * \note \b Thread-safety: this function is thread safe.
 *
 * \param devPtr Pointer to allocated device memory
 * \param size Allocation size in bytes
 */
SanitizerResult SANITIZERAPI sanitizerAlloc(void** devPtr,
                                            size_t size);

/**
 * \brief Allocate memory on the device
 *
 * Equivalent of cudaMalloc that can be called within a callback function.
 * \note \b Thread-safety: this function is thread safe.
 *
 * \param devPtr Pointer to allocated device memory
 * \param size Allocation size in bytes
 * \param memoryData Extra parameters for the memory allocation
 */
SanitizerResult SANITIZERAPI sanitizerAllocEx(void** devPtr,
                                              size_t size,
                                              Sanitizer_MemoryData* memoryData);

/**
 * \brief Frees memory on the device
 *
 * Equivalent of cudaFree that can be called within a callback function.
 * \note \b Thread-safety: this function is thread safe.
 *
 * \param devPtr Device pointer to memory to free
 */
SanitizerResult SANITIZERAPI sanitizerFree(void* devPtr);

/**
 * \brief Frees memory on the device
 *
 * Equivalent of cudaFree that can be called within a callback function.
 * \note \b Thread-safety: this function is thread safe.
 *
 * \param devPtr Device pointer to memory to free
 * \param memoryData Extra parameters for the memory free
 */
SanitizerResult SANITIZERAPI sanitizerFreeEx(void* devPtr,
                                             Sanitizer_MemoryData* memoryData);

/**
 * \brief Copies data from host to device
 *
 * Equivalent of cudaMemcpyAsync that can be called within a callback function.
 * The function will return once the pageable buffer has been copied to the
 * staging memory for DMA transfer to device memory, but the DMA to final
 * destination may not have completed.
 * \note \b Thread-safety: this function is thread safe.
 *
 * \param dst Destination memory address
 * \param src Source memory address
 * \param count Size in bytes to copy
 * \param stream Stream identifier
 */
SanitizerResult SANITIZERAPI sanitizerMemcpyHostToDeviceAsync(void* dst,
                                                              void* src,
                                                              size_t count,
                                                              CUstream stream);

/**
 * \brief Copies data from host to device
 *
 * Equivalent of cudaMemcpyAsync that can be called within a callback function.
 * The function will return once the pageable buffer has been copied to the
 * staging memory for DMA transfer to device memory, but the DMA to final
 * destination may not have completed.
 * \note \b Thread-safety: this function is thread safe.
 *
 * \param dst Destination memory address
 * \param src Source memory address
 * \param count Size in bytes to copy
 * \param stream Stream identifier
 * \param memoryData Extra parameters for the data copy
 */
SanitizerResult SANITIZERAPI sanitizerMemcpyHostToDeviceAsyncEx(void* dst,
                                                                void* src,
                                                                size_t count,
                                                                CUstream stream,
                                                                Sanitizer_MemoryData* memoryData);

/**
 * \brief Copies data from device to host
 *
 * Equivalent of cudaMemcpy that can be called within a callback function.
 * The function will return once the copy has completed.
 * \note \b Thread-safety: this function is thread safe.
 *
 * \param dst Destination memory address
 * \param src Source memory address
 * \param count Size in bytes to copy
 * \param stream Stream identifier
 */
SanitizerResult SANITIZERAPI sanitizerMemcpyDeviceToHost(void* dst,
                                                         void* src,
                                                         size_t count,
                                                         CUstream stream);

/**
 * \brief Copies data from device to host
 *
 * Equivalent of cudaMemcpy that can be called within a callback function.
 * The function will return once the copy has completed.
 * \note \b Thread-safety: this function is thread safe.
 *
 * \param dst Destination memory address
 * \param src Source memory address
 * \param count Size in bytes to copy
 * \param stream Stream identifier
 * \param memoryData Extra parameters for the data copy
 */
SanitizerResult SANITIZERAPI sanitizerMemcpyDeviceToHostEx(void* dst,
                                                           void* src,
                                                           size_t count,
                                                           CUstream stream,
                                                           Sanitizer_MemoryData* memoryData);

/**
 * \brief Initializes or sets device memory to a value.
 *
 * Equivalent of cudaMemset that can be called within a callback function.
 * \note \b Thread-safety: this function is thread safe.
 *
 * \param devPtr Pointer to device memory
 * \param value value to set for each byte of specified memory
 * \param count Size in bytes to set
 * \param stream Stream identifier
 */
SanitizerResult SANITIZERAPI sanitizerMemset(void* devPtr,
                                             int value,
                                             size_t count,
                                             CUstream stream);

/**
 * \brief Initializes or sets device memory to a value.
 *
 * Equivalent of cudaMemset that can be called within a callback function.
 * \note \b Thread-safety: this function is thread safe.
 *
 * \param devPtr Pointer to device memory
 * \param value value to set for each byte of specified memory
 * \param count Size in bytes to set
 * \param stream Stream identifier
 * \param memoryData Extra parameters for the data memset
 */
SanitizerResult SANITIZERAPI sanitizerMemsetEx(void* devPtr,
                                               int value,
                                               size_t count,
                                               CUstream stream,
                                               Sanitizer_MemoryData* memoryData);

/** @} */ /* END SANITIZER_MEMORY_API */

#if defined(__cplusplus)
}
#endif

#endif /* __SANITIZER_MEMORY_H__ */
