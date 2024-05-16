#file(GLOB SOURCES *.c)
file(GLOB_RECURSE SOURCES ./*.c)
file(GLOB_RECURSE UI_SOURCE "../ui/*.c")
file(GLOB_RECURSE UI_SOURCE "../uis/*.c")

idf_component_register(SRCS ${SOURCES} ${UI_SOURCE} "../components/wifi/wifi.c" "../components/backlight/backlight.c"
                       INCLUDE_DIRS . "../ui" "../ui/components" "../ui/fonts" "../ui/screens" "../uis" "../uis/components" "../uis/screens" "../uis/fonts"
                       )
