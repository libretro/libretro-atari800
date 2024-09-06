cd /developer
make -f Makefile.libretro platform=wiiu -j3 DEBUG_ALLOW_DIRTY_SUBMODULES=1
make -f Makefile platform=wiiu -j3 DEBUG_ALLOW_DIRTY_SUBMODULES=1
make clean
make -f Makefile platform=wiiu -j3 DEBUG_ALLOW_DIRTY_SUBMODULES=1
exit
