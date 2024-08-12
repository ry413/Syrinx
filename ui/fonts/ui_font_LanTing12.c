/*******************************************************************************
 * Size: 12 px
 * Bpp: 1
 * Opts: --bpp 1 --size 12 --font /Users/ry79/SquareLine/assets/方正兰亭黑.TTF -o /Users/ry79/SquareLine/assets/ui_font_LanTing12.c --format lvgl -r 0x20-0x7f --symbols 小时分钟后响铃 --no-compress --no-prefilter
 ******************************************************************************/

#include "../ui.h"

#ifndef UI_FONT_LANTING12
#define UI_FONT_LANTING12 1
#endif

#if UI_FONT_LANTING12

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xfe, 0x40,

    /* U+0022 "\"" */
    0xb6, 0x80,

    /* U+0023 "#" */
    0x14, 0x28, 0x93, 0xf2, 0x85, 0x3f, 0x24, 0x48,
    0xa0,

    /* U+0024 "$" */
    0x23, 0xaf, 0x5a, 0x30, 0xe5, 0xad, 0x5c, 0x42,
    0x0,

    /* U+0025 "%" */
    0x1, 0x18, 0x49, 0x22, 0x48, 0x94, 0x19, 0x0,
    0x98, 0x29, 0x12, 0x44, 0x92, 0x18,

    /* U+0026 "&" */
    0x38, 0x22, 0x11, 0xd, 0x83, 0x2, 0xd2, 0x39,
    0x4, 0x87, 0x3c, 0x80,

    /* U+0027 "'" */
    0xe0,

    /* U+0028 "(" */
    0x29, 0x49, 0x24, 0x91, 0x22,

    /* U+0029 ")" */
    0x89, 0x12, 0x49, 0x25, 0x28,

    /* U+002A "*" */
    0x21, 0x5c, 0xa5, 0x0,

    /* U+002B "+" */
    0x10, 0x20, 0x47, 0xf1, 0x2, 0x4, 0x0,

    /* U+002C "," */
    0xd4,

    /* U+002D "-" */
    0xfe,

    /* U+002E "." */
    0x80,

    /* U+002F "/" */
    0x22, 0x22, 0x44, 0x44, 0x88,

    /* U+0030 "0" */
    0x79, 0x28, 0x61, 0x86, 0x18, 0x61, 0x49, 0xe0,

    /* U+0031 "1" */
    0x13, 0x79, 0x11, 0x11, 0x11,

    /* U+0032 "2" */
    0x79, 0x18, 0x41, 0xc, 0x62, 0x10, 0xc3, 0xf0,

    /* U+0033 "3" */
    0x39, 0x14, 0x41, 0x38, 0x20, 0x61, 0x4d, 0xe0,

    /* U+0034 "4" */
    0x4, 0x18, 0x30, 0xa2, 0x44, 0x91, 0x7f, 0x4,
    0x8,

    /* U+0035 "5" */
    0x7d, 0x8, 0x2e, 0xcc, 0x10, 0x61, 0xcd, 0xe0,

    /* U+0036 "6" */
    0x39, 0x18, 0x2e, 0xce, 0x18, 0x61, 0x4d, 0xe0,

    /* U+0037 "7" */
    0xfc, 0x30, 0x84, 0x10, 0xc2, 0x8, 0x21, 0x80,

    /* U+0038 "8" */
    0x38, 0x89, 0x12, 0x23, 0x88, 0x90, 0xe1, 0x46,
    0x78,

    /* U+0039 "9" */
    0x7b, 0x28, 0x61, 0x87, 0x37, 0x41, 0xc9, 0xc0,

    /* U+003A ":" */
    0x82,

    /* U+003B ";" */
    0xf0, 0xd, 0x40,

    /* U+003C "<" */
    0x2, 0x18, 0xc3, 0x8, 0xc, 0xc, 0x6, 0x2,

    /* U+003D "=" */
    0xfc, 0x0, 0x3f,

    /* U+003E ">" */
    0x81, 0x80, 0xc0, 0x60, 0x21, 0x8c, 0x60, 0x80,

    /* U+003F "?" */
    0x7a, 0x28, 0x82, 0x8, 0x42, 0x8, 0x0, 0x80,

    /* U+0040 "@" */
    0x1f, 0x83, 0x6, 0x40, 0x2c, 0xf9, 0x99, 0x99,
    0x11, 0x91, 0x19, 0x32, 0x9c, 0xcc, 0x0, 0x40,
    0x23, 0x4, 0x1f, 0x80,

    /* U+0041 "A" */
    0x8, 0xc, 0x5, 0x6, 0x82, 0x21, 0x11, 0xf8,
    0x82, 0x41, 0x40, 0xc0,

    /* U+0042 "B" */
    0xfd, 0xe, 0x1c, 0x2f, 0x90, 0xe0, 0xc1, 0x87,
    0xf8,

    /* U+0043 "C" */
    0x3c, 0x62, 0xc3, 0x80, 0x80, 0x80, 0x80, 0xc3,
    0x42, 0x3c,

    /* U+0044 "D" */
    0xfc, 0x86, 0x83, 0x81, 0x81, 0x81, 0x81, 0x83,
    0x86, 0xfc,

    /* U+0045 "E" */
    0xff, 0x2, 0x4, 0xf, 0xd0, 0x20, 0x40, 0x81,
    0xfc,

    /* U+0046 "F" */
    0xfe, 0x8, 0x20, 0xfe, 0x8, 0x20, 0x82, 0x0,

    /* U+0047 "G" */
    0x3e, 0x31, 0xb0, 0x70, 0x8, 0x4, 0x3e, 0x3,
    0x81, 0x61, 0x9f, 0x0,

    /* U+0048 "H" */
    0x83, 0x6, 0xc, 0x1f, 0xf0, 0x60, 0xc1, 0x83,
    0x4,

    /* U+0049 "I" */
    0xff, 0xc0,

    /* U+004A "J" */
    0x8, 0x42, 0x10, 0x84, 0x31, 0x8b, 0x80,

    /* U+004B "K" */
    0x82, 0x84, 0x88, 0x90, 0xb0, 0xd0, 0x88, 0x84,
    0x86, 0x82,

    /* U+004C "L" */
    0x82, 0x8, 0x20, 0x82, 0x8, 0x20, 0x83, 0xf0,

    /* U+004D "M" */
    0xc1, 0xe0, 0xf0, 0x74, 0x5a, 0x2d, 0x96, 0x53,
    0x29, 0x88, 0xc4, 0x40,

    /* U+004E "N" */
    0x83, 0x86, 0x8d, 0x19, 0x32, 0x62, 0xc5, 0x87,
    0x4,

    /* U+004F "O" */
    0x3e, 0x31, 0xb0, 0x70, 0x18, 0xc, 0x6, 0x3,
    0x83, 0x63, 0x1f, 0x0,

    /* U+0050 "P" */
    0xfd, 0xe, 0xc, 0x18, 0x7f, 0xa0, 0x40, 0x81,
    0x0,

    /* U+0051 "Q" */
    0x3e, 0x31, 0xb0, 0x50, 0x18, 0xc, 0x6, 0x3,
    0x83, 0x63, 0x1f, 0x1, 0x80, 0x60, 0x0,

    /* U+0052 "R" */
    0xfc, 0x82, 0x82, 0x82, 0xfc, 0x88, 0x88, 0x84,
    0x86, 0x82,

    /* U+0053 "S" */
    0x79, 0xe, 0xc, 0x7, 0x1, 0x80, 0xc1, 0xc6,
    0xf8,

    /* U+0054 "T" */
    0xfe, 0x20, 0x40, 0x81, 0x2, 0x4, 0x8, 0x10,
    0x20,

    /* U+0055 "U" */
    0x83, 0x6, 0xc, 0x18, 0x30, 0x60, 0xc1, 0x44,
    0x70,

    /* U+0056 "V" */
    0xc0, 0xa0, 0x90, 0x4c, 0x62, 0x21, 0x10, 0x50,
    0x28, 0x14, 0x4, 0x0,

    /* U+0057 "W" */
    0x86, 0x18, 0x61, 0x46, 0x24, 0xa2, 0x49, 0x24,
    0x92, 0x29, 0x43, 0xc, 0x30, 0xc3, 0xc,

    /* U+0058 "X" */
    0x41, 0x31, 0x8, 0x82, 0x81, 0x80, 0xe0, 0x50,
    0x44, 0x43, 0x60, 0x80,

    /* U+0059 "Y" */
    0x41, 0xa0, 0x88, 0x86, 0xc1, 0x40, 0x40, 0x20,
    0x10, 0x8, 0x4, 0x0,

    /* U+005A "Z" */
    0x7f, 0x2, 0x6, 0x4, 0x8, 0x10, 0x10, 0x20,
    0x40, 0xff,

    /* U+005B "[" */
    0xf2, 0x49, 0x24, 0x92, 0x4e,

    /* U+005C "\\" */
    0x88, 0x44, 0x44, 0x22, 0x22,

    /* U+005D "]" */
    0xe4, 0x92, 0x49, 0x24, 0x9e,

    /* U+005E "^" */
    0x10, 0xc3, 0x12, 0x49, 0x10,

    /* U+005F "_" */
    0xff,

    /* U+0060 "`" */
    0xa4,

    /* U+0061 "a" */
    0x7b, 0x10, 0x5f, 0x86, 0x37, 0x40,

    /* U+0062 "b" */
    0x82, 0x8, 0x2e, 0xce, 0x18, 0x61, 0xce, 0xe0,

    /* U+0063 "c" */
    0x7b, 0x38, 0x20, 0x83, 0x37, 0x80,

    /* U+0064 "d" */
    0x4, 0x10, 0x5d, 0xce, 0x18, 0x61, 0xcd, 0xd0,

    /* U+0065 "e" */
    0x7b, 0x38, 0x7f, 0x83, 0x37, 0x80,

    /* U+0066 "f" */
    0x74, 0x4e, 0x44, 0x44, 0x44,

    /* U+0067 "g" */
    0x77, 0x38, 0x61, 0x87, 0x37, 0x41, 0x8d, 0xe0,

    /* U+0068 "h" */
    0x82, 0x8, 0x2e, 0xc6, 0x18, 0x61, 0x86, 0x10,

    /* U+0069 "i" */
    0x9f, 0xc0,

    /* U+006A "j" */
    0x24, 0x12, 0x49, 0x24, 0x9e,

    /* U+006B "k" */
    0x82, 0x8, 0x22, 0x92, 0x8e, 0x24, 0x9a, 0x20,

    /* U+006C "l" */
    0xff, 0xc0,

    /* U+006D "m" */
    0xb3, 0x66, 0x62, 0x31, 0x18, 0x8c, 0x46, 0x22,

    /* U+006E "n" */
    0xbb, 0x18, 0x61, 0x86, 0x18, 0x40,

    /* U+006F "o" */
    0x7b, 0x38, 0x61, 0x87, 0x37, 0x80,

    /* U+0070 "p" */
    0xbb, 0x38, 0x61, 0x87, 0x3b, 0xa0, 0x82, 0x0,

    /* U+0071 "q" */
    0x77, 0x38, 0x61, 0x87, 0x37, 0x41, 0x4, 0x10,

    /* U+0072 "r" */
    0xbc, 0x88, 0x88, 0x80,

    /* U+0073 "s" */
    0x74, 0xe0, 0xe0, 0xc5, 0xc0,

    /* U+0074 "t" */
    0x9, 0x74, 0x92, 0x4c,

    /* U+0075 "u" */
    0x86, 0x18, 0x61, 0x86, 0x37, 0x40,

    /* U+0076 "v" */
    0x85, 0x14, 0x92, 0x28, 0xc3, 0x0,

    /* U+0077 "w" */
    0x8c, 0x93, 0x24, 0xc9, 0x54, 0x55, 0xc, 0xc3,
    0x30,

    /* U+0078 "x" */
    0x45, 0x22, 0x84, 0x29, 0x2c, 0x40,

    /* U+0079 "y" */
    0x85, 0x14, 0x52, 0x28, 0xc1, 0x4, 0x23, 0x80,

    /* U+007A "z" */
    0xfc, 0x21, 0xc, 0x21, 0xf, 0xc0,

    /* U+007B "{" */
    0x34, 0x44, 0x44, 0x84, 0x44, 0x44, 0x30,

    /* U+007C "|" */
    0xff, 0xf8,

    /* U+007D "}" */
    0xc2, 0x22, 0x22, 0x12, 0x22, 0x22, 0xc0,

    /* U+007E "~" */
    0xf0, 0x3c,

    /* U+5206 "分" */
    0x9, 0x1, 0x10, 0x10, 0x82, 0xc, 0x40, 0x33,
    0xf8, 0x8, 0x80, 0x88, 0x18, 0x81, 0x8, 0x67,
    0x0,

    /* U+540E "后" */
    0x0, 0x4f, 0xf1, 0x0, 0x20, 0x7, 0xfe, 0x80,
    0x17, 0xf2, 0x82, 0x50, 0x52, 0xa, 0x7f, 0x0,

    /* U+54CD "响" */
    0x2, 0x0, 0x43, 0xbf, 0xd4, 0x1a, 0xbb, 0x59,
    0x6b, 0x2d, 0x65, 0xeb, 0xb5, 0x4, 0x27, 0x0,

    /* U+5C0F "小" */
    0x4, 0x0, 0x80, 0x10, 0x12, 0x46, 0x48, 0x88,
    0x91, 0x14, 0x21, 0x84, 0x0, 0x80, 0x70, 0x0,

    /* U+65F6 "时" */
    0x0, 0x9e, 0x12, 0x42, 0x4f, 0xf9, 0x9, 0xe9,
    0x25, 0x24, 0x94, 0x90, 0x9e, 0x12, 0x4e, 0x0,

    /* U+949F "钟" */
    0x21, 0x8, 0x21, 0xc4, 0x67, 0xff, 0x92, 0x52,
    0x4a, 0x4f, 0xff, 0x21, 0x6, 0x20, 0x84, 0x0,

    /* U+94C3 "铃" */
    0x21, 0x4, 0x21, 0xca, 0x22, 0x2f, 0x92, 0x40,
    0x3f, 0xf1, 0x2, 0x24, 0x86, 0x60, 0x82, 0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 53, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 58, .box_w = 1, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 3, .adv_w = 73, .box_w = 3, .box_h = 3, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 5, .adv_w = 116, .box_w = 7, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 14, .adv_w = 121, .box_w = 5, .box_h = 13, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 23, .adv_w = 186, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 37, .adv_w = 148, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 49, .adv_w = 39, .box_w = 1, .box_h = 3, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 50, .adv_w = 69, .box_w = 3, .box_h = 13, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 55, .adv_w = 69, .box_w = 3, .box_h = 13, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 60, .adv_w = 84, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 64, .adv_w = 125, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 71, .adv_w = 55, .box_w = 2, .box_h = 3, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 72, .adv_w = 125, .box_w = 7, .box_h = 1, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 73, .adv_w = 60, .box_w = 1, .box_h = 1, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 74, .adv_w = 56, .box_w = 4, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 79, .adv_w = 117, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 87, .adv_w = 117, .box_w = 4, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 92, .adv_w = 114, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 100, .adv_w = 116, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 108, .adv_w = 117, .box_w = 7, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 117, .adv_w = 116, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 125, .adv_w = 117, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 133, .adv_w = 116, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 141, .adv_w = 118, .box_w = 7, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 150, .adv_w = 117, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 158, .adv_w = 59, .box_w = 1, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 159, .adv_w = 59, .box_w = 2, .box_h = 9, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 162, .adv_w = 125, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 170, .adv_w = 122, .box_w = 6, .box_h = 4, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 173, .adv_w = 124, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 181, .adv_w = 116, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 189, .adv_w = 213, .box_w = 12, .box_h = 13, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 209, .adv_w = 141, .box_w = 9, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 221, .adv_w = 140, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 230, .adv_w = 152, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 240, .adv_w = 151, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 250, .adv_w = 140, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 259, .adv_w = 128, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 267, .adv_w = 162, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 279, .adv_w = 151, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 288, .adv_w = 55, .box_w = 1, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 290, .adv_w = 104, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 297, .adv_w = 140, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 307, .adv_w = 117, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 315, .adv_w = 175, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 327, .adv_w = 151, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 336, .adv_w = 164, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 348, .adv_w = 140, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 357, .adv_w = 164, .box_w = 9, .box_h = 13, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 372, .adv_w = 151, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 382, .adv_w = 140, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 391, .adv_w = 128, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 400, .adv_w = 152, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 409, .adv_w = 142, .box_w = 9, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 421, .adv_w = 199, .box_w = 12, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 436, .adv_w = 141, .box_w = 9, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 448, .adv_w = 140, .box_w = 9, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 460, .adv_w = 128, .box_w = 8, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 470, .adv_w = 56, .box_w = 3, .box_h = 13, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 475, .adv_w = 56, .box_w = 4, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 480, .adv_w = 56, .box_w = 3, .box_h = 13, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 485, .adv_w = 98, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 490, .adv_w = 124, .box_w = 8, .box_h = 1, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 491, .adv_w = 66, .box_w = 2, .box_h = 3, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 492, .adv_w = 117, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 498, .adv_w = 116, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 506, .adv_w = 106, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 512, .adv_w = 116, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 520, .adv_w = 117, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 526, .adv_w = 59, .box_w = 4, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 531, .adv_w = 117, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 539, .adv_w = 116, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 547, .adv_w = 44, .box_w = 1, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 549, .adv_w = 44, .box_w = 3, .box_h = 13, .ofs_x = -1, .ofs_y = -3},
    {.bitmap_index = 554, .adv_w = 104, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 562, .adv_w = 44, .box_w = 1, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 564, .adv_w = 173, .box_w = 9, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 572, .adv_w = 117, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 578, .adv_w = 117, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 584, .adv_w = 116, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 592, .adv_w = 116, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 600, .adv_w = 73, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 604, .adv_w = 108, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 609, .adv_w = 58, .box_w = 3, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 613, .adv_w = 116, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 619, .adv_w = 104, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 625, .adv_w = 151, .box_w = 10, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 634, .adv_w = 104, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 640, .adv_w = 104, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 648, .adv_w = 104, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 654, .adv_w = 69, .box_w = 4, .box_h = 13, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 661, .adv_w = 52, .box_w = 1, .box_h = 13, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 663, .adv_w = 69, .box_w = 4, .box_h = 13, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 670, .adv_w = 122, .box_w = 7, .box_h = 2, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 672, .adv_w = 192, .box_w = 12, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 689, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 705, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 721, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 737, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 753, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 769, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -1}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_1[] = {
    0x0, 0x208, 0x2c7, 0xa09, 0x13f0, 0x4299, 0x42bd
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 20998, .range_length = 17086, .glyph_id_start = 96,
        .unicode_list = unicode_list_1, .glyph_id_ofs_list = NULL, .list_length = 7, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
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
    .kern_dsc = NULL,
    .kern_scale = 0,
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
const lv_font_t ui_font_LanTing12 = {
#else
lv_font_t ui_font_LanTing12 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 14,          /*The maximum line height required by the font*/
    .base_line = 3,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if UI_FONT_LANTING12*/

