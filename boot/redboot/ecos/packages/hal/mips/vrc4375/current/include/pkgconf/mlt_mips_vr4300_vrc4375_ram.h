// eCos memory layout - Fri Oct 20 06:25:55 2000

// This is a generated file - do not edit

#ifndef __ASSEMBLER__
#include <cyg/infra/cyg_type.h>
#include <stddef.h>

#endif
#define CYGMEM_REGION_ram     (0x80100000)
#define CYGMEM_REGION_ram_SIZE (0x1e00000)
#define CYGMEM_REGION_ram_ATTR (CYGMEM_REGION_ATTR_R | CYGMEM_REGION_ATTR_W)
#ifndef __ASSEMBLER__
extern char CYG_LABEL_NAME (__heap1) [];
#endif
#define CYGMEM_SECTION_heap1 (CYG_LABEL_NAME (__heap1))
#define CYGMEM_SECTION_heap1_SIZE (0x81f00000 - (size_t) CYG_LABEL_NAME (__heap1))
#ifndef __ASSEMBLER__
extern char CYG_LABEL_NAME (__pci_window) [];
#endif
#define CYGMEM_SECTION_pci_window (CYG_LABEL_NAME (__pci_window))
#define CYGMEM_SECTION_pci_window_SIZE (0x100000)
