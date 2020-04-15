/*
 * Super not secure flash veneers
 */

#include <string.h>

#include "tfm_flash_veneers.h"
#include "Driver_Flash.h"
#include "flash_layout.h"

#include "tfm_secure_api.h"
#include "tfm_api.h"
#include "spm_partition_defs.h"
#include "platform_base_address.h"
#include "uart_stdout.h"

extern ARM_DRIVER_FLASH FLASH_DEV_NAME;

__tfm_secure_gateway_attributes__
uint8_t tfm_flash_init() {
    return FLASH_DEV_NAME.Initialize(NULL) != ARM_DRIVER_OK;
}

__tfm_secure_gateway_attributes__
uint8_t tfm_flash_is_busy() {
    return FLASH_DEV_NAME.GetStatus().busy;
}

__tfm_secure_gateway_attributes__
uint8_t tfm_flash_is_error() {
    return FLASH_DEV_NAME.GetStatus().error;
}

__tfm_secure_gateway_attributes__
uint8_t tfm_flash_read(uint32_t addr, uint32_t *buf, uint32_t len) {
    return FLASH_DEV_NAME.ReadData(addr, buf, len);
}

#define SECTOR_SIZE 0x1000
static uint8_t sector_buffer[SECTOR_SIZE];

int32_t _write_single_sector(uint32_t addr, uint32_t *buf, uint32_t len) {
    uint32_t sector_offset = addr % SECTOR_SIZE;
    uint32_t sector_addr = addr - sector_offset;

    uint32_t bytes_to_write = SECTOR_SIZE - sector_offset;
    if (len < bytes_to_write) bytes_to_write = len;

    // Read previous sector contents
    int rc = FLASH_DEV_NAME.ReadData(sector_addr, &sector_buffer, SECTOR_SIZE);
    if (rc != 0) {
        return -rc;
    }
    while(FLASH_DEV_NAME.GetStatus().busy);

    memcpy(&sector_buffer[sector_offset], buf, bytes_to_write);

    rc = FLASH_DEV_NAME.EraseSector(sector_addr);
    if (rc != 0) {
        return -rc;
    }
    while(FLASH_DEV_NAME.GetStatus().busy);

    rc = FLASH_DEV_NAME.ProgramData(sector_addr, sector_buffer, SECTOR_SIZE); 
    if (rc != 0) {
        return -rc;
    }
    while(FLASH_DEV_NAME.GetStatus().busy);

    return bytes_to_write;
}

__tfm_secure_gateway_attributes__
uint8_t tfm_flash_write(uint32_t addr, uint32_t *buf, uint32_t len) {

    while (len != 0) {
        int32_t written = _write_single_sector(addr, buf, len);
        if (written < 0) { // return error code
            return (uint8_t) -written;
        }

        addr += written;
        buf += written / sizeof(uint32_t);
        len -= written;
    }

    return 0;
    /*
    uint32_t sector_offset = addr % 0x1000;
    uint32_t sector_addr = addr - sector_offset;

    // Read previous sector contents
    int rc = FLASH_DEV_NAME.ReadData(sector_addr, &sector_buffer, 0x1000);
    if (rc != 0) {
        return rc;
    }
    while(FLASH_DEV_NAME.GetStatus().busy);

    / *
    printf("Sector (%x) contents before modification:\n", sector_addr);
    for (int i = 0; i < 0x1000; i++) {
        printf("%x ", sector_buffer[i]);
    }
    printf("\n\n");
    * /

    memcpy(&sector_buffer[sector_offset], buf, len);

    / *
    printf("Sector (%x) contents after modification of %d bytes at sector offset %x:\n", sector_addr, len, sector_offset);
    for (int i = 0; i < 0x1000; i++) {
        printf("%x ", sector_buffer[i]);
    }
    printf("\n\n");
    * /

    rc = FLASH_DEV_NAME.EraseSector(sector_addr);
    if (rc != 0) {
        return rc;
    }
    while(FLASH_DEV_NAME.GetStatus().busy);

    rc = FLASH_DEV_NAME.ProgramData(sector_addr, sector_buffer, 0x1000); 
    while(FLASH_DEV_NAME.GetStatus().busy);

    FLASH_DEV_NAME.ReadData(sector_addr, &sector_buffer, 0x1000);
    while(FLASH_DEV_NAME.GetStatus().busy);

    / *
    printf("Sector (%x) contents after program:\n", sector_addr);
    for (int i = 0; i < 0x1000; i++) {
        printf("%x ", sector_buffer[i]);
    }
    printf("\n\n");
    * /
    */
}

