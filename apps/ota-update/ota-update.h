extern resource_t res_upload_image;
extern resource_t res_part_count;
extern resource_t res_part_size;
extern resource_t res_boot_default;
extern resource_t res_boot_next;
extern resource_t res_active_part;
extern resource_t res_part_start;
extern resource_t res_part_ok;
extern resource_t res_reboot;

#define OTA_ACTIVATE_RESOURCES() \
    static char resname[] = "ota/update";\
    rest_activate_resource (&res_upload_image,   resname);\
    rest_activate_resource (&res_part_count,     (char *)"ota/part_count");\
    rest_activate_resource (&res_part_size,      (char *)"ota/part_size");\
    rest_activate_resource (&res_boot_default,   (char *)"ota/boot_default");\
    rest_activate_resource (&res_boot_next,      (char *)"ota/boot_next");\
    rest_activate_resource (&res_active_part,    (char *)"ota/active_part");\
    rest_activate_resource (&res_part_start,     (char *)"ota/part_start");\
    rest_activate_resource (&res_part_ok,        (char *)"ota/part_ok");\
    rest_activate_resource (&res_reboot,         (char *)"ota/reboot");

