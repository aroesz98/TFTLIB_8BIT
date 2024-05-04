#ifndef PRINTABLE_HPP
#define PRINTABLE_HPP

#include <stdlib.h>

class Print;

class Printable
{
public:
    virtual size_t printTo(Print &p) const = 0;
};

#endif // PRINTABLE_HPP
