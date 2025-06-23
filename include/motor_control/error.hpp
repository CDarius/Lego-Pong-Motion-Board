// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2020 The Pybricks Authors

#ifndef _PBIO_ERROR_H_
#define _PBIO_ERROR_H_

#include <Arduino.h>

/**
 * Error code.
 */
typedef enum {
    PBIO_SUCCESS,               /**< No error */
    PBIO_ERROR_FAILED,          /**< Unspecified error (used when no other error code fits) */
    PBIO_ERROR_INVALID_ARG,     /**< Invalid argument (other than port) */
    PBIO_ERROR_INVALID_PORT,    /**< Invalid port identifier (special case of ::PBIO_ERROR_INVALID_ARG) */
    PBIO_ERROR_IO,              /**< General I/O error */
    PBIO_ERROR_NO_DEV,          /**< Device is not connected */
    PBIO_ERROR_NOT_IMPLEMENTED, /**< Feature is not yet implemented */
    PBIO_ERROR_NOT_SUPPORTED,   /**< Feature is not supported on this device */
    PBIO_ERROR_AGAIN,           /**< Function should be called again later */
    PBIO_ERROR_INVALID_OP,      /**< Operation is not permitted in the current state */
    PBIO_ERROR_TIMEDOUT,        /**< The operation has timed out */
    PBIO_ERROR_CANCELED,        /**< The operation was canceled */
    PBIO_ERROR_TACHO_SEQUENCE,  /**< Encoder sequence counting error */
    PBIO_ERROR_HOME_SWITCH_ERR  /**< Home switch sequence error */
} pbio_error_t;

const char *pbio_error_str(pbio_error_t err);

#define PBIO_RETURN_ON_ERROR(expr)         \
    do {                                   \
        pbio_error_t _err = (expr);        \
        if (_err != PBIO_SUCCESS) {        \
            return _err;                   \
        }                                  \
    } while (0)
    
#endif // _PBIO_ERROR_H_
