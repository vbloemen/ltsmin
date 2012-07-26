HRE_LIB = ../hre/libhre.la
DIST_CPPFLAGS = -I ${top_srcdir}/src $(AM_CPPFLAGS)
MC_LIBS = ../libmc.la

bin_PROGRAMS = etf2lts-mc

#if HAVE_LIBSTEP_INFO
#bin_PROGRAMS += lpo2lts-mc
#endif

#if HAVE_LIBMCRL2
#bin_PROGRAMS += lps2lts-mc
#endif

#if HAVE_LIBPBES
#bin_PROGRAMS += pbes2lts-mc
#endif

if HAVE_DVE
bin_PROGRAMS += dve2lts-mc
endif

if HAVE_SPINJA
bin_PROGRAMS += prom2lts-mc
endif

if HAVE_OPAAL
bin_PROGRAMS += opaal2lts-mc
endif

# etf2lts-mc
etf2lts_mc_SOURCES   = pins2lts-mc.c
etf2lts_mc_SOURCES  += ../etf-greybox.c ../etf-greybox.h
etf2lts_mc_LDADD     = ../lts-io/liblts-io.la $(HRE_LIB) ../libltsmin.la
etf2lts_mc_LDADD    += $(MC_LIBS) 
etf2lts_mc_CPPFLAGS  = -DETF $(DIST_CPPFLAGS)

## lpo2lts-mc
#lpo2lts_mc_SOURCES   = pins2lts-mc.c
#lpo2lts_mc_SOURCES  += ../mcrl-greybox.c ../mcrl-greybox.h
#lpo2lts_mc_SOURCES  += ../at-map.c ../at-map.h
#lpo2lts_mc_LDADD     = ../lts-io/liblts-io.la $(HRE_LIB) ../libltsmin.la
#lpo2lts_mc_LDADD    += $(MC_LIBS) $(MCRL_LIBS)
#lpo2lts_mc_CPPFLAGS  = -DMCRL $(MCRL_CPPFLAGS) $(DIST_CPPFLAGS)
#lpo2lts_mc_LDFLAGS   = $(MCRL_LDFLAGS) $(AM_LDFLAGS)

## lps2lts-mc
#lps2lts_mc_SOURCES   = pins2lts-mc.c
#lps2lts_mc_LDADD     = ../lts-io/liblts-io.la $(HRE_LIB) ../libltsmin.la
#lps2lts_mc_LDADD    += $(MC_LIBS)  ../libmcrl2-pins.la
#lps2lts_mc_CPPFLAGS  = -DMCRL2 $(DIST_CPPFLAGS)
#lps2lts_mc_LDFLAGS   = $(MCRL2_LDFLAGS) $(AM_LDFLAGS)
#nodist_EXTRA_lps2lts_mc_SOURCES = ../automake-force-linker.cxx

## pbes2lts-mc
#pbes2lts_mc_SOURCES   = pins2lts-mc.c
#pbes2lts_mc_LDADD     = ../lts-io/liblts-io.la $(HRE_LIB) ../libltsmin.la
#pbes2lts_mc_LDADD    += $(MC_LIBS) ../libpbes-pins.la
#pbes2lts_mc_CPPFLAGS  = -DPBES $(DIST_CPPFLAGS)
#pbes2lts_mc_LDFLAGS   = $(MCRL2_LDFLAGS) $(AM_LDFLAGS)
#nodist_EXTRA_pbes2lts_mc_SOURCES = ../automake-force-linker.cxx

# dve2lts-mc
dve2lts_mc_SOURCES   = pins2lts-mc.c
dve2lts_mc_SOURCES  += ../dve-greybox.c ../dve-greybox.h
dve2lts_mc_LDADD     = ../lts-io/liblts-io.la $(HRE_LIB) ../libltsmin.la
dve2lts_mc_LDADD    += $(MC_LIBS) $(DVEC_LIBS)
dve2lts_mc_CPPFLAGS  = -DDIVINE $(DIVINE_CPPFLAGS) $(DIST_CPPFLAGS)
dve2lts_mc_LDFLAGS   = $(DIVINE_LDFLAGS) $(AM_LDFLAGS)

# prom2lts-mc
prom2lts_mc_SOURCES  = pins2lts-mc.c
prom2lts_mc_SOURCES += ../prom-greybox.c ../prom-greybox.h
prom2lts_mc_LDADD    = ../lts-io/liblts-io.la $(HRE_LIB) ../libltsmin.la
prom2lts_mc_LDADD   += $(MC_LIBS) $(SPINJA_LIBS)
prom2lts_mc_CPPFLAGS = -DSPINJA $(SPINJA_CPPFLAGS) $(DIST_CPPFLAGS)
prom2lts_mc_LDFLAGS  = $(SPINJA_LDFLAGS) $(AM_LDFLAGS)

# opaal2lts-mc
opaal2lts_mc_SOURCES   = pins2lts-mc.c
opaal2lts_mc_SOURCES  += ../opaal-greybox.c ../opaal-greybox.h
opaal2lts_mc_LDADD     = ../lts-io/liblts-io.la $(HRE_LIB) ../libltsmin.la
opaal2lts_mc_LDADD    += $(MC_LIBS) $(OPAAL_LDFLAGS) 
opaal2lts_mc_CPPFLAGS  = -DOPAAL $(DIST_CPPFLAGS)