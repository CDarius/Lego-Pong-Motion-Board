#include "motor_control/error.hpp"

/**
 * Gets a string describing an error.
 * @param [in]  err     The error code
 * @return              A string describing the error or *NULL*
 */
const char *pbio_error_str(pbio_error_t err) {
    switch (err) {
    case PBIO_SUCCESS:
        break;
    case PBIO_ERROR_FAILED:
        return "Unknown error";
    case PBIO_ERROR_INVALID_ARG:
        return "Invalid argument";
    case PBIO_ERROR_INVALID_PORT:
        return "Invalid port";
    case PBIO_ERROR_IO:
        return "I/O error";
    case PBIO_ERROR_NO_DEV:
        return "Device not connected";
    case PBIO_ERROR_NOT_IMPLEMENTED:
        return "Not implemented";
    case PBIO_ERROR_NOT_SUPPORTED:
        return "Not supported";
    case PBIO_ERROR_AGAIN:
        return "Try again later";
    case PBIO_ERROR_INVALID_OP:
        return "Invalid operation";
    case PBIO_ERROR_TIMEDOUT:
        return "Timed out";
    case PBIO_ERROR_CANCELED:
        return "Canceled";
    case PBIO_ERROR_TACHO_SEQUENCE:
        return "Tacho sequence error";
    }

    return NULL;
}

void serial_print_error(pbio_error_t err, const char* format, ...) {
    if (err != PBIO_SUCCESS) {
        va_list args;
        va_start(args, format);

        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);

        Serial.print(buffer);
        Serial.print(" -> ");
        Serial.println(pbio_error_str(err));
    }
}
