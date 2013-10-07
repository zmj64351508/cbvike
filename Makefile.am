INCLUDES = $(WX_CXXFLAGS) \
		-I$(top_srcdir)/src/include \
		-I$(top_srcdir)/src/sdk/wxscintilla/include

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libvike.la

libvike_la_LDFLAGS = -module -version-info 0:1:0 -shared -no-undefined -avoid-version

libvike_la_LIBADD = $(PLUGIN_WX_LIBS) ../../../sdk/libcodeblocks.la

libvike_la_SOURCES = cbvike.cpp \
			vike.cpp

noinst_HEADERS = cbvike.h \
			debugging.h \
			vike.h

EXTRA_DIST = $(srcdir)/*.cbp manifest.xml


pkgdata_DATA = vike.zip
CLEANFILES = $(pkgdata_DATA)

vike.zip: manifest.xml
	PWD=`pwd` cd $(srcdir) && zip $(PWD)/vike.zip manifest.xml > /dev/null
