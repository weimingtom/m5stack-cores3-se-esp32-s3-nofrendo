file(GLOB_RECURSE T_SOURCES *.c)
file(GLOB_RECURSE IMAGES images/*.c)

#idf_component_register(
#    SRCS ${T_SOURCES} ${IMAGES}
#    INCLUDE_DIRS "."
#    PRIV_REQUIRES driver nofrendo nvs_flash
#)
#hid

################

#file(GLOB_RECURSE T_SOURCES *.c)
file(GLOB_RECURSE CPU_SOURCES cpu/*.c)
file(GLOB_RECURSE NSS_SOURCES libsnss/*.c)
file(GLOB_RECURSE NES_SOURCES nes/*.c)
file(GLOB_RECURSE SNDHRDW_SOURCES sndhrdw/*.c)
file(GLOB_RECURSE MAP_SOURCES mappers/*.c)

#idf_component_register(SRCS ${T_SOURCES} ${CPU_SOURCES} ${NSS_SOURCES} ${NES_SOURCES} ${SNDHRDW_SOURCES} ${MAP_SOURCES} INCLUDE_DIRS "cpu" "libsnss" "nes" "sndhrdw" ".")


################

set(LV_DEMO_DIR "")
set(LV_DEMOS_SOURCES "")
if(CONFIG_LV_USE_DEMO_BENCHMARK)
    list(APPEND LV_DEMO_DIR ../managed_components/lvgl__lvgl/demos)
    file(GLOB_RECURSE LV_DEMOS_SOURCES ${LV_DEMO_DIR}/*.c)
endif()

idf_component_register(
    SRCS "main.c" "osd.c" "m5_cores3_keyboard.cpp" ${LV_DEMOS_SOURCES} ${T_SOURCES} ${IMAGES} 
	#${T_SOURCES} 
	${CPU_SOURCES} ${NSS_SOURCES} ${NES_SOURCES} ${SNDHRDW_SOURCES} ${MAP_SOURCES}
    INCLUDE_DIRS "cpu" "libsnss" "nes" "sndhrdw" "." ${LV_DEMO_DIR} 
    PRIV_REQUIRES nvs_flash m5_tab5_keyboard_component
)
#driver 
#nofrendo

if(CONFIG_LV_USE_DEMO_BENCHMARK)
set_source_files_properties(
    ${LV_DEMOS_SOURCES}
    PROPERTIES COMPILE_OPTIONS
    -DLV_LVGL_H_INCLUDE_SIMPLE)
endif()
