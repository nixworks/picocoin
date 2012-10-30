
INCLUDES	= @GLIB_CFLAGS@ -DTEST_SRCDIR=\"$(top_srcdir)/test\" \
		  -I$(top_srcdir)/include

EXTRA_DIST	= \
	base58_encode_decode.json \
	random.data

noinst_LIBRARIES= libtest.a

libtest_a_SOURCES= libtest.h libtest.c

noinst_PROGRAMS	= hex base58 fileio util

TESTS		= hex base58 fileio util

COMMON_LDADD	= libtest.a ../lib/libccoin.a \
		  @GLIB_LIBS@ @CRYPTO_LIBS@ @JANSSON_LIBS@

base58_LDADD	= $(COMMON_LDADD)
fileio_LDADD	= $(COMMON_LDADD)
hex_LDADD	= $(COMMON_LDADD)
util_LDADD	= $(COMMON_LDADD)
