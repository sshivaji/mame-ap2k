/***************************************************************************

    Video Hardware for Blood Brothers

***************************************************************************/

#include "emu.h"
#include "includes/bloodbro.h"


/***************************************************************************

  Callbacks for the TileMap code

***************************************************************************/

TILE_GET_INFO_MEMBER(bloodbro_state::get_bg_tile_info)
{
	int code = m_bgvideoram[tile_index];
	SET_TILE_INFO_MEMBER(
			1,
			code & 0xfff,
			(code >> 12),
			0);
}

TILE_GET_INFO_MEMBER(bloodbro_state::get_fg_tile_info)
{
	int code = m_fgvideoram[tile_index];
	SET_TILE_INFO_MEMBER(
			2,
			(code & 0xfff)+0x1000,
			(code >> 12),
			0);
}

TILE_GET_INFO_MEMBER(bloodbro_state::get_tx_tile_info)
{
	int code = m_txvideoram[tile_index];
	SET_TILE_INFO_MEMBER(
			0,
			code & 0xfff,
			code >> 12,
			0);
}



/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/

void bloodbro_state::video_start()
{
	m_bg_tilemap = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(bloodbro_state::get_bg_tile_info),this),TILEMAP_SCAN_ROWS,16,16,32,16);
	m_fg_tilemap = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(bloodbro_state::get_fg_tile_info),this),TILEMAP_SCAN_ROWS,16,16,32,16);
	m_tx_tilemap = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(bloodbro_state::get_tx_tile_info),this),TILEMAP_SCAN_ROWS, 8, 8,32,32);

	m_fg_tilemap->set_transparent_pen(15);
	m_tx_tilemap->set_transparent_pen(15);
}



/***************************************************************************

  Memory handlers

***************************************************************************/

WRITE16_MEMBER(bloodbro_state::bloodbro_bgvideoram_w)
{
	COMBINE_DATA(&m_bgvideoram[offset]);
	m_bg_tilemap->mark_tile_dirty(offset);
}

WRITE16_MEMBER(bloodbro_state::bloodbro_fgvideoram_w)
{
	COMBINE_DATA(&m_fgvideoram[offset]);
	m_fg_tilemap->mark_tile_dirty(offset);
}

WRITE16_MEMBER(bloodbro_state::bloodbro_txvideoram_w)
{
	COMBINE_DATA(&m_txvideoram[offset]);
	m_tx_tilemap->mark_tile_dirty(offset);
}



/***************************************************************************

  Display refresh


    Blood Bros / Skysmash Spriteram
    -------------------------------

    Slightly more sophisticated successor to the Toki sprite chip.

    It has "big sprites" created by setting width or height >0. Tile
    numbers are read consecutively.

    +0   x....... ........  sprite disabled if set
    +0   .x...... ........  Flip y (no evidence for this!!)
    +0   ..x..... ........  Flip x
    +0   ....x... ........  Priority (1=high)
    +0   ......xx x.......  Width: do this many tiles horizontally
    +0   ........ .xxx....  Height: do this many tiles vertically
    +0   ........ ....xxxx  Color bank

    +1   ...xxxxx xxxxxxxx  Tile number
    +2   .......x xxxxxxxx  X coordinate
    +3   .......x xxxxxxxx  Y coordinate


    Weststry Bootleg Spriteram
    --------------------------

    Lacks the "big sprite" feature of the original. Needs some
    tile number remapping for some reason.

    +0   .......x xxxxxxxx  Sprite Y coordinate
    +1   ...xxxxx xxxxxxxx  Sprite tile number
    +2   xxxx.... ........  Sprite color bank
    +2   ......x. ........  Sprite flip x
    +2   ........ x.......  Priority ??
    +3   .......x xxxxxxxx  Sprite X coordinate

***************************************************************************/

/* SPRITE INFO (8 bytes)

   D-F?P?SS SSSSCCCC
   ---TTTTT TTTTTTTT
   -------X XXXXXXXX
   -------- YYYYYYYY */

void bloodbro_state::bloodbro_draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	UINT16 *spriteram16 = m_spriteram;
	int offs;
	for (offs = 0;offs < m_spriteram.bytes()/2;offs += 4)
	{
		int sx,sy,x,y,width,height,attributes,tile_number,color,flipx,flipy,pri_mask;

		attributes = spriteram16[offs+0];
		if (attributes & 0x8000) continue;  /* disabled */

		width = ((attributes>>7)&7);
		height = ((attributes>>4)&7);
		pri_mask = (attributes & 0x0800) ? 0x02 : 0;
		tile_number = spriteram16[offs+1]&0x1fff;
		sx = spriteram16[offs+2]&0x1ff;
		sy = spriteram16[offs+3]&0x1ff;
		if (sx >= 256) sx -= 512;
		if (sy >= 256) sy -= 512;

		flipx = attributes & 0x2000;
		flipy = attributes & 0x4000;    /* ?? */
		color = attributes & 0xf;

		for (x = 0;x <= width;x++)
		{
			for (y = 0;y <= height;y++)
			{
				pdrawgfx_transpen(bitmap,cliprect,machine().gfx[3],
						tile_number++,
						color,
						flipx,flipy,
						flipx ? (sx + 16*(width-x)) : (sx + 16*x),flipy ? (sy + 16*(height-y)) : (sy + 16*y),
						machine().priority_bitmap,
						pri_mask,15);
			}
		}
	}
}

/* SPRITE INFO (8 bytes)

   D------- YYYYYYYY
   ---TTTTT TTTTTTTT
   CCCC--F? -?--????  Priority??
   -------X XXXXXXXX
*/

void bloodbro_state::weststry_draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	UINT16 *spriteram16 = m_spriteram;
	int offs;

	/* TODO: the last two entries are not sprites - control registers? */
	for (offs = 0;offs < m_spriteram.bytes()/2 - 8;offs += 4)
	{
		int data = spriteram16[offs+2];
		int data0 = spriteram16[offs+0];
		int code = spriteram16[offs+1]&0x1fff;
		int sx = spriteram16[offs+3]&0x1ff;
		int sy = 0xf0-(data0&0xff);
		int flipx = data & 0x200;
		int flipy = data & 0x400;   /* ??? */
		int color = (data&0xf000)>>12;
		int pri_mask = (data & 0x0080) ? 0x02 : 0;

		if (sx >= 256) sx -= 512;

		if (data0 & 0x8000) continue;   /* disabled */

		/* Remap code 0x800 <-> 0x1000 */
		code = (code&0x7ff) | ((code&0x800)<<1) | ((code&0x1000)>>1);

		pdrawgfx_transpen(bitmap,cliprect,machine().gfx[3],
				code,
				color,
				flipx,flipy,
				sx,sy,
				machine().priority_bitmap,
				pri_mask,15);
	}
}



UINT32 bloodbro_state::screen_update_bloodbro(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	m_bg_tilemap->set_scrollx(0,m_scrollram[0]);
	m_bg_tilemap->set_scrolly(0,m_scrollram[1]);
	m_fg_tilemap->set_scrollx(0,m_scrollram[2]);
	m_fg_tilemap->set_scrolly(0,m_scrollram[3]);

	machine().priority_bitmap.fill(0, cliprect);

	if(!(m_layer_en & 1))
		m_bg_tilemap->draw(bitmap, cliprect, 0,0);
	if(!(m_layer_en & 4))
		m_fg_tilemap->draw(bitmap, cliprect, 0,1);
	if(!(m_layer_en & 0x10))
		bloodbro_draw_sprites(bitmap,cliprect);
	if(!(m_layer_en & 8))
		m_tx_tilemap->draw(bitmap, cliprect, 0,0);
	return 0;
}

UINT32 bloodbro_state::screen_update_weststry(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
//  m_bg_tilemap->set_scrollx(0,m_scroll[0x10]);    /* ? */
//  m_bg_tilemap->set_scrolly(0,m_scroll[0x11]);    /* ? */
//  m_fg_tilemap->set_scrollx(0,m_scroll[0x12]);
//  m_fg_tilemap->set_scrolly(0,m_scroll[0x13]);

	machine().priority_bitmap.fill(0, cliprect);

	m_bg_tilemap->draw(bitmap, cliprect, 0,0);
	m_fg_tilemap->draw(bitmap, cliprect, 0,1);
	weststry_draw_sprites(bitmap,cliprect);
	m_tx_tilemap->draw(bitmap, cliprect, 0,0);
	return 0;
}


UINT32 bloodbro_state::screen_update_skysmash(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	m_bg_tilemap->set_scrollx(0,m_scrollram[0]);
	m_bg_tilemap->set_scrolly(0,m_scrollram[1]);
	m_fg_tilemap->set_scrollx(0,m_scrollram[2]);
	m_fg_tilemap->set_scrolly(0,m_scrollram[3]);

	machine().priority_bitmap.fill(0, cliprect);

	if(!(m_layer_en & 1))
		m_bg_tilemap->draw(bitmap, cliprect, 0,0);
	if(!(m_layer_en & 4))
		m_fg_tilemap->draw(bitmap, cliprect, 0,1);
	if(!(m_layer_en & 0x10))
		bloodbro_draw_sprites(bitmap,cliprect);
	if(!(m_layer_en & 8))
		m_tx_tilemap->draw(bitmap, cliprect, 0,0);

	return 0;
}
