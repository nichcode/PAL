
/**
 * @file tls.h
 * 
 * Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef PAL_THREAD_TLS_H
#define PAL_THREAD_TLS_H

#include "pal2/core/base.h"

/**
 * @typedef PalTLSId
 * @brief Opaque handle to a thread-local storage (TLS) slot.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 */
typedef uint32_t PalTLSId;

/**
 * @typedef PaTlsDestructorFn
 * @brief Thread-local storage destructor function.
 *
 * The function is called when the TLS is destroyed and the value associated
 * with it is not `nullptr`.
 *
 * `userData` is the value set with @ref palSetTLS() and is owned by the application;
 * PAL does not free it.
 *
 * The function signature should look like this:
 * @code
 * void PAL_CALL tlsDestructor(void* userData);
 * @endcode
 *
 * @param[in] userData Value associated with the TLS.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 */
typedef void (PAL_CALL* PaTlsDestructorFn)(void* userData);

/**
 * @brief Creates a TLS.
 *
 * The created TLS must be destroyed using @ref palDestroyTLS().
 *
 * The TLS handle can be shared by multiple threads, each of which has its own
 * value associated with it. The destructor is called when @ref palDestroyTLS()
 * is called and the TLS has a non-`nullptr` value associated with it.
 *
 * @param[in] destructor TLS destructor function, or `nullptr` for no destructor.
 * @return The created TLS on success or `0` on failure.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 *
 * @sa palDestroyTLS
 */
PAL_API PalTLSId PAL_CALL palCreateTLS(PaTlsDestructorFn destructor);

/**
 * @brief Destroys the TLS.
 *
 * The destructor passed to @ref palCreateTLS() is called if the TLS has a non-`nullptr`
 * value associated with it.
 *
 * @param[in] Tls TLS to destroy.
 *
 * @thread-safety `Tls` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 *
 * @sa palCreateTLS
 */
PAL_API void PAL_CALL palDestroyTLS(PalTLSId Tls);

/**
 * @brief Gets the value associated with the TLS on the calling thread.
 *
 * @param[in] Tls TLS to get the value of.
 * @return Pointer to the value on success or `nullptr` if no value was set or
 *         on failure.
 *
 * @thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 *
 * @sa palSetTLS
 */
PAL_API void* PAL_CALL palGetTLS(PalTLSId Tls);

/**
 * @brief Sets the TLS value on the calling thread.
 *
 * @param[in] Tls TLS to set the value of.
 * @param[in] data Pointer to the value to set, or `nullptr` for no value.
 *
 * @thread-safety `Tls` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @ingroup pal_thread
 *
 * @sa palGetTLS
 */
PAL_API void PAL_CALL palSetTLS(
    PalTLSId Tls,
    void* data);

#endif // PAL_THREAD_TLS_H