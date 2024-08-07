/*******************************************************************************
 * Size: 16 px
 * Bpp: 1
 * Opts: --bpp 1 --size 16 --font /Users/ry79/SquareLine/assets/微软雅黑.ttf -o /Users/ry79/SquareLine/assets/ui_font_MSBlack16.c --format lvgl -r 0x20-0x7f --symbols 卡未插入 --no-compress --no-prefilter
 ******************************************************************************/

#include "../ui.h"

#ifndef UI_FONT_MSBLACK16
#define UI_FONT_MSBLACK16 1
#endif

#if UI_FONT_MSBLACK16

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xff, 0xff, 0xcf,

    /* U+0022 "\"" */
    0xff, 0xff,

    /* U+0023 "#" */
    0x9, 0x2, 0x41, 0x91, 0xff, 0x12, 0x4, 0x81,
    0x23, 0xfe, 0x22, 0x9, 0x2, 0x40,

    /* U+0024 "$" */
    0x10, 0x20, 0xf6, 0x8d, 0x1a, 0x3c, 0x38, 0x1c,
    0x3c, 0x58, 0xb9, 0x7f, 0x4, 0x8,

    /* U+0025 "%" */
    0x78, 0x66, 0x66, 0x21, 0x21, 0xb, 0xc, 0xd0,
    0x3d, 0x80, 0x1b, 0xc0, 0xb3, 0xd, 0x8, 0xc8,
    0x44, 0x66, 0x61, 0xe0,

    /* U+0026 "&" */
    0x3e, 0x3, 0x18, 0x18, 0xc0, 0xc6, 0x63, 0xe3,
    0x1e, 0x19, 0x98, 0x98, 0x6c, 0xc1, 0xc6, 0x6,
    0x18, 0xd0, 0x7c, 0x70,

    /* U+0027 "'" */
    0xff,

    /* U+0028 "(" */
    0x11, 0x98, 0xcc, 0x63, 0x18, 0xc6, 0x30, 0xc6,
    0x18, 0x40,

    /* U+0029 ")" */
    0x43, 0xc, 0x61, 0x8c, 0x63, 0x18, 0xc6, 0x63,
    0x31, 0x0,

    /* U+002A "*" */
    0x20, 0x8f, 0xc8, 0x53, 0x20,

    /* U+002B "+" */
    0x10, 0x10, 0x10, 0xff, 0x10, 0x10, 0x10, 0x10,

    /* U+002C "," */
    0x69, 0x60,

    /* U+002D "-" */
    0xf8,

    /* U+002E "." */
    0xf0,

    /* U+002F "/" */
    0x6, 0x8, 0x10, 0x60, 0x83, 0x4, 0x8, 0x30,
    0x41, 0x82, 0x4, 0x18, 0x20, 0x0,

    /* U+0030 "0" */
    0x3c, 0x66, 0x43, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc2, 0x66, 0x3c,

    /* U+0031 "1" */
    0x31, 0xe0, 0xc1, 0x83, 0x6, 0xc, 0x18, 0x30,
    0x60, 0xc7, 0xf0,

    /* U+0032 "2" */
    0x7c, 0x47, 0x3, 0x3, 0x3, 0x6, 0xe, 0x1c,
    0x38, 0x70, 0x40, 0xff,

    /* U+0033 "3" */
    0x7d, 0x1c, 0x18, 0x30, 0xce, 0x3, 0x3, 0x6,
    0xe, 0x37, 0xc0,

    /* U+0034 "4" */
    0xe, 0x7, 0x7, 0x82, 0xc2, 0x61, 0x31, 0x19,
    0x8c, 0xff, 0x83, 0x1, 0x80, 0xc0,

    /* U+0035 "5" */
    0x7e, 0x60, 0x60, 0x60, 0x7c, 0x6, 0x3, 0x3,
    0x3, 0x3, 0x86, 0xfc,

    /* U+0036 "6" */
    0x1e, 0x30, 0x60, 0xc0, 0xdc, 0xe6, 0xc3, 0xc3,
    0xc3, 0xc3, 0x66, 0x3c,

    /* U+0037 "7" */
    0xff, 0x3, 0x2, 0x6, 0x4, 0xc, 0x8, 0x18,
    0x18, 0x10, 0x30, 0x20,

    /* U+0038 "8" */
    0x3c, 0xe7, 0xc3, 0xc3, 0x66, 0x3c, 0x7e, 0xc7,
    0xc3, 0xc3, 0xe6, 0x3c,

    /* U+0039 "9" */
    0x3c, 0x66, 0xc3, 0xc3, 0xc3, 0xe7, 0x7b, 0x3,
    0x3, 0x6, 0xc, 0x78,

    /* U+003A ":" */
    0xf0, 0x3, 0xc0,

    /* U+003B ";" */
    0x6c, 0x0, 0x3, 0x4b, 0x0,

    /* U+003C "<" */
    0x3, 0xc, 0x38, 0xe0, 0xe0, 0x38, 0xe, 0x1,

    /* U+003D "=" */
    0xff, 0x0, 0x0, 0x0, 0xff,

    /* U+003E ">" */
    0xc0, 0x70, 0x18, 0x6, 0xe, 0x38, 0xe0, 0x80,

    /* U+003F "?" */
    0x7d, 0xc, 0x18, 0x30, 0xc3, 0xc, 0x18, 0x0,
    0x0, 0xc1, 0x80,

    /* U+0040 "@" */
    0xf, 0xc0, 0x60, 0xc2, 0x1, 0x91, 0xe2, 0xcc,
    0x86, 0x62, 0x19, 0x88, 0x66, 0x21, 0x98, 0x86,
    0x66, 0x24, 0xe7, 0x18, 0x0, 0x30, 0x0, 0x3f,
    0x0,

    /* U+0041 "A" */
    0x6, 0x1, 0xc0, 0x28, 0xd, 0x81, 0x90, 0x23,
    0xc, 0x61, 0xfc, 0x20, 0xcc, 0x9, 0x1, 0x60,
    0x30,

    /* U+0042 "B" */
    0xfc, 0xc6, 0xc6, 0xc6, 0xcc, 0xf8, 0xc6, 0xc3,
    0xc3, 0xc3, 0xc6, 0xfc,

    /* U+0043 "C" */
    0xf, 0x98, 0x18, 0x1c, 0xc, 0x6, 0x3, 0x1,
    0x80, 0xc0, 0x30, 0xc, 0x23, 0xf0,

    /* U+0044 "D" */
    0xfc, 0x30, 0xcc, 0x1b, 0x3, 0xc0, 0xf0, 0x3c,
    0xf, 0x3, 0xc0, 0xb0, 0x6c, 0x33, 0xf0,

    /* U+0045 "E" */
    0xff, 0x83, 0x6, 0xc, 0x1f, 0xb0, 0x60, 0xc1,
    0x83, 0x7, 0xf0,

    /* U+0046 "F" */
    0xff, 0x83, 0x6, 0xc, 0x1f, 0xb0, 0x60, 0xc1,
    0x83, 0x6, 0x0,

    /* U+0047 "G" */
    0xf, 0xcc, 0x16, 0x1, 0x80, 0xc0, 0x30, 0xc,
    0x3f, 0x3, 0xc0, 0xd8, 0x33, 0xc, 0x7e,

    /* U+0048 "H" */
    0xc1, 0xe0, 0xf0, 0x78, 0x3c, 0x1f, 0xff, 0x7,
    0x83, 0xc1, 0xe0, 0xf0, 0x78, 0x30,

    /* U+0049 "I" */
    0xf6, 0x66, 0x66, 0x66, 0x66, 0x6f,

    /* U+004A "J" */
    0x18, 0xc6, 0x31, 0x8c, 0x63, 0x18, 0xcd, 0xc0,

    /* U+004B "K" */
    0xc3, 0x63, 0x31, 0x19, 0x8d, 0x87, 0x83, 0xc1,
    0xb0, 0xcc, 0x63, 0x30, 0xd8, 0x70,

    /* U+004C "L" */
    0xc1, 0x83, 0x6, 0xc, 0x18, 0x30, 0x60, 0xc1,
    0x83, 0x7, 0xf0,

    /* U+004D "M" */
    0xe0, 0x3f, 0x1, 0xfc, 0x1f, 0xa0, 0xfd, 0x5,
    0xec, 0x6f, 0x22, 0x79, 0xb3, 0xc5, 0x1e, 0x28,
    0xf1, 0xc7, 0x84, 0x30,

    /* U+004E "N" */
    0xe0, 0xf8, 0x3f, 0xf, 0x43, 0xd8, 0xf3, 0x3c,
    0x4f, 0x1b, 0xc2, 0xf0, 0xfc, 0x1f, 0x7,

    /* U+004F "O" */
    0x1f, 0x83, 0xc, 0x60, 0x6c, 0x3, 0xc0, 0x3c,
    0x3, 0xc0, 0x3c, 0x3, 0xc0, 0x36, 0x6, 0x30,
    0xc1, 0xf8,

    /* U+0050 "P" */
    0xfc, 0xc6, 0xc3, 0xc3, 0xc3, 0xc6, 0xfc, 0xc0,
    0xc0, 0xc0, 0xc0, 0xc0,

    /* U+0051 "Q" */
    0x1f, 0x83, 0xc, 0x60, 0x6c, 0x3, 0xc0, 0x3c,
    0x3, 0xc0, 0x3c, 0x3, 0xc0, 0x36, 0x6, 0x30,
    0xc1, 0xf8, 0x3, 0x0, 0x18, 0x0, 0x70,

    /* U+0052 "R" */
    0xfc, 0x63, 0x30, 0xd8, 0x6c, 0x36, 0x33, 0xe1,
    0x98, 0xc6, 0x61, 0xb0, 0x58, 0x30,

    /* U+0053 "S" */
    0x3e, 0x60, 0xc0, 0xc0, 0xe0, 0x78, 0x1e, 0x7,
    0x3, 0x3, 0x86, 0xfc,

    /* U+0054 "T" */
    0xff, 0x8c, 0x6, 0x3, 0x1, 0x80, 0xc0, 0x60,
    0x30, 0x18, 0xc, 0x6, 0x3, 0x0,

    /* U+0055 "U" */
    0xc1, 0xe0, 0xf0, 0x78, 0x3c, 0x1e, 0xf, 0x7,
    0x83, 0xc1, 0xe0, 0xd8, 0xc7, 0xc0,

    /* U+0056 "V" */
    0xc0, 0x68, 0x19, 0x83, 0x30, 0x42, 0x18, 0x63,
    0xc, 0x40, 0x98, 0x1b, 0x1, 0x40, 0x38, 0x6,
    0x0,

    /* U+0057 "W" */
    0xc1, 0x83, 0x41, 0x83, 0x61, 0xc2, 0x63, 0xc6,
    0x23, 0x46, 0x22, 0x66, 0x32, 0x64, 0x36, 0x2c,
    0x16, 0x2c, 0x14, 0x38, 0x1c, 0x38, 0xc, 0x18,

    /* U+0058 "X" */
    0x60, 0xd8, 0x63, 0x10, 0x4c, 0x1a, 0x3, 0x80,
    0xe0, 0x78, 0x13, 0xc, 0x66, 0x19, 0x3,

    /* U+0059 "Y" */
    0xc0, 0xd8, 0x66, 0x18, 0xcc, 0x33, 0x7, 0x81,
    0xe0, 0x30, 0xc, 0x3, 0x0, 0xc0, 0x30,

    /* U+005A "Z" */
    0xff, 0x81, 0x80, 0xc0, 0xc0, 0xc0, 0x60, 0x60,
    0x20, 0x30, 0x30, 0x10, 0x1f, 0xf0,

    /* U+005B "[" */
    0xfc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xf0,

    /* U+005C "\\" */
    0x81, 0x81, 0x3, 0x2, 0x4, 0xc, 0x8, 0x18,
    0x10, 0x20, 0x60, 0x40, 0xc1, 0x80,

    /* U+005D "]" */
    0xf3, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0xf0,

    /* U+005E "^" */
    0x10, 0x18, 0x3c, 0x24, 0x66, 0x42, 0xc3,

    /* U+005F "_" */
    0xfe,

    /* U+0060 "`" */
    0xc8, 0x80,

    /* U+0061 "a" */
    0x3c, 0x8c, 0x18, 0xf7, 0xf8, 0xf1, 0xe7, 0x76,

    /* U+0062 "b" */
    0xc0, 0xc0, 0xc0, 0xc0, 0xdc, 0xe6, 0xc3, 0xc3,
    0xc3, 0xc3, 0xc3, 0xe6, 0xdc,

    /* U+0063 "c" */
    0x3e, 0xc7, 0x6, 0xc, 0x18, 0x30, 0x31, 0x3e,

    /* U+0064 "d" */
    0x3, 0x3, 0x3, 0x3, 0x3b, 0x67, 0xc3, 0xc3,
    0xc3, 0xc3, 0xc3, 0x67, 0x3b,

    /* U+0065 "e" */
    0x3c, 0x66, 0xc3, 0xc3, 0xff, 0xc0, 0xc0, 0x60,
    0x3e,

    /* U+0066 "f" */
    0x3d, 0x86, 0x18, 0xf9, 0x86, 0x18, 0x61, 0x86,
    0x18, 0x60,

    /* U+0067 "g" */
    0x3b, 0x67, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x67,
    0x3b, 0x3, 0x3, 0x6, 0x7c,

    /* U+0068 "h" */
    0xc0, 0xc0, 0xc0, 0xc0, 0xde, 0xe7, 0xc3, 0xc3,
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3,

    /* U+0069 "i" */
    0xf0, 0xff, 0xff, 0xc0,

    /* U+006A "j" */
    0x18, 0xc0, 0x1, 0x8c, 0x63, 0x18, 0xc6, 0x31,
    0x8c, 0x66, 0xe0,

    /* U+006B "k" */
    0xc0, 0xc0, 0xc0, 0xc0, 0xc6, 0xcc, 0xd8, 0xd0,
    0xf0, 0xd8, 0xcc, 0xcc, 0xc6,

    /* U+006C "l" */
    0xff, 0xff, 0xff, 0xc0,

    /* U+006D "m" */
    0xdc, 0xee, 0xf3, 0xc6, 0x3c, 0x63, 0xc6, 0x3c,
    0x63, 0xc6, 0x3c, 0x63, 0xc6, 0x30,

    /* U+006E "n" */
    0xde, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3,

    /* U+006F "o" */
    0x3e, 0x31, 0xb0, 0x78, 0x3c, 0x1e, 0xf, 0x6,
    0xc6, 0x3e, 0x0,

    /* U+0070 "p" */
    0xdc, 0xe6, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xe6,
    0xdc, 0xc0, 0xc0, 0xc0, 0xc0,

    /* U+0071 "q" */
    0x3b, 0x67, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x67,
    0x3b, 0x3, 0x3, 0x3, 0x3,

    /* U+0072 "r" */
    0xdf, 0x31, 0x8c, 0x63, 0x18, 0xc0,

    /* U+0073 "s" */
    0x7b, 0xc, 0x38, 0x78, 0x70, 0xe3, 0xf8,

    /* U+0074 "t" */
    0x3, 0x19, 0xf6, 0x31, 0x8c, 0x63, 0x18, 0x70,

    /* U+0075 "u" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xe7,
    0x7b,

    /* U+0076 "v" */
    0xc1, 0x43, 0x62, 0x62, 0x26, 0x34, 0x3c, 0x18,
    0x18,

    /* U+0077 "w" */
    0xc2, 0x14, 0x71, 0x45, 0x36, 0x52, 0x6d, 0x22,
    0x9a, 0x28, 0xe3, 0x8c, 0x10, 0xc0,

    /* U+0078 "x" */
    0x43, 0x66, 0x24, 0x3c, 0x18, 0x1c, 0x24, 0x66,
    0x43,

    /* U+0079 "y" */
    0xc3, 0xc2, 0x42, 0x66, 0x64, 0x24, 0x38, 0x18,
    0x18, 0x10, 0x30, 0x20, 0xc0,

    /* U+007A "z" */
    0xfe, 0x18, 0x30, 0xc1, 0x6, 0x18, 0x20, 0xfe,

    /* U+007B "{" */
    0x36, 0x66, 0x66, 0x68, 0x66, 0x66, 0x66, 0x30,

    /* U+007C "|" */
    0xff, 0xff, 0xff, 0xff, 0xf0,

    /* U+007D "}" */
    0xc6, 0x66, 0x66, 0x61, 0x66, 0x66, 0x66, 0xc0,

    /* U+007E "~" */
    0x71, 0x99, 0x8e,

    /* U+007F "" */
    0x0,

    /* U+5165 "入" */
    0x4, 0x0, 0x2, 0x0, 0x3, 0x0, 0x1, 0x0,
    0x1, 0x80, 0x3, 0x80, 0x2, 0x40, 0x2, 0x40,
    0x6, 0x20, 0xc, 0x30, 0x8, 0x10, 0x18, 0x18,
    0x30, 0xc, 0xe0, 0x7, 0x0, 0x0,

    /* U+5361 "卡" */
    0x2, 0x0, 0x4, 0x0, 0xf, 0xf0, 0x10, 0x0,
    0x20, 0x0, 0x40, 0x3f, 0xff, 0x80, 0x0, 0x2,
    0x0, 0x7, 0x0, 0x9, 0x80, 0x10, 0xc0, 0x20,
    0x0, 0x40, 0x0, 0x80, 0x0,

    /* U+63D2 "插" */
    0x20, 0x0, 0x23, 0xfe, 0x20, 0x20, 0xf8, 0x20,
    0x27, 0xff, 0x20, 0x20, 0x28, 0xa0, 0x3f, 0xae,
    0xe6, 0x22, 0xa6, 0x22, 0x27, 0xae, 0x26, 0x22,
    0x26, 0x22, 0x27, 0xfe, 0xe6, 0x2,

    /* U+672A "未" */
    0x1, 0x0, 0x2, 0x1, 0xff, 0xf0, 0x8, 0x0,
    0x10, 0x0, 0x20, 0x0, 0x40, 0x7f, 0xff, 0x3,
    0x80, 0xa, 0x80, 0x25, 0x81, 0x88, 0xc6, 0x10,
    0xf0, 0x20, 0x40, 0x40, 0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 76, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 80, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 112, .box_w = 4, .box_h = 4, .ofs_x = 2, .ofs_y = 8},
    {.bitmap_index = 6, .adv_w = 163, .box_w = 10, .box_h = 11, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 20, .adv_w = 150, .box_w = 7, .box_h = 16, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 34, .adv_w = 228, .box_w = 13, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 54, .adv_w = 223, .box_w = 13, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 74, .adv_w = 66, .box_w = 2, .box_h = 4, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 75, .adv_w = 86, .box_w = 5, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 85, .adv_w = 86, .box_w = 5, .box_h = 15, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 95, .adv_w = 117, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 100, .adv_w = 190, .box_w = 8, .box_h = 8, .ofs_x = 2, .ofs_y = 1},
    {.bitmap_index = 108, .adv_w = 62, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 110, .adv_w = 111, .box_w = 5, .box_h = 1, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 111, .adv_w = 62, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 112, .adv_w = 109, .box_w = 7, .box_h = 15, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 126, .adv_w = 150, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 138, .adv_w = 150, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 149, .adv_w = 150, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 161, .adv_w = 150, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 172, .adv_w = 150, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 186, .adv_w = 150, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 198, .adv_w = 150, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 210, .adv_w = 150, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 222, .adv_w = 150, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 234, .adv_w = 150, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 246, .adv_w = 62, .box_w = 2, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 249, .adv_w = 62, .box_w = 3, .box_h = 11, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 254, .adv_w = 190, .box_w = 8, .box_h = 8, .ofs_x = 2, .ofs_y = 1},
    {.bitmap_index = 262, .adv_w = 190, .box_w = 8, .box_h = 5, .ofs_x = 2, .ofs_y = 3},
    {.bitmap_index = 267, .adv_w = 190, .box_w = 8, .box_h = 8, .ofs_x = 2, .ofs_y = 1},
    {.bitmap_index = 275, .adv_w = 124, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 286, .adv_w = 264, .box_w = 14, .box_h = 14, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 311, .adv_w = 180, .box_w = 11, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 328, .adv_w = 161, .box_w = 8, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 340, .adv_w = 171, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 354, .adv_w = 195, .box_w = 10, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 369, .adv_w = 141, .box_w = 7, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 380, .adv_w = 136, .box_w = 7, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 391, .adv_w = 190, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 406, .adv_w = 198, .box_w = 9, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 420, .adv_w = 75, .box_w = 4, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 426, .adv_w = 101, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 434, .adv_w = 163, .box_w = 9, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 448, .adv_w = 131, .box_w = 7, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 459, .adv_w = 250, .box_w = 13, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 479, .adv_w = 208, .box_w = 10, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 494, .adv_w = 209, .box_w = 12, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 512, .adv_w = 157, .box_w = 8, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 524, .adv_w = 209, .box_w = 12, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 547, .adv_w = 167, .box_w = 9, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 561, .adv_w = 148, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 573, .adv_w = 147, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 587, .adv_w = 191, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 601, .adv_w = 173, .box_w = 11, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 618, .adv_w = 261, .box_w = 16, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 642, .adv_w = 165, .box_w = 10, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 657, .adv_w = 155, .box_w = 10, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 672, .adv_w = 159, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 686, .adv_w = 86, .box_w = 4, .box_h = 15, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 694, .adv_w = 107, .box_w = 7, .box_h = 15, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 708, .adv_w = 86, .box_w = 4, .box_h = 15, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 716, .adv_w = 190, .box_w = 8, .box_h = 7, .ofs_x = 2, .ofs_y = 5},
    {.bitmap_index = 723, .adv_w = 115, .box_w = 7, .box_h = 1, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 724, .adv_w = 76, .box_w = 3, .box_h = 3, .ofs_x = 1, .ofs_y = 10},
    {.bitmap_index = 726, .adv_w = 142, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 734, .adv_w = 164, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 747, .adv_w = 128, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 755, .adv_w = 164, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 768, .adv_w = 145, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 777, .adv_w = 89, .box_w = 6, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 787, .adv_w = 164, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 800, .adv_w = 158, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 813, .adv_w = 68, .box_w = 2, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 817, .adv_w = 68, .box_w = 5, .box_h = 17, .ofs_x = -1, .ofs_y = -4},
    {.bitmap_index = 828, .adv_w = 139, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 841, .adv_w = 68, .box_w = 2, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 845, .adv_w = 240, .box_w = 12, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 859, .adv_w = 158, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 868, .adv_w = 163, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 879, .adv_w = 164, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 892, .adv_w = 164, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 905, .adv_w = 98, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 911, .adv_w = 119, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 918, .adv_w = 95, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 926, .adv_w = 158, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 935, .adv_w = 134, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 944, .adv_w = 202, .box_w = 12, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 958, .adv_w = 130, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 967, .adv_w = 136, .box_w = 8, .box_h = 13, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 980, .adv_w = 126, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 988, .adv_w = 86, .box_w = 4, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 996, .adv_w = 69, .box_w = 2, .box_h = 18, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 1001, .adv_w = 86, .box_w = 4, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 1009, .adv_w = 190, .box_w = 8, .box_h = 3, .ofs_x = 2, .ofs_y = 4},
    {.bitmap_index = 1012, .adv_w = 76, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1013, .adv_w = 256, .box_w = 16, .box_h = 15, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1043, .adv_w = 256, .box_w = 15, .box_h = 15, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1072, .adv_w = 256, .box_w = 16, .box_h = 15, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1102, .adv_w = 256, .box_w = 15, .box_h = 15, .ofs_x = 0, .ofs_y = -2}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_1[] = {
    0x0, 0x1fc, 0x126d, 0x15c5
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 96, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 20837, .range_length = 5574, .glyph_id_start = 97,
        .unicode_list = unicode_list_1, .glyph_id_ofs_list = NULL, .list_length = 4, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Pair left and right glyphs for kerning*/
static const uint8_t kern_pair_glyph_ids[] =
{
    3, 83,
    3, 84,
    8, 83,
    8, 84,
    9, 75,
    11, 34,
    11, 43,
    11, 68,
    11, 69,
    11, 70,
    11, 72,
    11, 80,
    11, 82,
    34, 11,
    34, 13,
    34, 28,
    34, 36,
    34, 40,
    34, 43,
    34, 48,
    34, 53,
    34, 54,
    34, 55,
    34, 56,
    34, 58,
    34, 59,
    34, 85,
    34, 87,
    34, 88,
    34, 90,
    35, 53,
    35, 58,
    36, 32,
    36, 36,
    36, 40,
    36, 48,
    36, 50,
    37, 13,
    37, 15,
    37, 34,
    37, 53,
    37, 57,
    37, 59,
    38, 34,
    38, 43,
    38, 53,
    38, 56,
    38, 57,
    39, 13,
    39, 15,
    39, 34,
    39, 43,
    39, 52,
    39, 53,
    39, 66,
    39, 71,
    40, 53,
    40, 55,
    40, 90,
    43, 13,
    43, 15,
    43, 34,
    43, 43,
    43, 66,
    44, 13,
    44, 28,
    44, 36,
    44, 40,
    44, 43,
    44, 48,
    44, 50,
    44, 57,
    44, 59,
    44, 68,
    44, 69,
    44, 70,
    44, 72,
    44, 80,
    44, 82,
    44, 85,
    44, 87,
    44, 88,
    44, 90,
    45, 11,
    45, 32,
    45, 34,
    45, 36,
    45, 40,
    45, 43,
    45, 48,
    45, 50,
    45, 53,
    45, 54,
    45, 55,
    45, 56,
    45, 58,
    45, 59,
    45, 85,
    45, 87,
    45, 88,
    45, 90,
    48, 13,
    48, 15,
    48, 34,
    48, 43,
    48, 53,
    48, 57,
    48, 58,
    48, 59,
    49, 13,
    49, 15,
    49, 34,
    49, 40,
    49, 43,
    49, 56,
    49, 57,
    49, 66,
    49, 68,
    49, 69,
    49, 70,
    49, 72,
    49, 80,
    49, 82,
    50, 13,
    50, 15,
    50, 34,
    50, 53,
    50, 57,
    50, 58,
    50, 59,
    51, 28,
    51, 36,
    51, 40,
    51, 43,
    51, 48,
    51, 50,
    51, 53,
    51, 58,
    51, 68,
    51, 69,
    51, 70,
    51, 72,
    51, 80,
    51, 82,
    52, 85,
    52, 87,
    52, 88,
    52, 90,
    53, 13,
    53, 15,
    53, 27,
    53, 28,
    53, 34,
    53, 36,
    53, 40,
    53, 43,
    53, 48,
    53, 50,
    53, 53,
    53, 55,
    53, 56,
    53, 57,
    53, 58,
    53, 66,
    53, 68,
    53, 69,
    53, 70,
    53, 71,
    53, 72,
    53, 78,
    53, 79,
    53, 80,
    53, 81,
    53, 82,
    53, 83,
    53, 84,
    53, 86,
    53, 87,
    53, 88,
    53, 89,
    53, 90,
    53, 91,
    54, 34,
    55, 13,
    55, 15,
    55, 34,
    55, 36,
    55, 40,
    55, 43,
    55, 48,
    55, 50,
    55, 52,
    55, 53,
    55, 66,
    55, 68,
    55, 69,
    55, 70,
    55, 72,
    55, 78,
    55, 79,
    55, 80,
    55, 81,
    55, 82,
    55, 83,
    55, 84,
    55, 86,
    56, 13,
    56, 15,
    56, 34,
    56, 53,
    56, 66,
    56, 68,
    56, 69,
    56, 70,
    56, 72,
    56, 80,
    56, 82,
    57, 13,
    57, 15,
    57, 28,
    57, 36,
    57, 40,
    57, 43,
    57, 48,
    57, 50,
    57, 53,
    58, 13,
    58, 15,
    58, 34,
    58, 36,
    58, 40,
    58, 43,
    58, 48,
    58, 50,
    58, 52,
    58, 53,
    58, 66,
    58, 68,
    58, 69,
    58, 70,
    58, 71,
    58, 72,
    58, 78,
    58, 79,
    58, 80,
    58, 81,
    58, 82,
    58, 83,
    58, 84,
    58, 86,
    59, 43,
    59, 53,
    59, 90,
    60, 75,
    67, 66,
    67, 71,
    67, 89,
    68, 43,
    68, 53,
    68, 58,
    70, 3,
    70, 8,
    71, 10,
    71, 13,
    71, 14,
    71, 15,
    71, 27,
    71, 28,
    71, 32,
    71, 62,
    71, 67,
    71, 73,
    71, 85,
    71, 87,
    71, 88,
    71, 89,
    71, 90,
    71, 94,
    72, 75,
    75, 75,
    76, 13,
    76, 14,
    76, 15,
    76, 27,
    76, 28,
    76, 68,
    76, 69,
    76, 70,
    76, 72,
    76, 80,
    76, 82,
    76, 85,
    79, 3,
    79, 8,
    80, 3,
    80, 8,
    80, 66,
    80, 71,
    80, 89,
    81, 66,
    81, 71,
    81, 89,
    82, 75,
    83, 13,
    83, 14,
    83, 15,
    83, 27,
    83, 28,
    83, 68,
    83, 69,
    83, 70,
    83, 71,
    83, 72,
    83, 78,
    83, 79,
    83, 80,
    83, 82,
    83, 84,
    83, 85,
    83, 87,
    83, 88,
    83, 89,
    83, 90,
    83, 91,
    85, 14,
    85, 32,
    85, 68,
    85, 69,
    85, 70,
    85, 72,
    85, 80,
    85, 82,
    85, 89,
    86, 3,
    86, 8,
    87, 13,
    87, 15,
    87, 66,
    87, 68,
    87, 69,
    87, 70,
    87, 72,
    87, 80,
    87, 82,
    88, 13,
    88, 15,
    88, 68,
    88, 69,
    88, 70,
    88, 72,
    88, 80,
    88, 82,
    89, 68,
    89, 69,
    89, 70,
    89, 72,
    89, 80,
    89, 82,
    90, 3,
    90, 8,
    90, 13,
    90, 15,
    90, 32,
    90, 68,
    90, 69,
    90, 70,
    90, 71,
    90, 72,
    90, 80,
    90, 82,
    90, 85,
    92, 75
};

/* Kerning between the respective left and right glyphs
 * 4.4 format which needs to scaled with `kern_scale`*/
static const int8_t kern_pair_values[] =
{
    -7, -9, -7, -9, 32, -22, -21, -14,
    -14, -14, -14, -14, -14, -17, 9, 9,
    -4, -4, 13, -4, -20, -4, -16, -10,
    -21, 8, -4, -6, -4, -5, -12, -9,
    0, -7, -7, -4, -7, -17, -17, -4,
    -12, -7, -7, 1, 9, 1, 4, 1,
    -21, -21, -18, -9, -4, 2, -10, 1,
    -7, -4, -4, -14, -14, -5, -9, -4,
    5, 5, -12, -12, 12, -12, -12, 5,
    5, -4, -4, -4, -4, -4, -4, -6,
    -10, -7, -12, -28, -14, 8, -9, -9,
    14, -9, -9, -15, -4, -16, -7, -17,
    8, -4, -14, -9, -10, -12, -12, -4,
    -1, -12, -5, -3, -7, -44, -44, -21,
    -1, -17, 5, -8, -9, -10, -10, -10,
    -10, -10, -10, -12, -17, -4, -12, -5,
    -1, -7, 11, -4, -4, 8, -3, -3,
    -7, -5, -7, -7, -8, -8, -8, -7,
    -9, -7, -4, -6, -17, -24, -3, -3,
    -21, -12, -12, -15, -12, -12, 5, 6,
    5, -1, 4, -29, -28, -28, -28, -13,
    -28, -24, -24, -28, -24, -28, -24, -21,
    -24, -14, -15, -24, -15, -17, -5, -28,
    -31, -16, -6, -6, -9, -2, -6, -4,
    5, -20, -17, -17, -17, -17, -10, -10,
    -17, -10, -17, -10, -9, -10, -16, -17,
    -10, 5, -10, -7, -7, -7, -7, -7,
    -7, 9, 8, 11, -3, -3, 13, -3,
    -3, 5, -24, -26, -21, -6, -6, -9,
    -6, -6, -4, 5, -27, -24, -24, -24,
    -4, -24, -19, -19, -24, -19, -24, -19,
    -18, -19, 11, 5, -7, 32, -4, -1,
    -3, 10, -14, -10, -14, -14, 19, -17,
    -14, -17, 11, 11, 9, 19, 2, 2,
    5, 5, 5, 2, 5, 11, 6, 5,
    11, -19, 11, 11, 11, -5, -4, -5,
    -5, -5, -4, -2, -14, -14, -20, -20,
    -4, -5, -3, -4, -5, -3, 14, -21,
    -17, -23, 11, 11, -4, -4, -4, 5,
    -4, 0, 0, -4, -4, 2, 8, 11,
    11, 8, 11, 5, -15, -7, -4, -4,
    -2, -2, -2, -2, 4, -9, -9, -16,
    -17, -5, -2, -2, -2, -2, -2, -2,
    -12, -14, -1, -1, -1, -1, -1, -1,
    -2, -2, -2, -2, -2, -2, 4, 4,
    -14, -17, -10, -1, -1, -1, 1, -1,
    -1, -1, 1, 27
};

/*Collect the kern pair's data in one place*/
static const lv_font_fmt_txt_kern_pair_t kern_pairs =
{
    .glyph_ids = kern_pair_glyph_ids,
    .values = kern_pair_values,
    .pair_cnt = 372,
    .glyph_ids_size = 0
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_pairs,
    .kern_scale = 16,
    .cmap_num = 2,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t ui_font_MSBlack16 = {
#else
lv_font_t ui_font_MSBlack16 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 18,          /*The maximum line height required by the font*/
    .base_line = 4,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if UI_FONT_MSBLACK16*/

