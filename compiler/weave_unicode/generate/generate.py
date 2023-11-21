#!/usr/bin/python
# -*- coding: ascii -*-

import os

BIT_ALPHA       =    1
BIT_BLANK       =    2
BIT_CNTRL       =    4
BIT_GRAPH       =    8
BIT_PUNCT       =   16
BIT_SPACE       =   32
BIT_LOWER       =   64
BIT_UPPER       =  128
BIT_DIGIT       =  256
BIT_XDIGT       =  512
BIT_CBYTE       = 1024

def bits_to_string(value):
    if value == 0:
        return "CTYPE_NONE"
    else:
        bits = []

        if (value & BIT_ALPHA) == BIT_ALPHA:
            bits.append("CTYPE_ALPHA")
        if (value & BIT_BLANK) == BIT_BLANK:
            bits.append("CTYPE_BLANK")
        if (value & BIT_CNTRL) == BIT_CNTRL:
            bits.append("CTYPE_CNTRL")
        if (value & BIT_GRAPH) == BIT_GRAPH:
            bits.append("CTYPE_GRAPH")
        if (value & BIT_PUNCT) == BIT_PUNCT:
            bits.append("CTYPE_PUNCT")
        if (value & BIT_SPACE) == BIT_SPACE:
            bits.append("CTYPE_SPACE")
        if (value & BIT_LOWER) == BIT_LOWER:
            bits.append("CTYPE_LOWER")
        if (value & BIT_UPPER) == BIT_UPPER:
            bits.append("CTYPE_UPPER")
        if (value & BIT_DIGIT) == BIT_DIGIT:
            bits.append("CTYPE_DIGIT")
        if (value & BIT_XDIGT) == BIT_XDIGT:
            bits.append("CTYPE_XDIGIT")
        if (value & BIT_CBYTE) == BIT_CBYTE:
            bits.append("CTYPE_CBYTE")

        return " | ".join(bits)

# Category to bitfield mapping
categories = {
    'Lu': BIT_ALPHA | BIT_GRAPH | BIT_UPPER,    # Uppercase
    'Ll': BIT_ALPHA | BIT_GRAPH | BIT_LOWER,    # Lowercase
    'Lt': BIT_ALPHA | BIT_GRAPH | BIT_UPPER,    # Title case. Upper?
    'Lm': BIT_ALPHA | BIT_GRAPH,                # Modifier. Case?
    'Lo': BIT_ALPHA | BIT_GRAPH,                # "Other" letter (e.g. Ideograph)
    'Nd': BIT_DIGIT | BIT_GRAPH,                # Decimal digit
    'Nl': BIT_GRAPH,                            # Letter-like numeric character
    'No': BIT_GRAPH,                            # Other numeric
    'Pc': BIT_PUNCT | BIT_GRAPH,                # Connecting punctuation
    'Pd': BIT_PUNCT | BIT_GRAPH,                # Dash punctuation
    'Ps': BIT_PUNCT | BIT_GRAPH,                # Opening punctuation
    'Pe': BIT_PUNCT | BIT_GRAPH,                # Closing punctuation
    'Pi': BIT_PUNCT | BIT_GRAPH,                # Opening quote
    'Pf': BIT_PUNCT | BIT_GRAPH,                # Closing quote
    'Po': BIT_PUNCT | BIT_GRAPH,                # Other punctuation
    'Sm': BIT_GRAPH,                            # Mathematical symbol
    'Sc': BIT_GRAPH,                            # Currency symbol
    'Sk': BIT_GRAPH,                            # Non-letterlike modifier symbol
    'So': BIT_GRAPH,                            # Other symbol
    'Zs': BIT_SPACE,                            # Non-zero-width space character
    'Zl': BIT_SPACE,                            # Line separator
    'Zp': BIT_SPACE,                            # Paragraph separator
    'Cc': BIT_CNTRL,                            # C0/C1 control codes
}

# Characters with special properties
special = {
    # Blank characters
    0x0020: BIT_SPACE | BIT_BLANK, # space
    0x0009: BIT_SPACE | BIT_BLANK, # tab

    # Digits
    0x0030: BIT_XDIGT | BIT_DIGIT | BIT_GRAPH,
    0x0031: BIT_XDIGT | BIT_DIGIT | BIT_GRAPH,
    0x0032: BIT_XDIGT | BIT_DIGIT | BIT_GRAPH,
    0x0033: BIT_XDIGT | BIT_DIGIT | BIT_GRAPH,
    0x0034: BIT_XDIGT | BIT_DIGIT | BIT_GRAPH,
    0x0035: BIT_XDIGT | BIT_DIGIT | BIT_GRAPH,
    0x0036: BIT_XDIGT | BIT_DIGIT | BIT_GRAPH,
    0x0037: BIT_XDIGT | BIT_DIGIT | BIT_GRAPH,
    0x0038: BIT_XDIGT | BIT_DIGIT | BIT_GRAPH,
    0x0039: BIT_XDIGT | BIT_DIGIT | BIT_GRAPH,

    # A-F (hex uppercase)
    0x0041: BIT_XDIGT | BIT_ALPHA | BIT_GRAPH | BIT_UPPER,
    0x0042: BIT_XDIGT | BIT_ALPHA | BIT_GRAPH | BIT_UPPER,
    0x0043: BIT_XDIGT | BIT_ALPHA | BIT_GRAPH | BIT_UPPER,
    0x0044: BIT_XDIGT | BIT_ALPHA | BIT_GRAPH | BIT_UPPER,
    0x0045: BIT_XDIGT | BIT_ALPHA | BIT_GRAPH | BIT_UPPER,
    0x0046: BIT_XDIGT | BIT_ALPHA | BIT_GRAPH | BIT_UPPER,


    # a-f (hex lowercase)
    0x0061: BIT_XDIGT | BIT_ALPHA | BIT_GRAPH | BIT_LOWER,
    0x0062: BIT_XDIGT | BIT_ALPHA | BIT_GRAPH | BIT_LOWER,
    0x0063: BIT_XDIGT | BIT_ALPHA | BIT_GRAPH | BIT_LOWER,
    0x0064: BIT_XDIGT | BIT_ALPHA | BIT_GRAPH | BIT_LOWER,
    0x0065: BIT_XDIGT | BIT_ALPHA | BIT_GRAPH | BIT_LOWER,
    0x0066: BIT_XDIGT | BIT_ALPHA | BIT_GRAPH | BIT_LOWER,
}

class Group:
    def __init__(self, start, flags, upper_delta, lower_delta):
        self.start = start
        self.flags = flags
        self.upper_delta = upper_delta
        self.lower_delta = lower_delta
        self.chars = []

    def add_char(self, num, label):
        self.chars.append((num, label))

    def write_to_file(self, f):
        #for char in self.chars:
        #    f.write("    // %x %s\n" % char)
        f.write("    {%#08x, %#04x, %s, %d, %d},\n" %
            (self.start, len(self.chars), bits_to_string(self.flags), self.lower_delta, self.upper_delta))

        #f.write("    {0x%08x, 0x%04x, %s, (uint32_t)(%d), (uint32_t)(%d)},\n" %

    def next(self):
        return self.start + len(self.chars)

groups = []

def add_char(num, upper, lower, bits, label):
    upper_delta = upper - num
    lower_delta = lower - num

    if len(groups) != 0:
        cur = groups[-1]
        if num == cur.next() and cur.flags == bits and \
                cur.upper_delta == upper_delta and \
                cur.lower_delta == lower_delta:
            cur.add_char(num, label)
            return

    g = Group(num, bits, upper_delta, lower_delta)
    g.add_char(num, label)
    groups.append(g)

in_file  = open('UnicodeData.txt', 'r')
out_file = open('unicodedata.cxx', 'w')
try:
    for line in in_file:
        (num_hex, name, category, combining_class, bidi_class, decomposition,
         numeric_type, numeric_digit, numeric_value, mirrored, u1name, iso_com, 
         upper_case_hex, lower_case_hex, title_case_hex) = line.split(";")

        num        = int(num_hex, 16)
        upper_case = int(upper_case_hex, 16) if len(upper_case_hex) else num
        lower_case = int(lower_case_hex, 16) if len(lower_case_hex) else num
        bits = special.get(num, categories.get(category, 0))

        if upper_case == 0 and lower_case == 0 and bits == 0:
            continue

        add_char(num, upper_case, lower_case, bits, name)

    out_file.write("""#include <locale.h>
#include <ctype.h>
#include <stddef.h>
#include "../libcrt_ctype.hxx"

namespace libcrt_ctype {
extern const unicode_data_entry unicode_data_table[] =
{
//  {value, length, flags, lower, upper},
""")
    for g in groups:
        g.write_to_file(out_file)
    out_file.write('};')
    out_file.write("""
extern const size_t unicode_data_table_size = sizeof(unicode_data_table) / sizeof(unicode_data_table[0]);
}
""")
except:
    in_file.close()
    out_file.close()
    os.remove('unicodedata.cxx')
    raise
else:
    in_file.close()
    out_file.close()
