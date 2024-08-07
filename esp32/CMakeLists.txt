set(srcs "bsp.c")
set(requires driver)

if(CONFIG_USE_BSP_USART_FIFO)
    list(APPEND srcs "bsp_usart_fifo.c")
endif()

idf_component_register(SRCS "${srcs}"
            INCLUDE_DIRS 
            inc
            REQUIRES
            ${requires})