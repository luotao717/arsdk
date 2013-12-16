/*
 * Copyright (c) 2010, Atheros Communications Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * @ingroup adf_os_public
 * @file adf_os_pci.h
 * This file abstracts the PCI subsystem.
 */
#ifndef __ADF_OS_PCI_H
#define __ADF_OS_PCI_H

#include <adf_os_pci_pvt.h>

/**
 * @brief Define the entry point for the PCI module.
 */ 
#define adf_os_pci_module_init(_fn)     __adf_os_pci_module_init(_fn)

/**
 * @brief Define the exit point for the PCI module.
 */ 
#define adf_os_pci_module_exit(_fn)     __adf_os_pci_module_exit(_fn)

/**
 * @brief Setup the following driver information: name, PCI IDs of devices
 * supported and some device handlers.
 */ 
#define adf_os_pci_set_drv_info(_name, _pci_ids, _attach, _detach, _suspend, _resume) \
    __adf_os_pci_set_drv_info(_name, _pci_ids, _attach, _detach, _suspend, _resume)

/**
 * @brief Read a byte of PCI config space.
 *
 * @param[in]  osdev    platform device instance
 * @param[in]  offset   offset to read
 * @param[out] val      value read
 *
 * @return status of operation
 */ 
static inline int 
adf_os_pci_config_read8(adf_os_device_t osdev, int offset, a_uint8_t *val)
{
    return __adf_os_pci_config_read8(osdev, offset, val);
}

/**
 * @brief Write a byte to PCI config space.
 *
 * @param[in] osdev    platform device instance
 * @param[in] offset   offset to write
 * @param[in] val      value to write
 *
 * @return status of operation
 */ 
static inline int 
adf_os_pci_config_write8(adf_os_device_t osdev, int offset, a_uint8_t val)
{
    return __adf_os_pci_config_write8(osdev, offset, val);
}

/**
 * @brief Read 2 bytes of PCI config space.
 *
 * @param[in]  osdev    platform device instance
 * @param[in]  offset   offset to read
 * @param[out] val      value read
 *
 * @return status of operation
 */ 
static inline int 
adf_os_pci_config_read16(adf_os_device_t osdev, int offset, a_uint16_t *val)
{
    return __adf_os_pci_config_read16(osdev, offset, val);
}

/**
 * @brief Write 2 bytes to PCI config space.
 *
 * @param[in] osdev    platform device instance
 * @param[in] offset   offset to write
 * @param[in] val      value to write
 *
 * @return status of operation
 */ 
static inline int 
adf_os_pci_config_write16(adf_os_device_t osdev, int offset, a_uint16_t val)
{
    return __adf_os_pci_config_write16(osdev, offset, val);
}

/**
 * @brief Read 4 bytes of PCI config space.
 *
 * @param[in]  osdev    platform device instance
 * @param[in]  offset   offset to read
 * @param[out] val      value read
 *
 * @return status of operation
 */ 
static inline int 
adf_os_pci_config_read32(adf_os_device_t osdev, int offset, a_uint32_t *val)
{
    return __adf_os_pci_config_read32(osdev, offset, val);
}

/**
 * @brief Write 4 bytes to PCI config space.
 *
 * @param[in] osdev    platform device instance
 * @param[in] offset   offset to write
 * @param[in] val      value to write
 *
 * @return status of operation
 */ 
static inline int 
adf_os_pci_config_write32(adf_os_device_t osdev, int offset, a_uint32_t val)
{
    return __adf_os_pci_config_write32(osdev, offset, val);
}
#endif

