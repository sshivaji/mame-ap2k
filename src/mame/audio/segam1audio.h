#pragma once

#ifndef __SEGAM1AUDIO_H__
#define __SEGAM1AUDIO_H__

#include "emu.h"
#include "cpu/m68000/m68000.h"
#include "sound/2612intf.h"
#include "sound/multipcm.h"

#define M1AUDIO_CPU_REGION "m1sndcpu"
#define M1AUDIO_MPCM1_REGION "m1pcm1"
#define M1AUDIO_MPCM2_REGION "m1pcm2"

#define MCFG_SEGAM1AUDIO_ADD(_tag) \
	MCFG_DEVICE_ADD(_tag, SEGAM1AUDIO, 0)

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class segam1audio_device : public device_t
{
public:
		// construction/destruction
		segam1audio_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

		// optional information overrides
		virtual machine_config_constructor device_mconfig_additions() const;

		required_device<cpu_device> m_audiocpu;
		required_device<multipcm_device> m_multipcm_1;
		required_device<multipcm_device> m_multipcm_2;
		required_device<ym3438_device> m_ym;

		DECLARE_READ16_MEMBER(m1_snd_68k_latch_r);
		DECLARE_READ16_MEMBER(m1_snd_v60_ready_r);
		DECLARE_WRITE16_MEMBER(m1_snd_mpcm_bnk1_w);
		DECLARE_WRITE16_MEMBER(m1_snd_mpcm_bnk2_w);
		DECLARE_WRITE16_MEMBER(m1_snd_68k_latch1_w);
		DECLARE_WRITE16_MEMBER(m1_snd_68k_latch2_w);
		DECLARE_READ16_MEMBER(ready_r);

		void check_fifo_irq();
		void write_fifo(UINT8 data);

protected:
		// device-level overrides
		virtual void device_start();
		virtual void device_reset();

private:
	int m_to_68k[8];
	int m_fifo_rptr;
	int m_fifo_wptr;
};


// device type definition
extern const device_type SEGAM1AUDIO;

#endif  /* __SEGAM1AUDIO_H__ */
