#file(GLOB SOURCES *.c)
file(GLOB_RECURSE SOURCES ./*.c)
file(GLOB_RECURSE UI_SOURCE "../ui/*.c" "../ui/*.cpp")

idf_component_register(SRCS
                        ${SOURCES}
                        ${UI_SOURCE}
                        "../components/wifi/wifi.c"
                        "../components/backlight/backlight.c"
                        "../components/bluetooth/bluetooth.c"
                        "../components/rs485/rs485.c"
                        "../components/timesync/timesync.c"
                       INCLUDE_DIRS
                        "."
                        "../ui"
                        "../ui/components"
                        "../ui/fonts"
                        "../ui/screens"
                       )
