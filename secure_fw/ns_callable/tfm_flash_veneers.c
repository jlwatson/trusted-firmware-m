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

static uint8_t sector_buffer[0x1000];
__tfm_secure_gateway_attributes__
uint8_t tfm_flash_write(uint32_t addr, uint32_t *buf, uint32_t len) {

    uint32_t sector_offset = addr % 0x1000;
    uint32_t sector_addr = addr - sector_offset;

    // Read previous sector contents
    int rc = FLASH_DEV_NAME.ReadData(sector_addr, &sector_buffer, 0x1000);
    if (rc != 0) {
        return rc;
    }
    while(FLASH_DEV_NAME.GetStatus().busy);

    /*
    printf("Sector (%x) contents before modification:\n", sector_addr);
    for (int i = 0; i < 0x1000; i++) {
        printf("%x ", sector_buffer[i]);
    }
    printf("\n\n");
    */

    memcpy(&sector_buffer[sector_offset], buf, len);

    /*
    printf("Sector (%x) contents after modification of %d bytes at sector offset %x:\n", sector_addr, len, sector_offset);
    for (int i = 0; i < 0x1000; i++) {
        printf("%x ", sector_buffer[i]);
    }
    printf("\n\n");
    */

    rc = FLASH_DEV_NAME.EraseSector(sector_addr);
    if (rc != 0) {
        return rc;
    }
    while(FLASH_DEV_NAME.GetStatus().busy);

    rc = FLASH_DEV_NAME.ProgramData(sector_addr, sector_buffer, 0x1000); 
    while(FLASH_DEV_NAME.GetStatus().busy);

    FLASH_DEV_NAME.ReadData(sector_addr, &sector_buffer, 0x1000);
    while(FLASH_DEV_NAME.GetStatus().busy);

    /*
    printf("Sector (%x) contents after program:\n", sector_addr);
    for (int i = 0; i < 0x1000; i++) {
        printf("%x ", sector_buffer[i]);
    }
    printf("\n\n");
    */

    return rc;
}

