#include "sound/msm5205.h"

class battlera_state : public driver_device
{
public:
	battlera_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_audiocpu(*this, "audiocpu"),
		m_msm(*this, "msm"),
		m_gfxdecode(*this, "gfxdecode"),
		m_screen(*this, "screen"),
		m_palette(*this, "palette") { }

	int m_control_port_select;
	int m_msm5205next;
	int m_toggle;
	int m_HuC6270_registers[20];
	int m_VDC_register;
	int m_vram_ptr;
	UINT8 *m_HuC6270_vram;
	UINT8 *m_vram_dirty;
	bitmap_ind16 *m_tile_bitmap;
	bitmap_ind16 *m_front_bitmap;
	UINT32 m_tile_dirtyseq;
	int m_current_scanline;
	int m_inc_value;
	int m_irq_enable;
	int m_rcr_enable;
	int m_sb_enable;
	int m_bb_enable;
	int m_bldwolf_vblank;
	UINT8 m_blank_tile[32];

	required_device<cpu_device> m_maincpu;
	DECLARE_WRITE8_MEMBER(battlera_sound_w);
	DECLARE_WRITE8_MEMBER(control_data_w);
	DECLARE_READ8_MEMBER(control_data_r);
	DECLARE_WRITE8_MEMBER(battlera_adpcm_data_w);
	DECLARE_WRITE8_MEMBER(battlera_palette_w);
	DECLARE_READ8_MEMBER(HuC6270_debug_r);
	DECLARE_WRITE8_MEMBER(HuC6270_debug_w);
	DECLARE_READ8_MEMBER(HuC6270_register_r);
	DECLARE_WRITE8_MEMBER(HuC6270_register_w);
	DECLARE_READ8_MEMBER(HuC6270_data_r);
	DECLARE_WRITE8_MEMBER(HuC6270_data_w);
	DECLARE_WRITE8_MEMBER(battlera_adpcm_reset_w);
	virtual void video_start();
	UINT32 screen_update_battlera(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	TIMER_DEVICE_CALLBACK_MEMBER(battlera_irq);
	void draw_sprites(bitmap_ind16 &bitmap,const rectangle &clip,int pri);
	DECLARE_WRITE_LINE_MEMBER(battlera_adpcm_int);
	required_device<cpu_device> m_audiocpu;
	required_device<msm5205_device> m_msm;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;	
};
