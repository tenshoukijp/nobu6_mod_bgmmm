#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


size_t ODSfwrite(const void *buf, size_t size, size_t n, FILE *fp) {
	size_t ret=fwrite(buf, size, n, fp);
	if ( fp == stderr || fp == stdout ) {
		char szBufDebug[4096] = "";
	    int length = sizeof(szBufDebug);
		strncpy(szBufDebug, buf, length);
	    szBufDebug[length - 1] = '\0';
		OutputDebugString(szBufDebug);
	}
	return ret;
}


int ODSfputs( const char *str , FILE *fp ) {
	int ret=fputs(str, fp);
	if ( fp == stderr || fp == stdout ) {
		char szBufDebug[4096] = "";
	    int length = sizeof(szBufDebug);
		strncpy(szBufDebug, str, length);
	    szBufDebug[length - 1] = '\0';
		OutputDebugString(szBufDebug);
	}
	return ret;
}

int ODSputs( const char *str ) {
	int ret =  puts(str);
	char szBufDebug[4096] = "";
    int length = sizeof(szBufDebug);
	strncpy(szBufDebug, str, length);
    szBufDebug[length - 1] = '\0';
	OutputDebugString(szBufDebug);
	return ret;
}

int ODSputc( int c, FILE *stream ) {
	char buf[2] = "";
	buf[0] = (unsigned char)c;
	if ( stream == stderr || stream == stdout ) {
		OutputDebugString(buf);
	}
	return putc(c, stream);
}

int ODSfprintf( FILE *stream, char *format, ... ) {
	char szBufDebug[4096] = "";
	int ret = 0;
	int length = sizeof(szBufDebug);

	va_list arg;

	va_start(arg, format);
	ret = vfprintf( stream, format, arg );
	_vsnprintf_s(szBufDebug, length, _TRUNCATE, format, arg);
	va_end(arg);

	szBufDebug[length - 1] = '\0';

	if ( stream == stderr || stream == stdout ) {
		OutputDebugString( szBufDebug );
	}
	return ret;
}

int ODSprintf( char *format, ... ) {
	char szBufDebug[4096] = "";
	int ret = 0;
	int length = sizeof(szBufDebug);

	va_list arg;

	va_start(arg, format);
	ret = vprintf( format, arg );
	_vsnprintf_s(szBufDebug, length, _TRUNCATE, format, arg);
	va_end(arg);

	szBufDebug[length - 1] = '\0';

	OutputDebugString( szBufDebug );
	return ret;
}
