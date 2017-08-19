#ifndef BOOTLOADER_IF_H_
#define BOOTLOADER_IF_H_

extern int      bootloader_write_page_to_flash
    (uint32_t address, unsigned int size, unsigned char *p);

extern uint8_t  bootloader_get_mac          (uint8_t);
extern uint32_t bootloader_get_part_count   (void);
extern uint32_t bootloader_get_part_size    (void);
extern uint32_t bootloader_get_part_start   (uint32_t part_index);
extern void     bootloader_set_part_ok      (uint32_t part_index);
extern void     bootloader_clr_part_ok      (uint32_t part_index);
extern void     bootloader_set_boot_default (uint32_t part_index);
extern void     bootloader_set_boot_next    (uint32_t part_index);
extern void     bootloader_backup_irq_table (uint32_t part_index);
extern uint32_t bootloader_get_boot_default (void);
extern uint32_t bootloader_get_boot_next    (void);
extern uint32_t bootloader_get_callers_part (void);

#endif /* BOOTLOADER_IF_H_ */
