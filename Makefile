#############################################################################
# Nick - 01/01/2005 - updated 23/09/2005 - nick@ukfsn.org
# Quake2plus code Makefile
#############################################################################

# Check if V=1 passed on command line
# to show full build commands, else keep it quiet.
# Idea (and how to do it) taken from Linux Kenel 2.6.x Makefile
XBUILD_VERBOSE = 0
ifdef V
  ifeq ("$(origin V)", "command line")
    XBUILD_VERBOSE = $(V)
  endif
endif

# i386 only!!
ARCH=i386
# use ICC?
#CC=icc
# GCC default
CC=gcc
BASE_CFLAGS=-m32 -Dstricmp=strcasecmp -I.

# This is my build_release flags for ICC with a PentuimPro CPU on the server build.
# Refer here for further information:
#    http://www.cbcb.duke.edu/computational/projects/public/icc.html
#CFLAGS_RELEASE=$(BASE_CFLAGS) -fast -O3 -pc32 -march=pentiumii -mcpu=pentiumpro -rcd -w
#CFLAGS_RELEASE=$(BASE_CFLAGS) -O1 -pc32 -mcpu=pentiumpro -rcd -w -parallel -tpp6

# 'make build_release' flags for GCC
# This is my build line with GCC 3.4.3 on PII/PentiumPro CPU
# Refer to GCC architecture optimization documentation.
#CFLAGS_RELEASE=$(BASE_CFLAGS) -Os -mtune=pentium2 -funroll-loops -ffast-math -fsingle-precision-constant

# This is my build line for all quake2 code on my client machine Slackware 10 - Athlon 1.2GHz
#CFLAGS_RELEASE=$(BASE_CFLAGS) -Os -ffast-math -funroll-loops -march=athlon \
#        -falign-jumps=2 -falign-functions=2 -fno-strict-aliasing -mmmx -m3dnow

# Build line for Reiner's setup (GCC 3.1.1)
#CFLAGS_RELEASE=$(BASE_CFLAGS) -O2 -funroll-loops -ffast-math -march=athlon-xp -mmmx -m3dnow -msse

# *** This default generic build line will work well on all ****
CFLAGS_RELEASE=$(BASE_CFLAGS) -O2 -march=i386 -funroll-loops -ffast-math

# 'make build_debug' flags.
CFLAGS_DEBUG=$(BASE_CFLAGS) -g

# linker flags.
ifeq ($(OSTYPE),FreeBSD)
LDFLAGS=-lm
endif
ifeq ($(OSTYPE),Linux)
LDFLAGS=-lm -ldl
endif

SHLIBEXT=so

SHLIBCFLAGS=-fPIC
SHLIBLDFLAGS=-shared

DO_CC=$(CC) $(CFLAGS) $(SHLIBCFLAGS) -o $@ -c $<

all:
	@echo ""
	@echo ==== Quake2plus Makefile ====
	@echo ""
	@echo "Default is to make quiet - for verbose use > make V=1 [target]."
	@echo "Four possible Quake2plus Makefile targets:"
	@echo ""
	@echo " \"> make [V=1] baseq2_build_release or xatrix_build_release\"  to build binary release."
	@echo " \"> make [V=1] baseq2_build_debug or xatrix_build_debug\"  to build binary with debug options."
	@echo " \"> make clean\"          to clean (remove) all .o and .so files."
	@echo " \"> make depend\"         for development only."
	@echo ""

# Build baseq2 debug binary.
baseq2_build_debug:
	@echo ""
	@echo "        ===== Building Quake2plus baseq2 debug binary ====="
	@echo ""
	$(MAKE) game$(ARCH).$(SHLIBEXT) CFLAGS="$(CFLAGS_DEBUG)"

# Build debug binary.
xatrix_build_debug:
	@echo ""
	@echo "        ===== Building Quake2plus xatrix debug binary ====="
	@echo ""
	$(MAKE) game$(ARCH).$(SHLIBEXT) CFLAGS="$(CFLAGS_DEBUG) -DXATRIX"

# Build baseq2 release binary.
baseq2_build_release:
	@echo ""
	@echo "        ===== Building Quake2plus baseq2 release binary ====="
	@echo ""
	$(MAKE) game$(ARCH).$(SHLIBEXT) CFLAGS="$(CFLAGS_RELEASE)"

# Build xatrix release binary.
xatrix_build_release:
	@echo ""
	@echo "        ===== Building Quake2plus xatrix release binary ====="
	@echo ""
	$(MAKE) game$(ARCH).$(SHLIBEXT) CFLAGS="$(CFLAGS_RELEASE) -DXATRIX"

# Clean object files and binary.
clean:
	-rm -f $(GAME_OBJS) game$(ARCH).$(SHLIBEXT);

.DEFAULT:
	@echo ""
	@echo ==== Quake2plus Makefile ====
	@echo ""
	@echo "Default is to make quiet - for verbose use > make V=1 [target]."
	@echo "Four possible  Makefile targets:"
	@echo " \"> make [V=1] baseq2_build_release or xatrix_build_release\"  to build binary release."
	@echo " \"> make [V=1] baseq2_build_debug or xatrix_build_debug"  to build binary with debug options."
	@echo " \"> make clean\"          to clean (remove) all .o and .so files."
	@echo " \"> make depend\"         for development only."
	@echo ""

#############################################################################
# SETUP AND BUILD
# GAME
#############################################################################
.c.o:
ifeq ($(XBUILD_VERBOSE),0)
	@$(DO_CC)
	@echo "	CC: $<"
endif

ifeq ($(XBUILD_VERBOSE),1)
	$(DO_CC)
endif

GAME_OBJS = g_cmds.o g_combat.o g_ent.o g_func.o g_items.o g_main.o \
	    g_misc.o g_phys.o g_save.o g_spawn.o g_target.o g_trigger.o \
	    g_utils.o g_weapon.o p_client.o p_hud.o p_view.o p_weapon.o q_shared.o


ifeq ($(XBUILD_VERBOSE),0)
game$(ARCH).$(SHLIBEXT) : $(GAME_OBJS)
	@echo ""
	@$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(GAME_OBJS) $(LDFLAGS)
	@echo ""
	@echo "Linking..."
	@echo "	...game$(ARCH) ready."
	@echo "Build finished."
	@echo ""
endif

ifeq ($(XBUILD_VERBOSE),1)
game$(ARCH).$(SHLIBEXT) : $(GAME_OBJS)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(GAME_OBJS) $(LDFLAGS)
endif

	@echo "Possibly now use 'install -s gamei386.so /path/to/quake2/baseq2 or /quake2/xatrix/'" 
	@echo "folder to install *if* not a debug_build ('install -s' strips symbols for a smaller binary),"
	@echo "or just copy gamei386.so to baseq2/ or xatrix/ as required."
	@echo ""
	

#############################################################################
# MISC
#############################################################################

depend:
	$(CC) -MM $(GAME_OBJS:.o=.c);
g_cmds.o: g_cmds.c g_local.h q_shared.h game.h
g_combat.o: g_combat.c g_local.h q_shared.h game.h
g_ent.o: g_ent.c g_local.h q_shared.h game.h
g_func.o: g_func.c g_local.h q_shared.h game.h
g_items.o: g_items.c g_local.h q_shared.h game.h
g_main.o: g_main.c g_local.h q_shared.h game.h
g_misc.o: g_misc.c g_local.h q_shared.h game.h
g_phys.o: g_phys.c g_local.h q_shared.h game.h
g_save.o: g_save.c g_local.h q_shared.h game.h
g_spawn.o: g_spawn.c g_local.h q_shared.h game.h
g_target.o: g_target.c g_local.h q_shared.h game.h
g_trigger.o: g_trigger.c g_local.h q_shared.h game.h
g_utils.o: g_utils.c g_local.h q_shared.h game.h
g_weapon.o: g_weapon.c g_local.h q_shared.h game.h
p_client.o: p_client.c g_local.h q_shared.h game.h
p_hud.o: p_hud.c g_local.h q_shared.h game.h
p_view.o: p_view.c g_local.h q_shared.h game.h
p_weapon.o: p_weapon.c g_local.h q_shared.h game.h
q_shared.o: q_shared.c q_shared.h
