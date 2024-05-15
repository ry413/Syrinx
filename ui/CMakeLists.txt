SET(SOURCES screens/ui_Main_Window.c
    screens/ui_Music_Window.c
    screens/ui_Nature_Sound_Window.c
    screens/ui_Bluetooth_WIndow.c
    screens/ui_Mode_WIndow.c
    screens/ui_Wake_up_Window.c
    screens/ui_Guide_Window.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    images/ui_img_861711258.c
    images/ui_img_490182776.c
    images/ui_img_2087007852.c
    images/ui_img_227802216.c
    images/ui_img_907415264.c
    images/ui_img_501316714.c
    images/ui_img_967182345.c
    images/ui_img_72497716.c
    images/ui_img_411136687.c
    images/ui_img_348826415.c
    images/ui_img_12669592.c
    images/ui_img_2034599484.c
    images/ui_img_304460963.c
    images/ui_img_1893023884.c
    images/ui_img_720053126.c
    images/ui_img_720048778.c
    images/ui_img_720023698.c
    images/ui_img_720017300.c
    images/ui_img_720032643.c
    images/ui_img_720045703.c
    images/ui_img_1526512104.c
    fonts/ui_font_LanTIng20.c
    fonts/ui_font_LanTing18.c
    fonts/ui_font_LanTingFine24.c)

add_library(ui ${SOURCES})
