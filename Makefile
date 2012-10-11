#
# use mingw on Debian squeeze
#

NAME = alert
SRCS = $(NAME).c
OBJS = $(SRCS:.c=.o)
RC = resource.rc
RES = $(RC:.rc=.o)
DEF = $(NAME).def

NKF= nkf -Us

TARGET = $(NAME).dll

INSTALL_DIR = ~/vmware/XP/mingw/test/

CC = i586-mingw32msvc-g++
WINDRES = i586-mingw32msvc-windres
DLLTOOL = i586-mingw32msvc-dlltool
CFLAGS = -O2 -Wall -D_WINDOWS -D_USRDLL \
 -DNPAPI_DLLTEST_EXPORTS -DMOZILLA_STRICT_API \
 -DXPCOM_GLUE -DXP_WIN -D_X86_ -DXULRUNNER_SDK \
 -DUNICODE -D_UNICODE

LDFLAGS = -mwindows

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS) $(RES) $(DEF)
	$(CC) $(CFLAGS) -shared -Wl,--enable-stdcall-fixup -o $(TARGET) $(OBJS) $(RES) $(DEF) $(LDFLAGS)

$(RES): $(RC)
#	$(NKF) $< > $(subst .rc,_sjis.rc,$<)
#	$(WINDRES) $(subst .rc,_sjis.rc,$<) $@
	$(WINDRES) $< $@

.c.o:
#	$(NKF) $< > $(subst .c,_sjis.c,$<)
#	$(CC) $(CFLAGS) -o $@ -c $(subst .c,_sjis.c,$<)
	$(CC) $(CFLAGS) -o $@ -c $<



.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS) $(RES)
	rm -f *.a *_sjis.* *.tmp *.base *.exp *~

install: all
	cp -f $(TARGET) $(INSTALL_DIR)

# flymake
PHONY: check-syntax
check-syntax:
	$(CC) -Wall -Wextra -pedantic -fsyntax-only $(CHK_SOURCES) $(CFLAGS)
