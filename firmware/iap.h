#ifndef _IAP_H_
#define _IAP_H_


//----- DEFINES -----
#define FLASH_SECTORS                  8
#define FLASH_SECTOR_BYTES             4096
#define FLASH_BYTES                    (FLASH_SECTORS*FLASH_SECTOR_BYTES)


//----- PROTOTYPES -----
uint32_t                               iap_prepare(uint32_t start_sector, uint32_t end_sector);
uint32_t                               iap_copy(uint32_t dst_addr, uint32_t src_addr, uint32_t len);
uint32_t                               iap_erase(uint32_t start_sector, uint32_t end_sector);
uint32_t                               iap_write(uint32_t *data, uint32_t len, uint32_t *tmp);


#endif //_IAP_H_
