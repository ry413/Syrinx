SET(SOURCES components/ui_comp_music_item1.c
    components/ui_comp.c
    screens/ui_Settings.c
    screens/ui_Backlight_Settings.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    images/ui_img_861711258.c
    images/ui_img_490182776.c
    images/ui_img_149594991.c
    images/ui_img_1830113796.c
    images/ui_img_348826415.c
    images/ui_img_404002205.c
    images/ui_img_239308628.c
    fonts/ui_font_LanTIng20.c
    fonts/ui_font_LanTing18.c
    fonts/ui_font_LanTingFine24.c
    fonts/ui_font_LanTingFine18.c
    fonts/ui_font_LanTingFine22.c)

add_library(ui ${SOURCES})
