all : embeddedcc

CFLAGS:=-Os -I. -I.. -flto -I../embeddedcommon 

LDFLAGS:= -s -Wl,--relax -Wl,-Map=test.map  -Wl,--gc-sections -ffunction-sections -fdata-sections -Wl,--strip-all -fno-asynchronous-unwind-tables


embeddedcc :  ../embeddedcommon/embeddednf.c ../embeddedcommon/DFT32.c embeddedcc.c  ../embeddedcommon/embeddedout.c
	gcc -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean :
	rm -rf embeddedcc *~ dummy_leds
