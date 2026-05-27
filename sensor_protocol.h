///**
// * @file sensor_protocol.h
// * @brief Common protocol definition for sensor data transmission
// *
// * This header can be used by both ESP/Arduino and Linux applications
// * to ensure consistent data structure.
// */
//
//#ifndef SENSOR_PROTOCOL_H
//#define SENSOR_PROTOCOL_H
//
//#include <stdint.h>
//
//// Magic bytes for packet validation
//#define PACKET_MAGIC_START 0xAA55
//#define PACKET_MAGIC_END   0x55AA
//
///**
// * @brief Sensor data packet structure
// *
// * This structure will be sent as binary data over UART.
// * Total size: 32 bytes
// */
//typedef struct __attribute__((packed)) {
//    uint16_t magic_start;      // Magic bytes for validation (0xAA55)
//    char device_id[20];        // Device identifier (null-terminated string)
//    float temperature;         // Temperature in Celsius
//    float humidity;            // Humidity in percentage
//    uint16_t checksum;         // Simple checksum for validation
//    uint16_t magic_end;        // End magic bytes (0x55AA)
//} SensorPacket;
//
///**
// * @brief Calculate checksum for sensor packet
// *
// * Simple XOR checksum of all bytes except the checksum field itself
// */
//static inline uint16_t calculate_checksum(const SensorPacket *packet) {
//    uint16_t checksum = 0;
//    const uint8_t *data = (const uint8_t *)packet;
//
//    // XOR all bytes except checksum field
//    for (size_t i = 0; i < sizeof(SensorPacket) - 4; i++) { // -4 to exclude checksum and magic_end
//        checksum ^= data[i];
//    }
//
//    return checksum;
//}
//
///**
// * @brief Validate sensor packet
// *
// * @return 1 if valid, 0 if invalid
// */
//static inline int validate_packet(const SensorPacket *packet) {
//    if (packet->magic_start != PACKET_MAGIC_START) return 0;
//    if (packet->magic_end != PACKET_MAGIC_END) return 0;
//    if (packet->checksum != calculate_checksum(packet)) return 0;
//    return 1;
//}
//
//#endif // SENSOR_PROTOCOL_H
