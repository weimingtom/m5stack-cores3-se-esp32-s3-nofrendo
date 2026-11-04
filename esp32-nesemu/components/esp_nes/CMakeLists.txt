file(GLOB_RECURSE T_SOURCES *.c)
file(GLOB_RECURSE IMAGES images/*.c)

idf_component_register(
    SRCS ${T_SOURCES} ${IMAGES} m5_cores3_keyboard3.cpp
    INCLUDE_DIRS "."
    PRIV_REQUIRES driver nofrendo nvs_flash M5Faces
)
#hid
