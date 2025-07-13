// diskio.c

#include "diskio.h"
#include "sd_io.h"     // Your ulibSD-based SPI driver

extern SD_DEV sddev;

DSTATUS disk_initialize (BYTE pdrv) {
    if (SD_Init(&sddev) == SD_OK)
        return 0;
    return STA_NOINIT;
}

DSTATUS disk_status (BYTE pdrv) {
    return SD_Status(&sddev);
}

DRESULT disk_read (BYTE pdrv, BYTE *buff, DWORD sector, UINT count) {
    while(count--) {
        if (SD_Read(&sddev, buff, sector++, 0, 512) != SD_OK)
            return RES_ERROR;
        buff += 512;
    }
    return RES_OK;
}

#if _READONLY == 0
DRESULT disk_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count) {
    while(count--) {
        if (SD_Write(&sddev, (void*)buff, sector++) != SD_OK)
            return RES_ERROR;
        buff += 512;
    }
    return RES_OK;
}
#endif

DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void *buff) {
    switch(cmd) {
        case GET_SECTOR_COUNT:
            *(DWORD*)buff = sddev.last_sector;
            return RES_OK;
        case GET_SECTOR_SIZE:
            *(WORD*)buff = 512;
            return RES_OK;
        case CTRL_SYNC:
            return RES_OK;
        default:
            return RES_PARERR;
    }
}

DWORD get_fattime(void) {
    // Optional: implement using RTC if you have one
    return ((DWORD)(2025 - 1980) << 25)  // Year
         | ((DWORD)7 << 21)             // Month
         | ((DWORD)12 << 16)            // Day
         | ((DWORD)15 << 11)            // Hour
         | ((DWORD)30 << 5)             // Min
         | ((DWORD)0 >> 1);             // Sec/2
}
