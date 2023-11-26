
# Bug d'affichage avec uname sous clang64 ...
ifeq (${MSYSTEM},CLANG64)
	UNAME=/usr/bin/uname
else
	UNAME=uname
endif

OS=$(shell ${UNAME} -s)

ifneq (${OS},Linux)
ECHO=echo -e
else
ECHO=echo
endif

PGF=$(subst \,/,$(subst C:\,/c/,$(PROGRAMFILES)))
PGF86=${PGF} (x86)
PATH:=${PATH}:${PGF86}/Inno Setup 6
PATH:=${PATH}:${PGF}/Inkscape/bin
PATH:=${PATH}:${PGF86}/Pandoc

ifneq (${OS},Linux)
 MAGICK=magick
endif
RC=windres
STRIP=strip
UPX=upx

VERSION=$(shell git describe --abbrev=0 --tags 2>/dev/null || echo 'Unknown_version')
COMMIT=$(shell git rev-parse --short HEAD 2>/dev/null || echo 'Unknown_commit')
DECORATION=Nawak-Bidon

CXXFLAGS += -std=c++20 -g
CXXFLAGS += -Wall # -pedantic -Wextra # Utiliser ces 2 dernières options de temps en temps peut-être utile ...
LDFLAGS += -g

ifeq (${OS},Linux)
#LDFLAGS += -static
PANDOC=pandoc
else
ifeq (${MSYSTEM},CLANG64)
PATH:=/clang64/bin:$PATH
CC=clang
CXX=clang++
endif
EXEXT=.exe
LDFLAGS += -mwindows
LDFLAGS += -static

LDLIBS += -ladvapi32 -lole32 -lshell32 -lshlwapi -luser32 -lversion -lwsock32 -lws2_32 -luxtheme

endif

#MSBUILD='C:\Program\ Files\Microsoft\ Visual\ Studio\2022\Community\MSBuild\Current\Bin\amd64\MSBuild.exe'
MSBUILD=/c/Program\ Files/Microsoft\ Visual\ Studio/2022/Community/MSBuild/Current/Bin/amd64/MSBuild.exe
DO_MSBUILD=$(shell test -f $(MSBUILD) && echo 1 || echo 0)
ifeq ($(MAKECMDGOALS),gcc)
DO_MSBUILD=0
endif

PREFIX=weekday
SRCS=$(wildcard *.cpp)

ifeq (${OS},Linux)
SRCS:=$(filter-out wv-winapi.cpp,${SRCS})
else
OBJS=${PREFIX}Res.o
RES_SRC=${PREFIX}Res.rc
endif

OBJS+=$(SRCS:.cpp=.o)
TARGET=${PREFIX}${EXEXT}

.PHONY: FORCE

ARCH=x64
ifeq ($(DO_MSBUILD),1)
CONF=Release
DEFAULT_TARGET=version_check.txt version.h ${PREFIX}.ico ${PREFIX}Res.manifest ${TARGET} README.docx

${TARGET} : ${ARCH}/${CONF}/${TARGET}
	cp ${ARCH}/${CONF}/${TARGET} .

${ARCH}/${CONF}/${TARGET} : ${PREFIX}.ico ${PREFIX}Res.manifest ${SRCS} ${RES_SRC}
	${MSBUILD} ${PREFIX}.sln -p:Configuration=${CONF}
else
DEFAULT_TARGET=version_check.txt version.h ${TARGET}

${TARGET} : ${OBJS}
	$(LINK.cc) ${OBJS} $(LOADLIBES) $(LDLIBS) -o $@
endif

all : ${DEFAULT_TARGET}
	@echo "All done ${DEFAULT_TARGET}"

gcc : all

${PREFIX}Res.o : ${PREFIX}.ico ${PREFIX}Res.manifest

${PREFIX}Res.manifest :
	@echo "Building $@"
ifeq ($(findstring 64,$(ARCH)),64)
	@sed 's/PROCARCH/amd64/' manifest >$@
else
	@sed 's/PROCARCH/x86/' manifest >$@
endif

strip : $(TARGET)
	$(STRIP) $(TARGET) | true

upx : strip
	$(UPX) $(TARGET) | true

ALL_SRCS=$(wildcard *.cpp) $(wildcard *.hpp) $(wildcard *.h)
format :
	@clang-format -style="{ BasedOnStyle: Microsoft, IndentWidth: 2 }" --sort-includes -i ${ALL_SRCS}

clean :
	rm -f *~ *.d ${PREFIX}.ico *.o $(OBJS) ${DEFAULT_TARGET} $(TARGET) $(TARGET:.exe=res.manifest)
ifeq ($(DO_MSBUILD),1)
	rm -rf ${ARCH} README.docx
endif

# Génération du version.h intégré dans l'appli
version.h : version_check.txt
	@${ECHO} "Building C++ header $@"
	@${ECHO} "std::string name=\"${PREFIX}\", version=\"${VERSION}\", decoration=\"${DECORATION}\", commit=\"${COMMIT}\", created_at=\"${ISO8601}\";" >$@

# Génération du version.json intégré dans le paquetage
version.json : version_check.txt
	@${ECHO} "Building json file $@"
	@${ECHO} '{ "name":"${PREFIX}", "version":"${VERSION}", "decoration":"${DECORATION}", "commit":"${COMMIT}","created_at":"${ISO8601}" }' >$@

# Pour regénérer version.h et version.json dès qu'un des champs version ou decoration ou commit, est modifié.
version_check.txt : FORCE
	@${ECHO} "Version:${VERSION}, decoration:${DECORATION}, commit:${COMMIT}" >new_$@
	@if diff new_$@ $@ >/dev/null 2>&1; then rm -f new_$@; else mv -f new_$@ $@; rm -f ${PREFIX}.iss ${PREFIX}-standalone.iss; fi

cfg :
	@echo "PGF ${PGF}"
	@echo "PGF86 ${PGF86}"
	@echo "PATH"
	@echo "${PATH}" | sed 's/:/\n/g'
	@echo "END PATH"
	@which inkscape.exe
	@echo "DO_MSBUILD [${DO_MSBUILD}]"

# Ces régles implicites ne sont pas utiles quand on fait 'make clean'
ifneq ($(MAKECMDGOALS),clean)
%.docx : %.md
	pandoc -o $@ -f markdown -t docx $<

%.ico : %.png
	${MAGICK} convert -background none $< $@

%.ico : %.svg
	${MAGICK} convert -background none $< $@

%${EXEXT}: %.o
	$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) -o $@

%${EXEXT}: %.c
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@

%${EXEXT}: %.cpp
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

ifneq ($(DO_MSBUILD),1)
# Régles pour construire les fichier objet d'après les .rc
%.o : %.rc
	$(RC) $(CPPFLAGS) $< --include-dir . $(OUTPUT_OPTION)

%.d: %.c
	@echo Checking header dependencies from $<
	@$(COMPILE.c) -isystem /usr/include -MM $< >> $@

#	@echo "Building "$@" from "$<
%.d: %.cpp
	@echo Checking header dependencies from $<
	@$(COMPILE.cpp) -isystem /usr/include -MM $< >> $@

# Inclusion des fichiers de dépendance .d
ifdef OBJS
-include $(OBJS:.o=.d)
endif
endif
endif

