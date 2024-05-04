#ifndef PRINT_HPP
#define PRINT_HPP

#include <inttypes.h>
#include <stdio.h> // for size_t

#include "String.hpp"
#include "Printable.hpp"

#define DEC 10
#define HEX 16
#define OCT 8
#ifdef BIN // Prevent warnings if BIN is previously defined in "iotnx4.h" or similar
#undef BIN
#endif
#define BIN 2

class Print
{
private:
    int write_error;
    size_t printNumber(unsigned long, uint8_t);
    size_t printFloat(double, uint8_t);

protected:
    void setWriteError(int err = 1) { write_error = err; }

public:
    Print() : write_error(0) {}

    int getWriteError() { return write_error; }
    void clearWriteError() { setWriteError(0); }

    template <typename T> static inline uint8_t
	read_byte(T addr)
    {
    	return (*(uint8_t *)(addr));
    }

    template <typename T> static inline uint16_t
	read_word(T addr)
    {
    	return (*(uint16_t *)(addr));
    }

    template <typename T> static inline uint32_t
	read_dword(T addr)
    {
    	return (*(uint32_t *)(addr));
    }

    virtual size_t write(uint8_t) = 0;
    size_t write(const char *str)
    {
        if (str == NULL)
            return 0;
        return write((const uint8_t *)str, strlen(str));
    }

    virtual size_t write(const uint8_t *buffer, size_t size);
    size_t write(const char *buffer, size_t size)
    {
        return write((const uint8_t *)buffer, size);
    }

    // default to zero, meaning "a single write may block"
    // should be overridden by subclasses with buffering
    virtual int availableForWrite() { return 0; }

    size_t print(const __FlashStringHelper *);
    size_t print(const String &);
    size_t print(const char[]);
    size_t print(char);
    size_t print(unsigned char, int = DEC);
    size_t print(int, int = DEC);
    size_t print(unsigned int, int = DEC);
    size_t print(long, int = DEC);
    size_t print(unsigned long, int = DEC);
    size_t print(double, int = 2);
    size_t print(const Printable &);

    size_t println(const __FlashStringHelper *);
    size_t println(const String &s);
    size_t println(const char[]);
    size_t println(char);
    size_t println(unsigned char, int = DEC);
    size_t println(int, int = DEC);
    size_t println(unsigned int, int = DEC);
    size_t println(long, int = DEC);
    size_t println(unsigned long, int = DEC);
    size_t println(double, int = 2);
    size_t println(const Printable &);
    size_t println(void);

    virtual void flush()
    { /* Empty implementation for backward compatibility */
    }
};

#endif // PRINT_HPP
