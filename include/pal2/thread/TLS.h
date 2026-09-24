/**
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

#include "pal2/core/defines.h"

/**
 * @typedef PalTLSId
 * @brief Opaque handle to a Thread Local Storage.
 *
 * @since Added in version 2.0
 */
typedef uint32_t PalTLSId;

/**
 * @typedef PaTlsDestructorFn
 * @brief Function pointer type used for TLS.
 * 
 * The function signature should look like this:
 * @code
 * void PAL_CALL tlsDestructor(void* userData);
 * @endcode
 *
 * This is called when the TLS is destroyed and its value is not `nullptr`.
 *
 * @param[in] userData User data passed from `palCreateTLS()`.
 * Can be `nullptr`.
 *
 * @since Added in version 2.0
 */
typedef void (PAL_CALL* PaTlsDestructorFn)(void* userData);

/**
 * @brief Creates a new TLS.
 *
 * The TLS handle can be used by multiple threads to associate thread local
 * vaules. The destructor will be called if `palDestroyTLS()` is called and
 * the TLS has a valid value.
 *
 * @param[in] destructor The TLS destructor callback. Can be `nullptr`.
 *
 * @return The created TLS id on success or `0` on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palDestroyTLS
 */
PAL_API PalTLSId PAL_CALL palCreateTLS(PaTlsDestructorFn destructor);

/**
 * @brief Destroys the provided TLS.
 *
 * @param[in] Tls The TLS.
 *
 * @Thread-safety `Tls` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palCreateTLS
 */
PAL_API void PAL_CALL palDestroyTLS(PalTLSId Tls);

/**
 * @brief Gets the value associated with the TLS on the calling thread.
 *
 * @param[in] Tls The TLS.
 *
 * @return the value on success or `nullptr` on failure.
 *
 * @Thread-safety Thread safe.
 *
 * @since Added in version 2.0
 * @sa palSetTLS
 */
PAL_API void* PAL_CALL palGetTLS(PalTLSId Tls);

/**
 * @brief Sets a value associated with the TLS on the calling thread.
 *
 * @param[in] Tls The TLS.
 * @param[in] data The value to set for the calling thread.
 *
 * @Thread-safety `Tls` must be externally synchronized.
 *
 * @since Added in version 2.0
 * @sa palGetTLS
 */
PAL_API void PAL_CALL palSetTLS(
    PalTLSId Tls,
    void* data);

#endif // PAL_THREAD_TLS_H