
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

/**
 * @defgroup thread TLS section
 * @ingroup pal_thread
 * @{
 */

#ifndef _THREAD_TLS_H
#define _THREAD_TLS_H

#include "pal/core/defines.h"

/**
 * @typedef PalTLSId
 * @brief Opaque handle to a Thread Local Storage.
 *
 * @since 1.0
 */
typedef uint32_t PalTLSId;

/**
 * @typedef PaTlsDestructorFn
 * @brief Function pointer type used for TLS.
 *
 * This is called when the TLS is destroyed and its value is not nullptr.
 *
 * @param[in] userData Optional pointer to user data. Can be nullptr.
 *
 * @since 1.0
 */
typedef void (*PaTlsDestructorFn)(void* userData);

/**
 * @brief Create a new TLS.
 *
 * The TLS handle can be used by multiple threads to associate thread local
 * vaules. The destructor will be called if palDestroyTLS() is called and
 * the TLS has a valid value.
 *
 * @param[in] destructor Pointer to the TLS destructor function. Can be
 * nullptr.
 *
 * @return The TLS on success or 0 on failure.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palDestroyTLS
 */
PAL_API PalTLSId PAL_CALL palCreateTLS(PaTlsDestructorFn destructor);

/**
 * @brief Destroy the provided TLS.
 *
 * @param[in] id The TLS to destroy.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palCreateTL
 */
PAL_API void PAL_CALL palDestroyTLS(PalTLSId id);

/**
 * @brief Get the value associated with the provided TLS on the calling thread.
 *
 * @param[in] id The TLS to query value.
 *
 * @return the value on success or nullptr on failure.
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palSetTLS
 */
PAL_API void* PAL_CALL palGetTLS(PalTLSId id);

/**
 * @brief Set a value associated with the provided TLS on the calling thread.
 *
 * @param[in] id The TLS to set value to.
 * @param[in] data The value to set for the calling thread
 *
 * Thread safety: Thread safe.
 *
 * @since 1.0
 * @sa palGetTLS
 */
PAL_API void PAL_CALL palSetTLS(
    PalTLSId id,
    void* data);

#endif // _THREAD_TLS_H

/** @} */