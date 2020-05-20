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


// ----------------------

typedef void __attribute__((cmse_nonsecure_call)) ns_next_cb(void);
typedef void _write_next_cb(void);

ns_next_cb *next_cb = NULL;
_write_next_cb *_write_next = NULL;

__tfm_secure_gateway_attributes__
uint8_t tfm_flash_write_step() {
    if (!_write_next) {
        //LOG_MSG("!write next\n");
        return 0;
    }

    if (!FLASH_DEV_NAME.GetStatus().busy) {
        //LOG_MSG("not busy, calling next\n");
        _write_next();
    }
    //LOG_MSG("return 1\n");
    return 1;
}

#define SECTOR_SIZE 0x1000
static uint8_t sector_buffer[SECTOR_SIZE];

uint32_t overall_addr = 0x0;
uint32_t *overall_buf = NULL;
uint32_t overall_len = 0;
uint32_t overall_written = 0;

// single sector write vvv

uint32_t write_addr = 0x0;
uint32_t *write_buf = NULL;
uint32_t write_len = 0;

uint32_t write_sector_offset = 0;
uint32_t write_sector_addr = 0x0;
uint32_t write_bytes_to_write = 0;

void _write_single_sector(uint32_t, uint32_t *, uint32_t);

void _write_entry(void) {

    if (overall_len == overall_written) {
        overall_written = 0;
        _write_next = NULL; 
    } else {
        _write_single_sector(
                overall_addr + overall_written,
                overall_buf + (overall_written / sizeof(uint32_t)),
                overall_len - overall_written
        );
    }
}

void _write_program_sector(void) {
    // XXX ignore errors for now
    FLASH_DEV_NAME.ProgramData(write_sector_addr, sector_buffer, SECTOR_SIZE); 
    overall_written += write_bytes_to_write;

    _write_next = _write_entry;
}

void _write_erase_sector(void) {
    memcpy(&sector_buffer[write_sector_offset], write_buf, write_bytes_to_write);

    // XXX ignore errors for now
    FLASH_DEV_NAME.EraseSector(write_sector_addr);
    _write_next = _write_program_sector;
}

void _write_single_sector(uint32_t addr, uint32_t *buf, uint32_t len) {

    write_addr = addr;
    write_buf = buf;
    write_len = len;

    write_sector_offset = write_addr % SECTOR_SIZE;
    write_sector_addr = write_addr - write_sector_offset;

    write_bytes_to_write = SECTOR_SIZE - write_sector_offset;
    if (write_len < write_bytes_to_write) write_bytes_to_write = write_len;

    // Read previous sector contents
    // XXX ignore errors for now 
    //int rc = FLASH_DEV_NAME.ReadData(write_sector_addr, &write_sector_buffer, SECTOR_SIZE);
    FLASH_DEV_NAME.ReadData(write_sector_addr, &sector_buffer, SECTOR_SIZE);

    _write_next = _write_erase_sector;
}

__tfm_secure_gateway_attributes__
uint8_t tfm_flash_write(uint32_t addr, uint32_t *buf, uint32_t len) {

    if (tfm_flash_is_busy() || _write_next != NULL) return -1; // write in progress

    overall_addr = addr;
    overall_buf = buf;
    overall_len = len;

    _write_entry();
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
    LOG_MSG("Sector (%x) contents before modification:\n", sector_addr);
    for (int i = 0; i < 0x1000; i++) {
        LOG_MSG("%x ", sector_buffer[i]);
    }
    LOG_MSG("\n\n");
    * /

    memcpy(&sector_buffer[sector_offset], buf, len);

    / *
    LOG_MSG("Sector (%x) contents after modification of %d bytes at sector offset %x:\n", sector_addr, len, sector_offset);
    for (int i = 0; i < 0x1000; i++) {
        LOG_MSG("%x ", sector_buffer[i]);
    }
    LOG_MSG("\n\n");
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
    LOG_MSG("Sector (%x) contents after program:\n", sector_addr);
    for (int i = 0; i < 0x1000; i++) {
        LOG_MSG("%x ", sector_buffer[i]);
    }
    LOG_MSG("\n\n");
    * /
    */
}

