////
//// Created by Jeremy Cote on 2024-07-21.
////
//
//#ifndef UOSM_TELEMETRY_PICO_MQTT_PAL_H
//#define UOSM_TELEMETRY_PICO_MQTT_PAL_H
//
//#if defined(__cplusplus)
//extern "C" {
//#endif
//
//#include <stddef.h>
//#include <stdint.h>
//#include <stdarg.h>
//#include <string.h>
//#include <time.h>
//#include <FreeRTOS.h>
//#include <semphr.h>
//
//#define INT_MIN INT32_MIN
//
//#define MQTT_PAL_HTONS(s) ((s << 8) | (s >> 8))
//#define MQTT_PAL_NTOHS(s) ((s << 8) | (s >> 8))
//
//#define MQTT_PAL_TIME() time(NULL)
//
//typedef time_t mqtt_pal_time_t;
//
//typedef SemaphoreHandle_t mqtt_pal_mutex_t;
//typedef StaticSemaphore_t mqtt_pal_mutex_buffer_t;
//
//#define MQTT_PAL_MUTEX_INIT(mtx_ptr) xSemaphoreCreateMutexStatic(mtx_ptr)
//#define MQTT_PAL_MUTEX_LOCK(mtx_ptr) xSemaphoreTake(*mtx_ptr, portMAX_DELAY)
//#define MQTT_PAL_MUTEX_UNLOCK(mtx_ptr) xSemaphoreGive(*mtx_ptr)
//
//struct mbedtls_ssl_context;
//typedef struct mbedtls_ssl_context *mqtt_pal_socket_handle;
//
///**
// * @brief Sends all the bytes in a buffer.
// * @ingroup pal
// *
// * @param[in] fd The file-descriptor (or handle) of the socket.
// * @param[in] buf A pointer to the first byte in the buffer to send.
// * @param[in] len The number of bytes to send (starting at \p buf).
// * @param[in] flags Flags which are passed to the underlying socket.
// *
// * @returns The number of bytes sent if successful, an \ref MQTTErrors otherwise.
// *
// * Note about the error handling:
// * - On an error, if some bytes have been processed already,
// *   this function should return the number of bytes successfully
// *   processed. (partial success)
// * - Otherwise, if the error is an equivalent of EAGAIN, return 0.
// * - Otherwise, return MQTT_ERROR_SOCKET_ERROR.
// */
//ssize_t mqtt_pal_sendall(mqtt_pal_socket_handle fd, const void* buf, size_t len, int flags);
//
///**
// * @brief Non-blocking receive all the byte available.
// * @ingroup pal
// *
// * @param[in] fd The file-descriptor (or handle) of the socket.
// * @param[in] buf A pointer to the receive buffer.
// * @param[in] bufsz The max number of bytes that can be put into \p buf.
// * @param[in] flags Flags which are passed to the underlying socket.
// *
// * @returns The number of bytes received if successful, an \ref MQTTErrors otherwise.
// *
// * Note about the error handling:
// * - On an error, if some bytes have been processed already,
// *   this function should return the number of bytes successfully
// *   processed. (partial success)
// * - Otherwise, if the error is an equivalent of EAGAIN, return 0.
// * - Otherwise, return MQTT_ERROR_SOCKET_ERROR.
// */
//ssize_t mqtt_pal_recvall(mqtt_pal_socket_handle fd, void* buf, size_t bufsz, int flags);
//
//#if defined(__cplusplus)
//}
//#endif
//
//
//#endif //UOSM_TELEMETRY_PICO_MQTT_PAL_H
