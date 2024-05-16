idf_component_register(SRCS "backlight.c"
                       INCLUDE_DIRS ".")
target_include_directories(${COMPONENT_LIB} PUBLIC 
    $ENV{IDF_PATH}/components/driver/ledc/include
    $ENV{IDF_PATH}/components/driver/gpio/include
    $ENV{IDF_PATH}/components/nvs_flash/include
)
