-- license:BSD-3-Clause
-- copyright-holders:MAMEdev Team

---------------------------------------------------------------------------
--
--   emu.lua
--
--   Rules for building emu cores
--
---------------------------------------------------------------------------

project ("emu")
targetsubdir(_OPTIONS["target"] .."_" .. _OPTIONS["subtarget"])
uuid ("e6fa15e4-a354-4526-acef-13c8e80fcacf")
kind (LIBTYPE)

addprojectflags()
precompiledheaders()
options {
	"ArchiveSplit",
}
includedirs {
	MAME_DIR .. "src/osd",
	MAME_DIR .. "src/emu",
	MAME_DIR .. "src/devices", -- till deps are fixed
	MAME_DIR .. "src/lib",
	MAME_DIR .. "src/lib/util",
	MAME_DIR .. "3rdparty",
	GEN_DIR  .. "emu",
	GEN_DIR  .. "emu/layout",
}
if _OPTIONS["with-bundled-expat"] then
	includedirs {
		MAME_DIR .. "3rdparty/expat/lib",
	}
end
if _OPTIONS["with-bundled-lua"] then
	includedirs {
		MAME_DIR .. "3rdparty/lua/src",
	}
end

if (_OPTIONS["targetos"] == "windows") then
	defines {
		"UI_WINDOWS",
	}
end

if (_OPTIONS["osd"] == "sdl") then
	defines {
		"UI_SDL",
	}
end

files {
	MAME_DIR .. "src/emu/emu.h",
	MAME_DIR .. "src/emu/gamedrv.h",
	MAME_DIR .. "src/emu/hashfile.cpp",
	MAME_DIR .. "src/emu/hashfile.h",
	MAME_DIR .. "src/emu/hiscore.cpp",
	MAME_DIR .. "src/emu/hiscore.h",
	MAME_DIR .. "src/emu/addrmap.cpp",
	MAME_DIR .. "src/emu/addrmap.h",
	MAME_DIR .. "src/emu/attotime.cpp",
	MAME_DIR .. "src/emu/attotime.h",
	MAME_DIR .. "src/emu/audit.cpp",
	MAME_DIR .. "src/emu/audit.h",
	MAME_DIR .. "src/emu/bookkeeping.cpp",
	MAME_DIR .. "src/emu/bookkeeping.h",
	MAME_DIR .. "src/emu/cheat.cpp",
	MAME_DIR .. "src/emu/cheat.h",
	MAME_DIR .. "src/emu/clifront.cpp",
	MAME_DIR .. "src/emu/clifront.h",
	MAME_DIR .. "src/emu/cliopts.cpp",
	MAME_DIR .. "src/emu/cliopts.h",
	MAME_DIR .. "src/emu/config.cpp",
	MAME_DIR .. "src/emu/config.h",
	MAME_DIR .. "src/emu/crsshair.cpp",
	MAME_DIR .. "src/emu/crsshair.h",
	MAME_DIR .. "src/emu/debugger.cpp",
	MAME_DIR .. "src/emu/debugger.h",
	MAME_DIR .. "src/emu/devdelegate.cpp",
	MAME_DIR .. "src/emu/devdelegate.h",
	MAME_DIR .. "src/emu/devcb.cpp",
	MAME_DIR .. "src/emu/devcb.h",
	MAME_DIR .. "src/emu/devcpu.cpp",
	MAME_DIR .. "src/emu/devcpu.h",
	MAME_DIR .. "src/emu/devfind.cpp",
	MAME_DIR .. "src/emu/devfind.h",
	MAME_DIR .. "src/emu/device.cpp",
	MAME_DIR .. "src/emu/device.h",
	MAME_DIR .. "src/emu/device.ipp",
	MAME_DIR .. "src/emu/didisasm.cpp",
	MAME_DIR .. "src/emu/didisasm.h",
	MAME_DIR .. "src/emu/diexec.cpp",
	MAME_DIR .. "src/emu/diexec.h",
	MAME_DIR .. "src/emu/digfx.cpp",
	MAME_DIR .. "src/emu/digfx.h",
	MAME_DIR .. "src/emu/diimage.cpp",
	MAME_DIR .. "src/emu/diimage.h",
	MAME_DIR .. "src/emu/dimemory.cpp",
	MAME_DIR .. "src/emu/dimemory.h",
	MAME_DIR .. "src/emu/dinetwork.cpp",
	MAME_DIR .. "src/emu/dinetwork.h",
	MAME_DIR .. "src/emu/dinvram.cpp",
	MAME_DIR .. "src/emu/dinvram.h",
	MAME_DIR .. "src/emu/dioutput.cpp",
	MAME_DIR .. "src/emu/dioutput.h",
	MAME_DIR .. "src/emu/dipty.cpp",
	MAME_DIR .. "src/emu/dipty.h",
	MAME_DIR .. "src/emu/dirtc.cpp",
	MAME_DIR .. "src/emu/dirtc.h",
	MAME_DIR .. "src/emu/diserial.cpp",
	MAME_DIR .. "src/emu/diserial.h",
	MAME_DIR .. "src/emu/dislot.cpp",
	MAME_DIR .. "src/emu/dislot.h",
	MAME_DIR .. "src/emu/disound.cpp",
	MAME_DIR .. "src/emu/disound.h",
	MAME_DIR .. "src/emu/dispatch.cpp",
	MAME_DIR .. "src/emu/dispatch.h",
	MAME_DIR .. "src/emu/distate.cpp",
	MAME_DIR .. "src/emu/distate.h",
	MAME_DIR .. "src/emu/divideo.cpp",
	MAME_DIR .. "src/emu/divideo.h",
	MAME_DIR .. "src/emu/divtlb.cpp",
	MAME_DIR .. "src/emu/divtlb.h",
	MAME_DIR .. "src/emu/drawgfx.cpp",
	MAME_DIR .. "src/emu/drawgfx.h",
	MAME_DIR .. "src/emu/drawgfxm.h",
	MAME_DIR .. "src/emu/driver.cpp",
	MAME_DIR .. "src/emu/driver.h",
	MAME_DIR .. "src/emu/drivenum.cpp",
	MAME_DIR .. "src/emu/drivenum.h",
	MAME_DIR .. "src/emu/emualloc.cpp",
	MAME_DIR .. "src/emu/emualloc.h",
	MAME_DIR .. "src/emu/emucore.cpp",
	MAME_DIR .. "src/emu/emucore.h",
	MAME_DIR .. "src/emu/emuopts.cpp",
	MAME_DIR .. "src/emu/emuopts.h",
	MAME_DIR .. "src/emu/emupal.cpp",
	MAME_DIR .. "src/emu/emupal.h",
	MAME_DIR .. "src/emu/faststart.cpp",
	MAME_DIR .. "src/emu/faststart.h",
	MAME_DIR .. "src/emu/fileio.cpp",
	MAME_DIR .. "src/emu/fileio.h",
	MAME_DIR .. "src/emu/hash.cpp",
	MAME_DIR .. "src/emu/hash.h",
	MAME_DIR .. "src/emu/image.cpp",
	MAME_DIR .. "src/emu/image.h",
	MAME_DIR .. "src/emu/info.cpp",
	MAME_DIR .. "src/emu/info.h",
	MAME_DIR .. "src/emu/input.cpp",
	MAME_DIR .. "src/emu/input.h",
	MAME_DIR .. "src/emu/ioport.cpp",
	MAME_DIR .. "src/emu/ioport.h",
	MAME_DIR .. "src/emu/inpttype.h",
	MAME_DIR .. "src/emu/luaengine.cpp",
	MAME_DIR .. "src/emu/luaengine.h",
	MAME_DIR .. "src/emu/language.cpp",
	MAME_DIR .. "src/emu/language.h",
	MAME_DIR .. "src/emu/mame.cpp",
	MAME_DIR .. "src/emu/mame.h",
	MAME_DIR .. "src/emu/machine.cpp",
	MAME_DIR .. "src/emu/machine.h",
	MAME_DIR .. "src/emu/machine.ipp",
	MAME_DIR .. "src/emu/mconfig.cpp",
	MAME_DIR .. "src/emu/mconfig.h",
	MAME_DIR .. "src/emu/memarray.cpp",
	MAME_DIR .. "src/emu/memarray.h",
	MAME_DIR .. "src/emu/memory.cpp",
	MAME_DIR .. "src/emu/memory.h",
	MAME_DIR .. "src/emu/network.cpp",
	MAME_DIR .. "src/emu/network.h",
	MAME_DIR .. "src/emu/parameters.cpp",
	MAME_DIR .. "src/emu/parameters.h",
	MAME_DIR .. "src/emu/output.cpp",
	MAME_DIR .. "src/emu/output.h",
	MAME_DIR .. "src/emu/render.cpp",
	MAME_DIR .. "src/emu/render.h",
	MAME_DIR .. "src/emu/rendfont.cpp",
	MAME_DIR .. "src/emu/rendfont.h",
	MAME_DIR .. "src/emu/rendlay.cpp",
	MAME_DIR .. "src/emu/rendlay.h",
	MAME_DIR .. "src/emu/rendutil.cpp",
	MAME_DIR .. "src/emu/rendutil.h",
	MAME_DIR .. "src/emu/romload.cpp",
	MAME_DIR .. "src/emu/romload.h",
	MAME_DIR .. "src/emu/save.cpp",
	MAME_DIR .. "src/emu/save.h",
	MAME_DIR .. "src/emu/schedule.cpp",
	MAME_DIR .. "src/emu/schedule.h",
	MAME_DIR .. "src/emu/screen.cpp",
	MAME_DIR .. "src/emu/screen.h",
	MAME_DIR .. "src/emu/softlist.cpp",
	MAME_DIR .. "src/emu/softlist.h",
	MAME_DIR .. "src/emu/sound.cpp",
	MAME_DIR .. "src/emu/sound.h",
	MAME_DIR .. "src/emu/speaker.cpp",
	MAME_DIR .. "src/emu/speaker.h",
	MAME_DIR .. "src/emu/sprite.cpp",
	MAME_DIR .. "src/emu/sprite.h",
	MAME_DIR .. "src/emu/tilemap.cpp",
	MAME_DIR .. "src/emu/tilemap.h",
	MAME_DIR .. "src/emu/timer.cpp",
	MAME_DIR .. "src/emu/timer.h",
	MAME_DIR .. "src/emu/uiinput.cpp",
	MAME_DIR .. "src/emu/uiinput.h",
	MAME_DIR .. "src/emu/ui/ui.cpp",
	MAME_DIR .. "src/emu/ui/ui.h",
	MAME_DIR .. "src/emu/ui/devctrl.h",
	MAME_DIR .. "src/emu/ui/menu.cpp",
	MAME_DIR .. "src/emu/ui/menu.h",
	MAME_DIR .. "src/emu/ui/submenu.cpp",
	MAME_DIR .. "src/emu/ui/submenu.h",
	MAME_DIR .. "src/emu/ui/mainmenu.cpp",
	MAME_DIR .. "src/emu/ui/mainmenu.h",
	MAME_DIR .. "src/emu/ui/miscmenu.cpp",
	MAME_DIR .. "src/emu/ui/miscmenu.h",
	MAME_DIR .. "src/emu/ui/barcode.cpp",
	MAME_DIR .. "src/emu/ui/barcode.h",
	MAME_DIR .. "src/emu/ui/cheatopt.cpp",
	MAME_DIR .. "src/emu/ui/cheatopt.h",
	MAME_DIR .. "src/emu/ui/devopt.cpp",
	MAME_DIR .. "src/emu/ui/devopt.h",
	MAME_DIR .. "src/emu/ui/filemngr.cpp",
	MAME_DIR .. "src/emu/ui/filemngr.h",
	MAME_DIR .. "src/emu/ui/filesel.cpp",
	MAME_DIR .. "src/emu/ui/filesel.h",
	MAME_DIR .. "src/emu/ui/imgcntrl.cpp",
	MAME_DIR .. "src/emu/ui/imgcntrl.h",
	MAME_DIR .. "src/emu/ui/info.cpp",
	MAME_DIR .. "src/emu/ui/info.h",
	MAME_DIR .. "src/emu/ui/info_pty.cpp",
	MAME_DIR .. "src/emu/ui/info_pty.h",
	MAME_DIR .. "src/emu/ui/inputmap.cpp",
	MAME_DIR .. "src/emu/ui/inputmap.h",
	MAME_DIR .. "src/emu/ui/sliders.cpp",
	MAME_DIR .. "src/emu/ui/sliders.h",
	MAME_DIR .. "src/emu/ui/slotopt.cpp",
	MAME_DIR .. "src/emu/ui/slotopt.h",
	MAME_DIR .. "src/emu/ui/swlist.cpp",
	MAME_DIR .. "src/emu/ui/swlist.h",
	MAME_DIR .. "src/emu/ui/tapectrl.cpp",
	MAME_DIR .. "src/emu/ui/tapectrl.h",
	MAME_DIR .. "src/emu/ui/videoopt.cpp",
	MAME_DIR .. "src/emu/ui/videoopt.h",
	MAME_DIR .. "src/emu/ui/viewgfx.cpp",
	MAME_DIR .. "src/emu/ui/viewgfx.h",
	MAME_DIR .. "src/emu/ui/auditmenu.cpp",
	MAME_DIR .. "src/emu/ui/auditmenu.h",
	MAME_DIR .. "src/emu/ui/cmddata.h",
	MAME_DIR .. "src/emu/ui/cmdrender.h",
	MAME_DIR .. "src/emu/ui/ctrlmenu.cpp",
	MAME_DIR .. "src/emu/ui/ctrlmenu.h",
	MAME_DIR .. "src/emu/ui/custmenu.cpp",
	MAME_DIR .. "src/emu/ui/custmenu.h",
	MAME_DIR .. "src/emu/ui/custui.cpp",
	MAME_DIR .. "src/emu/ui/custui.h",
	MAME_DIR .. "src/emu/ui/datfile.cpp",
	MAME_DIR .. "src/emu/ui/datfile.h",
	MAME_DIR .. "src/emu/ui/datmenu.cpp",
	MAME_DIR .. "src/emu/ui/datmenu.h",
	MAME_DIR .. "src/emu/ui/defimg.h",
	MAME_DIR .. "src/emu/ui/dirmenu.cpp",
	MAME_DIR .. "src/emu/ui/dirmenu.h",
	MAME_DIR .. "src/emu/ui/dsplmenu.cpp",
	MAME_DIR .. "src/emu/ui/dsplmenu.h",
	MAME_DIR .. "src/emu/ui/icorender.h",
	MAME_DIR .. "src/emu/ui/inifile.cpp",
	MAME_DIR .. "src/emu/ui/inifile.h",
	MAME_DIR .. "src/emu/ui/miscmenu.cpp",
	MAME_DIR .. "src/emu/ui/miscmenu.h",
	MAME_DIR .. "src/emu/ui/moptions.cpp",
	MAME_DIR .. "src/emu/ui/moptions.h",
	MAME_DIR .. "src/emu/ui/optsmenu.cpp",
	MAME_DIR .. "src/emu/ui/optsmenu.h",
	MAME_DIR .. "src/emu/ui/selector.cpp",
	MAME_DIR .. "src/emu/ui/selector.h",
	MAME_DIR .. "src/emu/ui/selgame.cpp",
	MAME_DIR .. "src/emu/ui/selgame.h",
	MAME_DIR .. "src/emu/ui/simpleselgame.cpp",
	MAME_DIR .. "src/emu/ui/simpleselgame.h",
	MAME_DIR .. "src/emu/ui/selsoft.cpp",
	MAME_DIR .. "src/emu/ui/selsoft.h",
	MAME_DIR .. "src/emu/ui/sndmenu.cpp",
	MAME_DIR .. "src/emu/ui/sndmenu.h",
	MAME_DIR .. "src/emu/ui/starimg.h",
	MAME_DIR .. "src/emu/ui/toolbar.h",
	MAME_DIR .. "src/emu/ui/utils.cpp",
	MAME_DIR .. "src/emu/ui/utils.h",	
	MAME_DIR .. "src/emu/validity.cpp",
	MAME_DIR .. "src/emu/validity.h",
	MAME_DIR .. "src/emu/video.cpp",
	MAME_DIR .. "src/emu/video.h",
	MAME_DIR .. "src/emu/rendersw.inc",
	MAME_DIR .. "src/emu/debug/debugcmd.cpp",
	MAME_DIR .. "src/emu/debug/debugcmd.h",
	MAME_DIR .. "src/emu/debug/debugcon.cpp",
	MAME_DIR .. "src/emu/debug/debugcon.h",
	MAME_DIR .. "src/emu/debug/debugcpu.cpp",
	MAME_DIR .. "src/emu/debug/debugcpu.h",
	MAME_DIR .. "src/emu/debug/debughlp.cpp",
	MAME_DIR .. "src/emu/debug/debughlp.h",
	MAME_DIR .. "src/emu/debug/debugvw.cpp",
	MAME_DIR .. "src/emu/debug/debugvw.h",
	MAME_DIR .. "src/emu/debug/dvdisasm.cpp",
	MAME_DIR .. "src/emu/debug/dvdisasm.h",
	MAME_DIR .. "src/emu/debug/dvmemory.cpp",
	MAME_DIR .. "src/emu/debug/dvmemory.h",
	MAME_DIR .. "src/emu/debug/dvbpoints.cpp",
	MAME_DIR .. "src/emu/debug/dvbpoints.h",
	MAME_DIR .. "src/emu/debug/dvwpoints.cpp",
	MAME_DIR .. "src/emu/debug/dvwpoints.h",
	MAME_DIR .. "src/emu/debug/dvstate.cpp",
	MAME_DIR .. "src/emu/debug/dvstate.h",
	MAME_DIR .. "src/emu/debug/dvtext.cpp",
	MAME_DIR .. "src/emu/debug/dvtext.h",
	MAME_DIR .. "src/emu/debug/express.cpp",
	MAME_DIR .. "src/emu/debug/express.h",
	MAME_DIR .. "src/emu/debug/textbuf.cpp",
	MAME_DIR .. "src/emu/debug/textbuf.h",
	MAME_DIR .. "src/emu/profiler.cpp",
	MAME_DIR .. "src/emu/profiler.h",
	MAME_DIR .. "src/emu/sound/filter.cpp",
	MAME_DIR .. "src/emu/sound/filter.h",
	MAME_DIR .. "src/devices/sound/flt_vol.cpp",
	MAME_DIR .. "src/devices/sound/flt_vol.h",
	MAME_DIR .. "src/devices/sound/flt_rc.cpp",
	MAME_DIR .. "src/devices/sound/flt_rc.h",
	MAME_DIR .. "src/emu/sound/wavwrite.cpp",
	MAME_DIR .. "src/emu/sound/wavwrite.h",
	MAME_DIR .. "src/devices/sound/samples.cpp",
	MAME_DIR .. "src/devices/sound/samples.h",
	MAME_DIR .. "src/emu/drivers/empty.cpp",
	MAME_DIR .. "src/emu/drivers/testcpu.cpp",
	MAME_DIR .. "src/emu/drivers/xtal.h",
	MAME_DIR .. "src/devices/machine/bcreader.cpp",
	MAME_DIR .. "src/devices/machine/bcreader.h",
	MAME_DIR .. "src/devices/machine/buffer.cpp",
	MAME_DIR .. "src/devices/machine/buffer.h",
	MAME_DIR .. "src/devices/machine/clock.cpp",
	MAME_DIR .. "src/devices/machine/clock.h",
	MAME_DIR .. "src/devices/machine/keyboard.cpp",
	MAME_DIR .. "src/devices/machine/keyboard.h",
	MAME_DIR .. "src/devices/machine/laserdsc.cpp",
	MAME_DIR .. "src/devices/machine/laserdsc.h",
	MAME_DIR .. "src/devices/machine/latch.cpp",
	MAME_DIR .. "src/devices/machine/latch.h",
	MAME_DIR .. "src/devices/machine/nvram.cpp",
	MAME_DIR .. "src/devices/machine/nvram.h",
	MAME_DIR .. "src/devices/machine/ram.cpp",
	MAME_DIR .. "src/devices/machine/ram.h",
	MAME_DIR .. "src/devices/machine/legscsi.cpp",
	MAME_DIR .. "src/devices/machine/legscsi.h",
	MAME_DIR .. "src/devices/machine/terminal.cpp",
	MAME_DIR .. "src/devices/machine/terminal.h",
	MAME_DIR .. "src/devices/imagedev/bitbngr.cpp",
	MAME_DIR .. "src/devices/imagedev/bitbngr.h",
	MAME_DIR .. "src/devices/imagedev/cassette.cpp",
	MAME_DIR .. "src/devices/imagedev/cassette.h",
	MAME_DIR .. "src/devices/imagedev/chd_cd.cpp",
	MAME_DIR .. "src/devices/imagedev/chd_cd.h",
	MAME_DIR .. "src/devices/imagedev/diablo.cpp",
	MAME_DIR .. "src/devices/imagedev/diablo.h",
	MAME_DIR .. "src/devices/imagedev/flopdrv.cpp",
	MAME_DIR .. "src/devices/imagedev/flopdrv.h",
	MAME_DIR .. "src/devices/imagedev/floppy.cpp",
	MAME_DIR .. "src/devices/imagedev/floppy.h",
	MAME_DIR .. "src/devices/imagedev/harddriv.cpp",
	MAME_DIR .. "src/devices/imagedev/harddriv.h",
	MAME_DIR .. "src/devices/imagedev/mfmhd.cpp",
	MAME_DIR .. "src/devices/imagedev/mfmhd.h",
	MAME_DIR .. "src/devices/imagedev/midiin.cpp",
	MAME_DIR .. "src/devices/imagedev/midiin.h",
	MAME_DIR .. "src/devices/imagedev/midiout.cpp",
	MAME_DIR .. "src/devices/imagedev/midiout.h",
	MAME_DIR .. "src/devices/imagedev/printer.cpp",
	MAME_DIR .. "src/devices/imagedev/printer.h",
	MAME_DIR .. "src/devices/imagedev/snapquik.cpp",
	MAME_DIR .. "src/devices/imagedev/snapquik.h",
	MAME_DIR .. "src/emu/video/generic.cpp",
	MAME_DIR .. "src/emu/video/generic.h",
	MAME_DIR .. "src/emu/video/resnet.cpp",
	MAME_DIR .. "src/emu/video/resnet.h",
	MAME_DIR .. "src/emu/video/rgbutil.h",
	MAME_DIR .. "src/emu/video/rgbgen.cpp",
	MAME_DIR .. "src/emu/video/rgbgen.h",
	MAME_DIR .. "src/emu/video/rgbsse.cpp",
	MAME_DIR .. "src/emu/video/rgbsse.h",
	MAME_DIR .. "src/emu/video/rgbvmx.cpp",
	MAME_DIR .. "src/emu/video/rgbvmx.h",
	MAME_DIR .. "src/emu/video/vector.cpp",
	MAME_DIR .. "src/emu/video/vector.h",
	MAME_DIR .. "src/devices/video/poly.h",
}

dependency {
	--------------------------------------------------
	-- additional dependencies
	--------------------------------------------------
	{ MAME_DIR .. "src/emu/rendfont.cpp", GEN_DIR .. "emu/uismall.fh" },
	{ MAME_DIR .. "src/emu/rendfont.cpp", GEN_DIR .. "emu/ui/uicmd14.fh" },
	-------------------------------------------------
	-- core layouts
	--------------------------------------------------
	{ MAME_DIR .. "src/emu/rendlay.cpp", GEN_DIR .. "emu/layout/dualhovu.lh" },
	{ MAME_DIR .. "src/emu/rendlay.cpp", GEN_DIR .. "emu/layout/dualhsxs.lh" },
	{ MAME_DIR .. "src/emu/rendlay.cpp", GEN_DIR .. "emu/layout/dualhuov.lh" },
	{ MAME_DIR .. "src/emu/rendlay.cpp", GEN_DIR .. "emu/layout/horizont.lh" },
	{ MAME_DIR .. "src/emu/rendlay.cpp", GEN_DIR .. "emu/layout/triphsxs.lh" },
	{ MAME_DIR .. "src/emu/rendlay.cpp", GEN_DIR .. "emu/layout/quadhsxs.lh" },
	{ MAME_DIR .. "src/emu/rendlay.cpp", GEN_DIR .. "emu/layout/vertical.lh" },
	{ MAME_DIR .. "src/emu/rendlay.cpp", GEN_DIR .. "emu/layout/lcd.lh" },
	{ MAME_DIR .. "src/emu/rendlay.cpp", GEN_DIR .. "emu/layout/lcd_rot.lh" },
	{ MAME_DIR .. "src/emu/rendlay.cpp", GEN_DIR .. "emu/layout/noscreens.lh" },

	{ MAME_DIR .. "src/emu/video.cpp",   GEN_DIR .. "emu/layout/snap.lh" },

}

custombuildtask {
	{ MAME_DIR .. "scripts/font/NotoSans-Bold.bdc", GEN_DIR .. "emu/uismall.fh",     {  MAME_DIR .. "scripts/build/file2str.py" }, {"@echo Converting NotoSans-Bold.bdc...", PYTHON .. " $(1) $(<) $(@) font_uismall UINT8" }},
	{ MAME_DIR .. "src/emu/ui/uicmd14.png"        , GEN_DIR .. "emu/ui/uicmd14.fh",  {  MAME_DIR.. "scripts/build/png2bdc.py",  MAME_DIR .. "scripts/build/file2str.py" }, {"@echo Converting uicmd14.png...", PYTHON .. " $(1) $(<) temp_cmd.bdc", PYTHON .. " $(2) temp_cmd.bdc $(@) font_uicmd14 UINT8" }},

	layoutbuildtask("emu/layout", "dualhovu"),
	layoutbuildtask("emu/layout", "dualhsxs"),
	layoutbuildtask("emu/layout", "dualhuov"),
	layoutbuildtask("emu/layout", "horizont"),
	layoutbuildtask("emu/layout", "triphsxs"),
	layoutbuildtask("emu/layout", "quadhsxs"),
	layoutbuildtask("emu/layout", "vertical"),
	layoutbuildtask("emu/layout", "lcd"),
	layoutbuildtask("emu/layout", "lcd_rot"),
	layoutbuildtask("emu/layout", "noscreens"),
	layoutbuildtask("emu/layout", "snap"),
}

project ("precompile")
targetsubdir(_OPTIONS["target"] .."_" .. _OPTIONS["subtarget"])
uuid ("a6fb15d4-b123-4445-acef-13c8e80fcacf")
kind (LIBTYPE)

addprojectflags()
precompiledheaders()

includedirs {
	MAME_DIR .. "src/osd",
	MAME_DIR .. "src/emu",
	MAME_DIR .. "src/devices", -- till deps are fixed
	MAME_DIR .. "src/lib",
	MAME_DIR .. "src/lib/util",
	MAME_DIR .. "3rdparty",
	GEN_DIR  .. "emu",
	GEN_DIR  .. "emu/layout",
}
files {
	MAME_DIR .. "src/emu/drivers/empty.cpp",
}
dependency {
	{ "$(OBJDIR)/src/emu/drivers/empty.o", "$(GCH)", true  },
}


