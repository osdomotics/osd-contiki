#ifndef BOOTLOADER_IF_H_
#define BOOTLOADER_IF_H_

extern uint8_t bootloader_get_mac(uint8_t);
extern int bootloader_write_page_to_flash
    (uint32_t address, unsigned int size, unsigned char *p);

#endif /* BOOTLOADER_IF_H_ */
