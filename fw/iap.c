#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "cmsis/LPC11xx.h"
#include "main.h"
#include "settings.h"
#include "iap.h"


//IAP commands
#define	IAP_PREPARE_SECTORS            50
#define	IAP_COPY_RAM_TO_FLASH          51
#define	IAP_ERASE_SECTORS              52

//IAP boot ROM location and access function
#define IAP_ROM_LOCATION               0x1FFF1FF1UL
#define IAP_EXECUTE_CMD(a, b)          ((void(*)())(IAP_ROM_LOCATION))(a, b)


uint32_t iap_prepare(uint32_t start_sector, uint32_t end_sector)
{
  uint32_t cmd[5], result[4], status;

  if(start_sector <= end_sector)
  {
    cmd[0] = IAP_PREPARE_SECTORS;
    cmd[1] = start_sector;
    cmd[2] = end_sector;
    IAP_EXECUTE_CMD(cmd, result);
    status = result[0];
  }
  else
  {
    status = 7; //7 invalid sector number
  }

  return status;
}


uint32_t iap_copy(uint32_t dst_addr, uint32_t src_addr, uint32_t len)
{
  uint32_t cmd[5], result[4], status;

  cmd[0] = IAP_COPY_RAM_TO_FLASH;
  cmd[1] = dst_addr;
  cmd[2] = src_addr;
  cmd[3] = len; //256, 512, 1024 or 4096
  cmd[4] = sysclock(0) / 1000UL; //core clock frequency in kHz
  IAP_EXECUTE_CMD(cmd, result);
  status = result[0];

  return status;
}


uint32_t iap_erase(uint32_t start_sector, uint32_t end_sector)
{
  uint32_t cmd[5], result[4], status;

  if(start_sector <= end_sector)
  {
    cmd[0] = IAP_ERASE_SECTORS;
    cmd[1] = start_sector;
    cmd[2] = end_sector;
    cmd[3] = sysclock(0) / 1000UL; //core clock frequency in kHz
    IAP_EXECUTE_CMD(cmd, result);
    status = result[0];
  }
  else
  {
    status = 7; //7 invalid sector number
  }

  return status;
}


uint32_t iap_write(uint32_t *data, uint32_t len, uint32_t *tmp)
{
  uint32_t i, status=1;
  //uint32_t tmp[FLASH_SECTOR_BYTES/4];
  uint32_t *ptr = (uint32_t *)(FLASH_BYTES-FLASH_SECTOR_BYTES);

  if(len != 0)
  {
    DISABLE_IRQ();

    //save last sector
    for(i=0; i<(FLASH_SECTOR_BYTES/4); i++)
    {
      tmp[i] = *ptr++;
    }

    //overwrite last 128 bytes of sector with data
    for(i=0; i<len; i++)
    {
      tmp[(FLASH_SECTOR_BYTES/4)-(128/4)+i] = *data++;
    }

    //erase and re-program sector
    if(iap_prepare(FLASH_SECTORS-1, FLASH_SECTORS-1) == 0)
    {
      if(iap_erase(FLASH_SECTORS-1, FLASH_SECTORS-1) == 0)
      {
        if(iap_prepare(FLASH_SECTORS-1, FLASH_SECTORS-1) == 0)
        {
          if(iap_copy(FLASH_BYTES-FLASH_SECTOR_BYTES, (uint32_t)tmp, FLASH_SECTOR_BYTES) == 0)
          {
            status = 0;
          }
        }
      }
    }

    ENABLE_IRQ();
  }

  return status;
}
