// license:MAME|LGPL-2.1+
// copyright-holders:Michael Zapf
/****************************************************************************

    Nouspikel USB / SmartMedia interface card
    See tn_usmsm.c for documentation

    Michael Zapf

    February 2012: Rewritten as class

*****************************************************************************/

#ifndef __USBSMART__
#define __USBSMART__

#include "emu.h"
#include "ti99defs.h"
#include "peribox.h"
#include "machine/smartmed.h"

extern const device_type TI99_USBSM;

class nouspikel_usb_smartmedia_device : public ti_expansion_card_device
{
public:
	nouspikel_usb_smartmedia_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	DECLARE_READ8Z_MEMBER(readz);
	DECLARE_WRITE8_MEMBER(write);

	DECLARE_READ8Z_MEMBER(crureadz);
	DECLARE_WRITE8_MEMBER(cruwrite);

protected:
	virtual void device_start(void);
	virtual void device_reset(void);
	virtual const rom_entry *device_rom_region() const;
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual ioport_constructor device_input_ports() const;

private:
	UINT16      usbsm_mem_16_r(offs_t offset);
	void        usbsm_mem_16_w(offs_t offset, UINT16 data);
	smartmedia_image_device*    m_smartmedia;
	device_t*   m_strata;

	int         m_feeprom_page;
	int         m_sram_page;
	int         m_cru_register;
	bool        m_tms9995_mode;

	UINT16      m_input_latch;
	UINT16      m_output_latch;
	UINT16*     m_ram;
};

#endif
