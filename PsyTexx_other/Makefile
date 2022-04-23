ifeq ($(MAKECMDGOALS),)
    $(error No target selected. Please type "make palm" or "make linux")
    exit
endif

## ------------------------------------------ ##
## Main target options:                       ##
## ------------------------------------------ ##
		

ifeq ($(MAKECMDGOALS),palm)
    SDK = /sdk-5/
    PSYTEXX_DEFINES = \
	-DPALMOS \
	-DCOLOR8BITS \
	-DNOPSYNTH
    MCC = m68k-palmos-gcc -O2
    CC = arm-elf-gcc -O3
endif
ifeq ($(MAKECMDGOALS),linux)
    # Console libs:
    #LIBS = -lpthread -lrt -lgpm -lstdc++ -lX11
    # X11 (OpenGL) libs:
    #LIBS = -L/usr/X11R6/lib -lpthread -lrt -lstdc++ -lX11 -lGL -lXext
    # X11 libs:
    LIBS = -L/usr/X11R6/lib -lrt -lpthread -lstdc++ -lX11 
    PSYTEXX_DEFINES = \
	-DLINUX \
	-DUSE_NAMES \
  -DDEBUGMODE \
	-DX11 \
	-DCOLOR16BITS
    CC = gcc -g -fpermissive -O3
    CXX = gcc -g -fpermissive -O3
endif

## ------------------------------------------ ##
## Directories:                               ##
## ------------------------------------------ ##

PSY = ../psytexx/
CPSY = $(PSY)code/
MEM = ../memory/
CMEM = $(MEM)code/
XM = ../xm/
CXM = $(XM)code/
SND = ../sound/
CSND = $(SND)code/
CORE = ../core/
CCORE = $(CORE)code/
FS = ../filesystem/
CFS = $(FS)code/
TM = ../time/
CTM = $(TM)code/
DEMO = ../demoengine/
CDEMO = $(DEMO)code/
JPG = $(DEMO)tiny_jpg/
WM = ../window_manager/
CWM = $(WM)code/
ARM = ARMPalmOS/
CARM = $(ARM)code/

## ------------------------------------------ ##
## Object files and includes:                 ##
## ------------------------------------------ ##

DEFINES = $(PSYTEXX_DEFINES) -I../

STD_INCS = $(MEM)memory.h $(CORE)core.h $(CORE)debug.h
XM_STD_INCS = $(XM)xm.h $(STD_INCS)
PSYNTH_STD_INCS = $(XM)psynth/psynth.h $(XM)psynth/psynth_net.h $(STD_INCS)
XM_OBJ = \
    xm_play.o \
    xm_song.o \
    xm_pattern.o \
    xm_instrum.o \
    xm_sample.o \
    xm_tables.o \
    xm_channels.o \
    xm_main.o
PSYNTH_OBJ = \
    psynths.o \
    psynth_net.o
SOUND_OBJ = sound_player.o sound_sndout.o
WINDOW_MANAGER_OBJ = wmanager.o eventloop.o
PSY_WINDOWS_OBJ = \
    win_main.o \
    win_files.o \
    win_list.o \
    win_button.o \
    win_scrollbar.o \
    win_popup.o \
    win_text.o \
    win_dialog.o \
    win_psymenu.o \
    win_psytable.o \
    win_psysamples.o \
    win_psypattern.o \
    win_psypatterntab.o \
    win_psyconfig.o \
    win_psyplaylist.o \
    win_psychannels.o \
    win_psyinsteditor.o \
    win_psysmpeditor.o \
    win_psysmpview.o \
    win_psynet.o
DEMOENGINE_OBJ = demoengine.o jpg.o
OBJECTS = \
    psytexx.o \
    memory.o \
    debug.o \
    v3nus_fs.o \
    timemanager.o \
    $(PSY_WINDOWS_OBJ) \
    $(XM_OBJ) \
    $(PSYNTH_OBJ) \
    $(SOUND_OBJ) \
    $(WINDOW_MANAGER_OBJ) \
    $(DEMOENGINE_OBJ)

ifeq ($(MAKECMDGOALS),palm)
    ARM_PALM_INCS = -IARMPalmOS/ -I$(SDK)include -I$(SDK)include/libraries -I$(SDK)include/Extensions/ExpansionMgr -I$(SDK)include/Dynamic -I$(SDK)include/Core -I$(SDK)include/Core/Hardware -I$(SDK)include/Core/System -I$(SDK)include/Core/System/Unix -I$(SDK)include/Core/ui
    ARM_PALM_FLAGS = -DNATIVE -mcpu=arm9tdmi -nostartfiles -mshort-load-bytes -ffixed-r9 -ffixed-r8 -mpic-register=r10 -msingle-pic-base -fPIC -mfp=3
    DEFINES += $(ARM_PALM_INCS) $(ARM_PALM_FLAGS) $(ARM_PALM_DEFINES)
    ARM_PALM_FILES = $(CARM)palm_functions.cpp $(ARM)PalmTypes.h $(ARM)DebugMgr.h $(ARM)palm_functions.h
    ARM_PALM_OBJECTS = palm_functions.o
    PALM_OBJECTS = $(OBJECTS) $(ARM_PALM_OBJECTS)
endif

## ------------------------------------------ ##
## Main rules for different targets:          ##
## ------------------------------------------ ##

linux: $(OBJECTS)
	$(CC) $(DEFINES) -o psytexx $(OBJECTS) $(LIBS)

palm: $(PALM_OBJECTS)
#Link main code ( with special ARM script - armscript.ld ) :
	$(CC) $(DEFINES) -o main_palm.o $(PALM_OBJECTS) -Xlinker --script=ARMFiles/armscript.ld
#Create BIN files from the program_palm.o :
	arm-elf-objcopy -j .text -O binary main_palm.o armcode
	split -b 64000 armcode armc
	mv armcaa armc0000.bin
	mv armcab armc0001.bin
	mv armcac armc0002.bin
	arm-elf-objcopy -j .got -O binary main_palm.o armg0000.bin
	arm-elf-objcopy -j .alldata -O binary main_palm.o armd0000.bin
	arm-elf-objcopy -j .rel -O binary main_palm.o armr0000.bin
#Create Palm resources ( *.BIN files ) :
	pilrc Resources/resources.rcp
#Build PRC file :
	$(MCC) ARMFiles/arm_starter.cpp -o arm_starter.o
	m68k-palmos-obj-res arm_starter.o
	build-prc -v 1.0 ARMFiles/info.def -o psytexx.prc *.grc *.bin

## ------------------------------------------ ##
## Other rules:                               ##
## ------------------------------------------ ##

psytexx.o: $(CPSY)psytexx.cpp $(STD_INCS)
	$(CC) $(DEFINES) -c $(CPSY)psytexx.cpp

win_main.o: $(CPSY)win_main.cpp $(PSY)win_main.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CPSY)win_main.cpp

win_files.o: $(CPSY)win_files.cpp $(PSY)win_files.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CPSY)win_files.cpp

win_list.o: $(CPSY)win_list.cpp $(PSY)win_list.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CPSY)win_list.cpp

win_button.o: $(CPSY)win_button.cpp $(PSY)win_button.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CPSY)win_button.cpp

win_popup.o: $(CPSY)win_popup.cpp $(PSY)win_popup.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CPSY)win_popup.cpp

win_scrollbar.o: $(CPSY)win_scrollbar.cpp $(PSY)win_scrollbar.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CPSY)win_scrollbar.cpp

win_psymenu.o: $(CPSY)win_psymenu.cpp $(PSY)win_psymenu.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CPSY)win_psymenu.cpp

win_psytable.o: $(CPSY)win_psytable.cpp $(PSY)win_psytable.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CPSY)win_psytable.cpp

win_text.o: $(CPSY)win_text.cpp $(PSY)win_text.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CPSY)win_text.cpp

win_psysamples.o: $(CPSY)win_psysamples.cpp $(PSY)win_psysamples.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CPSY)win_psysamples.cpp

win_psypattern.o: $(CPSY)win_psypattern.cpp $(PSY)win_psypattern.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CPSY)win_psypattern.cpp

win_psypatterntab.o: $(CPSY)win_psypatterntab.cpp $(PSY)win_psypatterntab.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CPSY)win_psypatterntab.cpp

win_dialog.o: $(CPSY)win_dialog.cpp $(PSY)win_dialog.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CPSY)win_dialog.cpp

win_psyconfig.o: $(CPSY)win_psyconfig.cpp $(PSY)win_psyconfig.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CPSY)win_psyconfig.cpp

win_psyplaylist.o: $(CPSY)win_psyplaylist.cpp $(PSY)win_psyplaylist.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CPSY)win_psyplaylist.cpp

win_psychannels.o: $(CPSY)win_psychannels.cpp $(PSY)win_psychannels.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CPSY)win_psychannels.cpp

win_psyinsteditor.o: $(CPSY)win_psyinsteditor.cpp $(PSY)win_psyinsteditor.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CPSY)win_psyinsteditor.cpp

win_psysmpeditor.o: $(CPSY)win_psysmpeditor.cpp $(PSY)win_psysmpeditor.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CPSY)win_psysmpeditor.cpp

win_psysmpview.o: $(CPSY)win_psysmpview.cpp $(PSY)win_psysmpview.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CPSY)win_psysmpview.cpp

win_psynet.o: $(CPSY)win_psynet.cpp $(PSY)win_psynet.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CPSY)win_psynet.cpp

## ------------------------------------------ ##
##                                            ##
## ------------------------------------------ ##

xm_play.o: $(CXM)xm_play.cpp $(XM_STD_INCS)
	$(CC) $(DEFINES) -c $(CXM)xm_play.cpp

xm_song.o: $(CXM)xm_song.cpp $(XM_STD_INCS)
	$(CC) $(DEFINES) -c $(CXM)xm_song.cpp

xm_pattern.o: $(CXM)xm_pattern.cpp $(XM_STD_INCS)
	$(CC) $(DEFINES) -c $(CXM)xm_pattern.cpp

xm_instrum.o: $(CXM)xm_instrum.cpp $(XM_STD_INCS)
	$(CC) $(DEFINES) -c $(CXM)xm_instrum.cpp

xm_sample.o: $(CXM)xm_sample.cpp $(XM_STD_INCS)
	$(CC) $(DEFINES) -c $(CXM)xm_sample.cpp

xm_tables.o: $(CXM)xm_tables.cpp $(XM_STD_INCS)
	$(CC) $(DEFINES) -c $(CXM)xm_tables.cpp

xm_channels.o: $(CXM)xm_channels.cpp $(XM_STD_INCS)
	$(CC) $(DEFINES) -c $(CXM)xm_channels.cpp

xm_main.o: $(CXM)xm_main.cpp $(XM_STD_INCS)
	$(CC) $(DEFINES) -c $(CXM)xm_main.cpp

## ------------------------------------------ ##
##                                            ##
## ------------------------------------------ ##

psynth_net.o: $(XM)psynth/psynth_net.cpp $(PSYNTH_STD_INCS)
	$(CC) $(DEFINES) -c $(XM)psynth/psynth_net.cpp

psynths.o: $(XM)psynth/psynths.cpp $(PSYNTH_STD_INCS)
	$(CC) $(DEFINES) -c $(XM)psynth/psynths.cpp

## ------------------------------------------ ##
##                                            ##
## ------------------------------------------ ##

sound_player.o: $(CSND)sound_player.cpp $(SND)sound.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CSND)sound_player.cpp

sound_sndout.o: $(CSND)sound_sndout.cpp $(SND)sound.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CSND)sound_sndout.cpp

## ------------------------------------------ ##
##                                            ##
## ------------------------------------------ ##

timemanager.o: $(CTM)timemanager.cpp $(TM)timemanager.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CTM)timemanager.cpp

v3nus_fs.o: $(CFS)v3nus_fs.cpp $(FS)v3nus_fs.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CFS)v3nus_fs.cpp

debug.o: $(CCORE)debug.cpp
	$(CC) $(DEFINES) -c $(CCORE)debug.cpp

memory.o: $(CMEM)memory.cpp $(MEM)memory.h $(CORE)core.h
	$(CC) $(DEFINES) -c $(CMEM)memory.cpp

## ------------------------------------------ ##
##                                            ##
## ------------------------------------------ ##

demoengine.o: $(CDEMO)demoengine.cpp $(DEMO)demoengine.h $(CDEMO)demo_effects.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CDEMO)demoengine.cpp

jpg.o: $(JPG)jpg.cpp $(JPG)jpg.h
	$(CC) $(DEFINES) -c $(JPG)jpg.cpp

## ------------------------------------------ ##
##                                            ##
## ------------------------------------------ ##

wmanager.o: $(CWM)wmanager.cpp $(WM)wmanager.h $(WM)struct.h $(CWM)wm_linux.h $(CWM)wm_linuxgraphics.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CWM)wmanager.cpp

eventloop.o: $(CWM)eventloop.cpp $(WM)wmanager.h $(WM)struct.h $(CWM)wm_linux.h $(CWM)wm_linuxgraphics.h $(STD_INCS)
	$(CC) $(DEFINES) -c $(CWM)eventloop.cpp

## ------------------------------------------ ##
##                                            ##
## ------------------------------------------ ##

palm_functions.o: $(ARM_PALM_FILES) 
	$(CC) $(DEFINES) -c $(CARM)palm_functions.cpp

## ------------------------------------------ ##
##                                            ##
## ------------------------------------------ ##

clean:
	rm *.o *.grc *.bin armcode

