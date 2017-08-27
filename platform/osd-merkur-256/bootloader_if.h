#ifndef BOOTLOADER_IF_H_
#define BOOTLOADER_IF_H_

extern uint8_t  bootloader_get_mac           (uint8_t);
extern uint32_t bootloader_get_part_count    (void);
extern uint32_t bootloader_get_part_size     (void);
extern uint32_t bootloader_get_part_start    (uint32_t part_index);
extern uint32_t bootloader_get_boot_default  (void);
extern uint32_t bootloader_get_boot_next     (void);
extern uint32_t bootloader_get_active_part   (void);
extern uint32_t bootloader_get_part_ok       (uint32_t part_index);

/* These write to flash and need to turn off interrupts before start */
extern void     _bootloader_set_part_ok      (uint32_t part_index);
extern void     _bootloader_clr_part_ok      (uint32_t part_index);
extern void     _bootloader_set_boot_default (uint32_t part_index);
extern void     _bootloader_set_boot_next    (uint32_t part_index);
extern int      _bootloader_write_page_to_flash
    (uint32_t address, unsigned int size, unsigned char *p);

/* Wrap the routines that write to flash (see above) */

static inline void bootloader_set_part_ok (uint32_t part_index)
{
    uint8_t sreg = SREG;
    cli ();
    _bootloader_set_part_ok (part_index);
    SREG = sreg;
}

static inline void bootloader_clr_part_ok (uint32_t part_index)
{
    uint8_t sreg = SREG;
    cli ();
    _bootloader_clr_part_ok (part_index);
    SREG = sreg;
}

static inline void bootloader_set_boot_default (uint32_t part_index)
{
    uint8_t sreg = SREG;
    cli ();
    _bootloader_set_boot_default (part_index);
    SREG = sreg;
}

static inline void bootloader_set_boot_next (uint32_t part_index)
{
    uint8_t sreg = SREG;
    cli ();
    _bootloader_set_boot_next (part_index);
    SREG = sreg;
}

static inline int bootloader_write_page_to_flash 
    (uint32_t address, unsigned int size, unsigned char *p)
{
    int ret = 0;
    uint8_t sreg = SREG;
    cli ();
    ret = _bootloader_write_page_to_flash (address, size, p);
    SREG = sreg;
    return ret;
}

#endif /* BOOTLOADER_IF_H_ */
