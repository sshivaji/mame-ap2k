/***************************************************************************

Eggs & Dommy

Very similar to Burger Time hardware (and uses its video driver)

driver by Nicola Salmoria

To Do:
Sprite Priorities in Dommy


Rock Duck
---------

Rock Duck support added based on preliminary findings by Roberto Fresca
It looks like the game may be a ghastly hack of one of the 'eggs' titles
(need to check code but gameplay appears to be the same)

The dump was from a rom 'blister' (no PCB available) and is missing at
least the colour PROM.


Some notes by Roberto Fresca:
----------------------------

1 - The hack was made using "Scrambled Egg" program roms instead of "Eggs".

2 - The code is almost identical to scregg except for a couple of changed
    zero page registers, some data for gfx, strings, and two new subroutines.

Strictly to the code, they re-routed two subroutines to the unused high
program space where there are only zeroes:

$5732: jmp $7f0e (when start to drawing the screen)
This subroutine ($7f0e) is used to draw the game's frame with the holes and
tunnel.

$30ca: jsr $7f44 (called constantly)
This one is amazing!... They made this subroutine ($7f44) only to protect
the string "(R)DATEL SAS" placed at bottom-right of the screen. The code
compare the string stored in rom ($62d8 - $62e1) against the one drawn in
the videoram ($13cf - $13d8"). If something is different, just jump to a
reset ($3003) and the game starts again.

Obviously the string isn't easily visible. You need some arithmetic to show
it as ASCII text.


***************************************************************************/

#include "driver.h"
#include "deprecat.h"
#include "sound/ay8910.h"


/* from video/btime.c */
extern UINT8 *btime_videoram;
extern size_t btime_videoram_size;
extern UINT8 *btime_colorram;

PALETTE_INIT( btime );
VIDEO_START( btime );
VIDEO_UPDATE( eggs );

READ8_HANDLER( btime_mirrorvideoram_r );
WRITE8_HANDLER( btime_mirrorvideoram_w );
READ8_HANDLER( btime_mirrorcolorram_r );
WRITE8_HANDLER( btime_mirrorcolorram_w );
WRITE8_HANDLER( btime_video_control_w );

static ADDRESS_MAP_START( dommy_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x07ff) AM_READ(SMH_RAM)
	AM_RANGE(0x2000, 0x27ff) AM_READ(SMH_RAM)
	AM_RANGE(0x2800, 0x2bff) AM_READ(btime_mirrorvideoram_r)
	AM_RANGE(0x4000, 0x4000) AM_READ(input_port_2_r)     /* DSW1 */
	AM_RANGE(0x4001, 0x4001) AM_READ(input_port_3_r)     /* DSW2 */
/*  AM_RANGE(0x4004, 0x4004)  */ /* this is read */
	AM_RANGE(0x4002, 0x4002) AM_READ(input_port_0_r)     /* IN0 */
	AM_RANGE(0x4003, 0x4003) AM_READ(input_port_1_r)     /* IN1 */
	AM_RANGE(0xa000, 0xffff) AM_READ(SMH_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( dommy_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x07ff) AM_WRITE(SMH_RAM)
	AM_RANGE(0x2000, 0x23ff) AM_WRITE(SMH_RAM) AM_BASE(&btime_videoram) AM_SIZE(&btime_videoram_size)
	AM_RANGE(0x2400, 0x27ff) AM_WRITE(SMH_RAM) AM_BASE(&btime_colorram)
	AM_RANGE(0x2800, 0x2bff) AM_WRITE(btime_mirrorvideoram_w)
	AM_RANGE(0x4000, 0x4000) AM_WRITE(SMH_NOP)
	AM_RANGE(0x4001, 0x4001) AM_WRITE(btime_video_control_w)
	AM_RANGE(0x4004, 0x4004) AM_WRITE(AY8910_control_port_0_w)
	AM_RANGE(0x4005, 0x4005) AM_WRITE(AY8910_write_port_0_w)
	AM_RANGE(0x4006, 0x4006) AM_WRITE(AY8910_control_port_1_w)
	AM_RANGE(0x4007, 0x4007) AM_WRITE(AY8910_write_port_1_w)
	AM_RANGE(0xa000, 0xffff) AM_WRITE(SMH_ROM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( eggs_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x07ff) AM_READ(SMH_RAM)
	AM_RANGE(0x1000, 0x17ff) AM_READ(SMH_RAM)
	AM_RANGE(0x1800, 0x1bff) AM_READ(btime_mirrorvideoram_r)
	AM_RANGE(0x1c00, 0x1fff) AM_READ(btime_mirrorcolorram_r)
	AM_RANGE(0x2000, 0x2000) AM_READ(input_port_2_r)     /* DSW1 */
	AM_RANGE(0x2001, 0x2001) AM_READ(input_port_3_r)     /* DSW2 */
	AM_RANGE(0x2002, 0x2002) AM_READ(input_port_0_r)     /* IN0 */
	AM_RANGE(0x2003, 0x2003) AM_READ(input_port_1_r)     /* IN1 */
	AM_RANGE(0x3000, 0x7fff) AM_READ(SMH_ROM)
	AM_RANGE(0xf000, 0xffff) AM_READ(SMH_ROM)    /* reset/interrupt vectors */
ADDRESS_MAP_END

static ADDRESS_MAP_START( eggs_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x07ff) AM_WRITE(SMH_RAM)
	AM_RANGE(0x1000, 0x13ff) AM_WRITE(SMH_RAM) AM_BASE(&btime_videoram) AM_SIZE(&btime_videoram_size)
	AM_RANGE(0x1400, 0x17ff) AM_WRITE(SMH_RAM) AM_BASE(&btime_colorram)
	AM_RANGE(0x1800, 0x1bff) AM_WRITE(btime_mirrorvideoram_w)
	AM_RANGE(0x1c00, 0x1fff) AM_WRITE(btime_mirrorcolorram_w)
	AM_RANGE(0x2000, 0x2000) AM_WRITE(btime_video_control_w)
	AM_RANGE(0x2001, 0x2001) AM_WRITE(SMH_NOP)
	AM_RANGE(0x2004, 0x2004) AM_WRITE(AY8910_control_port_0_w)
	AM_RANGE(0x2005, 0x2005) AM_WRITE(AY8910_write_port_0_w)
	AM_RANGE(0x2006, 0x2006) AM_WRITE(AY8910_control_port_1_w)
	AM_RANGE(0x2007, 0x2007) AM_WRITE(AY8910_write_port_1_w)
	AM_RANGE(0x3000, 0x7fff) AM_WRITE(SMH_ROM)
ADDRESS_MAP_END



static INPUT_PORTS_START( scregg )
	PORT_START      /* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

	PORT_START      /* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY PORT_COCKTAIL
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START      /* DSW1 */
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_3C ) )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_3C ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Cocktail ) )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_VBLANK  )

	PORT_START      /* DSW2 */
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x01, "3" )
	PORT_DIPSETTING(    0x00, "5" )
	PORT_DIPNAME( 0x06, 0x04, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x04, "30000" )
	PORT_DIPSETTING(    0x02, "50000" )
	PORT_DIPSETTING(    0x00, "70000"  )
	PORT_DIPSETTING(    0x06, "Never"  )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hard ) )
INPUT_PORTS_END




static const gfx_layout charlayout =
{
	8,8,    /* 8*8 characters */
	1024,   /* 1024 characters */
	3,      /* 3 bits per pixel */
	{ 2*1024*8*8, 1024*8*8, 0 },    /* the bitplanes are separated */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8     /* every char takes 8 consecutive bytes */
};

static const gfx_layout spritelayout =
{
	16,16,  /* 16*16 sprites */
	256,    /* 256 sprites */
	3,      /* 3 bits per pixel */
	{ 2*256*16*16, 256*16*16, 0 },  /* the bitplanes are separated */
	{ 16*8+0, 16*8+1, 16*8+2, 16*8+3, 16*8+4, 16*8+5, 16*8+6, 16*8+7,
	  0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
	  8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	32*8    /* every sprite takes 32 consecutive bytes */
};

static GFXDECODE_START( scregg )
	GFXDECODE_ENTRY( REGION_GFX1, 0, charlayout,          0, 1 )     /* char set #1 */
	GFXDECODE_ENTRY( REGION_GFX1, 0, spritelayout,        0, 1 )     /* sprites */
GFXDECODE_END



static MACHINE_DRIVER_START( dommy )

	/* basic machine hardware */
	MDRV_CPU_ADD(M6502, 1500000)
	MDRV_CPU_PROGRAM_MAP(dommy_readmem,dommy_writemem)
	MDRV_CPU_VBLANK_INT_HACK(irq0_line_hold,16)

	/* video hardware */
	MDRV_SCREEN_ADD("main", RASTER)
	MDRV_SCREEN_REFRESH_RATE(57)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(3072)        /* frames per second, vblank duration taken from Burger Time */)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 31*8-1, 1*8, 31*8-1)

	MDRV_GFXDECODE(scregg)
	MDRV_PALETTE_LENGTH(8)

	MDRV_PALETTE_INIT(btime)
	MDRV_VIDEO_START(btime)
	MDRV_VIDEO_UPDATE(eggs)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(AY8910, 1500000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.23)

	MDRV_SOUND_ADD(AY8910, 1500000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.23)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( scregg )

	/* basic machine hardware */
	MDRV_CPU_ADD(M6502, 1500000)
	MDRV_CPU_PROGRAM_MAP(eggs_readmem,eggs_writemem)
	MDRV_CPU_VBLANK_INT_HACK(irq0_line_hold,16)

	/* video hardware */
	MDRV_SCREEN_ADD("main", RASTER)
	MDRV_SCREEN_REFRESH_RATE(57)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(3072)        /* frames per second, vblank duration taken from Burger Time */)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(1*8, 31*8-1, 1*8, 31*8-1)

	MDRV_GFXDECODE(scregg)
	MDRV_PALETTE_LENGTH(8)

	MDRV_PALETTE_INIT(btime)
	MDRV_VIDEO_START(btime)
	MDRV_VIDEO_UPDATE(eggs)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(AY8910, 1500000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.23)

	MDRV_SOUND_ADD(AY8910, 1500000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.23)
MACHINE_DRIVER_END


ROM_START( dommy )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "dommy.e01",  0xa000, 0x2000, CRC(9ae064ed) SHA1(73082e5254d54d8386f580cc82a74242a6debd84) )
	ROM_LOAD( "dommy.e11",  0xc000, 0x2000, CRC(7c4fad5c) SHA1(fb733ac979092a6fc278836b82d8ed3fae7a20d9) )
	ROM_LOAD( "dommy.e21",  0xe000, 0x2000, CRC(cd1a4d55) SHA1(f7f4f5ef2e89519652e8401e75dc4e2b8edf4bae) )

	ROM_REGION( 0x6000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "dommy.e50",  0x0000, 0x2000, CRC(5e9db0a4) SHA1(82e60d6b65a4d901102d7e195e66b278f18586f7) )
	ROM_LOAD( "dommy.e40",  0x2000, 0x2000, CRC(4d1c36fb) SHA1(5904421e8e2f727dd6292045871429a1373085e9) )
	ROM_LOAD( "dommy.e30",  0x4000, 0x2000, CRC(4e68bb12) SHA1(de26d278e43882deffad4d5b19d785f8824cf05a) )

	ROM_REGION( 0x0040, REGION_PROMS, 0 ) /* palette decoding is probably wrong */
	ROM_LOAD( "dommy.e70",  0x0018, 0x0008, CRC(50c1d86e) SHA1(990a87a7f7e6a2af67dc6890e2326c7403e46520) )	/* palette */
	ROM_CONTINUE(			  0x0000, 0x0018 )
	ROM_LOAD( "dommy.e60",  0x0020, 0x0020, CRC(24da2b63) SHA1(4db7e1ff1b9fd5ae4098cd7ca66cf1fa2574501a) )	/* unknown */
ROM_END

ROM_START( scregg )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "scregg.e14",   0x3000, 0x1000, CRC(29226d77) SHA1(e1a329a4452eeb90801d001140ce865bf1ea7716) )
	ROM_LOAD( "scregg.d14",   0x4000, 0x1000, CRC(eb143880) SHA1(73b3ca6e0d72cd0db951ae9ed1552cf8b7d91e68) )
	ROM_LOAD( "scregg.c14",   0x5000, 0x1000, CRC(4455f262) SHA1(fc7b2d9094fa5e25c1bf4b68386f640f4502e0c0) )
	ROM_LOAD( "scregg.b14",   0x6000, 0x1000, CRC(044ac5d2) SHA1(f2d2fe2236de1b3b2614cc95f61a90571638cd69) )
	ROM_LOAD( "scregg.a14",   0x7000, 0x1000, CRC(b5a0814a) SHA1(192cdc506fb0bbfed8ae687f2699397ace3bef30) )
	ROM_RELOAD(               0xf000, 0x1000 )        /* for reset/interrupt vectors */

	ROM_REGION( 0x6000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "scregg.j12",   0x0000, 0x1000, CRC(a485c10c) SHA1(88edd35479ceb58244f644a7e0520d225df3bf65) )
	ROM_LOAD( "scregg.j10",   0x1000, 0x1000, CRC(1fd4e539) SHA1(3067bbd9493614e80d8d3982fe80ef25688d256c) )
	ROM_LOAD( "scregg.h12",   0x2000, 0x1000, CRC(8454f4b2) SHA1(6a8d257a3fec901453c7216ad894badf96188ebf) )
	ROM_LOAD( "scregg.h10",   0x3000, 0x1000, CRC(72bd89ee) SHA1(2e38c27b546eeef0fe42340777c8687f4c65ee97) )
	ROM_LOAD( "scregg.g12",   0x4000, 0x1000, CRC(ff3c2894) SHA1(0da866db6a79f658de3efc609b9ca8520b4d22d0) )
	ROM_LOAD( "scregg.g10",   0x5000, 0x1000, CRC(9c20214a) SHA1(e01b72501a01ffc0370cf19c9a379a54800cccc6) )

	ROM_REGION( 0x0040, REGION_PROMS, 0 )
	ROM_LOAD( "screggco.c6",  0x0000, 0x0020, CRC(ff23bdd6) SHA1(d09738915da456449bb4e8d9eefb8e6378f0edea) )	/* palette */
	ROM_LOAD( "screggco.b4",  0x0020, 0x0020, CRC(7cc4824b) SHA1(2a283fc17fac32e63385948bfe180d05f1fb8727) )	/* unknown */
ROM_END

ROM_START( eggs )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "e14.bin",      0x3000, 0x1000, CRC(4e216f9d) SHA1(7b9d984481c8227e417dae4a1adbb5dec5f959b7) )
	ROM_LOAD( "d14.bin",      0x4000, 0x1000, CRC(4edb267f) SHA1(f5d1a79b13d6fbb92561b4e4cfb78465114497d1) )
	ROM_LOAD( "c14.bin",      0x5000, 0x1000, CRC(15a5c48c) SHA1(70141c739a8c019554a6c5257ad12606a1542b1f) )
	ROM_LOAD( "b14.bin",      0x6000, 0x1000, CRC(5c11c00e) SHA1(4a9295086bf935a1c9b1b01f83d1ff6242d74907) )
	ROM_LOAD( "a14.bin",      0x7000, 0x1000, CRC(953faf07) SHA1(ee3181e9ee664053d6e6899fe38e136a9ea6abe1) )
	ROM_RELOAD(               0xf000, 0x1000 )   /* for reset/interrupt vectors */

	ROM_REGION( 0x6000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "j12.bin",      0x0000, 0x1000, CRC(ce4a2e46) SHA1(6b31c481ca038834ae295d015054f852baa6330f) )
	ROM_LOAD( "j10.bin",      0x1000, 0x1000, CRC(a1bcaffc) SHA1(74f6df3136826822bbc22b027700fb3ddfceaa97) )
	ROM_LOAD( "h12.bin",      0x2000, 0x1000, CRC(9562836d) SHA1(c5d5d6ceede6105975c87ff8e1f7e5312b992b92) )
	ROM_LOAD( "h10.bin",      0x3000, 0x1000, CRC(3cfb3a8e) SHA1(e60c9da1a7841c3bb5351a109d99c8df34747212) )
	ROM_LOAD( "g12.bin",      0x4000, 0x1000, CRC(679f8af7) SHA1(f69302ff0125d96fbfdd914d7ecefd7130a24616) )
	ROM_LOAD( "g10.bin",      0x5000, 0x1000, CRC(5b58d3b5) SHA1(f138b7294dd20d050bb8a2191e87b0c3815f6148) )

	ROM_REGION( 0x0040, REGION_PROMS, 0 )
	ROM_LOAD( "eggs.c6",      0x0000, 0x0020, CRC(e8408c81) SHA1(549b9948a4a73e7a704731b942565183cef05d52) )	/* palette */
	ROM_LOAD( "screggco.b4",  0x0020, 0x0020, CRC(7cc4824b) SHA1(2a283fc17fac32e63385948bfe180d05f1fb8727) )	/* unknown */
ROM_END

// rockduck - check gfx roms (planes) order

ROM_START( rockduck )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "rde.bin",	0x4000, 0x2000, CRC(56e2a030) SHA1(f03cca53ac30f1c4ec45afbe58c231673739e425) )
	ROM_COPY( REGION_CPU1,	0x5000, 0x3000, 0x1000 ) // rgn,srcoffset,offset,length.
	ROM_LOAD( "rdc.bin",	0x6000, 0x2000, CRC(482d9a0c) SHA1(2838cbcd35edaf19848fcf1588ec3a35adf5b179) )
	ROM_COPY( REGION_CPU1,	0x7000, 0x5000, 0x1000 ) // rgn,srcoffset,offset,length.
	ROM_LOAD( "rdb.bin",	0x8000, 0x2000, CRC(974626f2) SHA1(cfd767947df9aa99b22afbc0a83afd3f92e7d903) )
	ROM_RELOAD(				0xe000, 0x2000 )	// for vectors/pointers
	ROM_COPY( REGION_CPU1,	0x9000, 0x7000, 0x1000 ) // rgn,srcoffset,offset,length.

//  ROM_LOAD( "b.bin",  0x8000, 0x2000, CRC(637fbb50) SHA1(b31799f9cc6aefd9f4b39cc1afb1ca00d9200efb) ) // alternate rom, bad dump
//  this rom is a bad dump of rdb.bin with only 1 bit different.
//  (bit 5 is on at offset $1629).

	ROM_REGION( 0x8000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "rd3.rdg",	0x0000, 0x2000, CRC(8a3f1e53) SHA1(398bbbab314e4ea87cc5f5978c7e806818398d02) ) // not scrambled
	ROM_LOAD( "rd2.rdh",	0x2000, 0x2000, CRC(e94e673e) SHA1(0adf01d35879b9dd355d0c53a51b5f416f22d7b2) )
	ROM_LOAD( "rd1.rdj",	0x4000, 0x2000, CRC(654afff2) SHA1(f1e21447f0a2ac23cd64cf1f6f315937787b6377) )

	ROM_REGION( 0x0040, REGION_PROMS, 0 )
	/* no proms were in the rock duck set and no PCB available, use eggs palette for now, although its probably wrong */
	ROM_LOAD( "eggs.c6",      0x0000, 0x0020, BAD_DUMP CRC(e8408c81) SHA1(549b9948a4a73e7a704731b942565183cef05d52) )	/* palette */
//  ROM_LOAD( "screggco.b4",  0x0020, 0x0020, BAD_DUMP CRC(7cc4824b) SHA1(2a283fc17fac32e63385948bfe180d05f1fb8727) )   /* unknown */
ROM_END


static DRIVER_INIT( rockduck )
{
	// rd2.rdh and rd1.rdj are bitswapped, but not rd3.rdg .. are they really from the same board?
	int x;
	UINT8 *src = memory_region( machine, REGION_GFX1 );

	for (x=0x2000;x<0x6000;x++)
	{
		src[x] = BITSWAP8(src[x],2,0,3,6,1,4,7,5);

	}
}


GAME( 198?, dommy,    0,        dommy,  scregg, 0, ROT270, "Technos", "Dommy", 0 )
GAME( 1983, scregg,   0,        scregg, scregg, 0, ROT270, "Technos", "Scrambled Egg", 0 )
GAME( 1983, eggs,     scregg,   scregg, scregg, 0, ROT270, "[Technos] Universal USA", "Eggs", 0 )
GAME( 1983, rockduck, 0,        scregg, scregg, rockduck, ROT270, "Datel SAS", "Rock Duck (prototype?)", GAME_WRONG_COLORS )
