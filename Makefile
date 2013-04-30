STD_PERIPH_DIR ?= ../STM32F10x_StdPeriph_Lib_V3.5.0
KIT ?= ""
PROGRAM = dmx_dimmer


STD_PERIPH_INC = $(STD_PERIPH_DIR)/Libraries/STM32F10x_StdPeriph_Driver/inc
CMSIS_INC = $(STD_PERIPH_DIR)/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
CORE_SUP = $(STD_PERIPH_DIR)/Libraries/CMSIS/CM3/CoreSupport

CROSS_COMPILE = arm-none-eabi-

CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)gcc
AR=$(CROSS_COMPILE)ar
AS=$(CROSS_COMPILE)as
OBJCOPY=$(CROSS_COMPILE)objcopy

ifeq ($(KIT), "discovery")
LINKER_SCRIPT = stm32_flash_128_8.ld
START_CODE = startup_stm32f10x_md_vl.o
CPU_DEF = -DSTM32F10X_MD_VL=1
CFLAGS += -DDISCOVERY_KIT
else
LINKER_SCRIPT = stm32_flash_512_64.ld
START_CODE = startup_stm32f10x_hd.o
CPU_DEF = -DSTM32F10X_HD=1
endif

# even linker has to got these, it can generate wrong istructions anyway (blx)
CPU_FLAGS = -mcpu=cortex-m3 -mthumb -mfloat-abi=soft $(CPU_DEF)

CFLAGS += $(CPU_FLAGS) -DUSE_STDPERIPH_DRIVER
CFLAGS += -I$(STD_PERIPH_INC) -I$(CMSIS_INC) -I$(CORE_SUP) -I.
CFLAGS += -c -g -O0 -Wall
ASFLAGS += -g -gstabs

LDFLAGS = -T $(LINKER_SCRIPT) -L.

CC_DEPFLAGS = -MMD -MF $(@:.o=.d) -MT $@

vpath %.c $(STD_PERIPH_DIR)/Libraries/STM32F10x_StdPeriph_Driver/src

OBJS  = main.o stm32f10x_it.o system_stm32f10x.o $(START_CODE)
OBJS += dmx512_rec.o 
OBJS += serial_tracer.o 

LIB_OBJS =  stm32f10x_dbgmcu.o	stm32f10x_pwr.o \
        	misc.o				stm32f10x_dma.o		stm32f10x_rcc.o \
			stm32f10x_adc.o		stm32f10x_exti.o	stm32f10x_rtc.o \
			stm32f10x_flash.o	stm32f10x_sdio.o \
			stm32f10x_spi.o \
			stm32f10x_gpio.o	stm32f10x_tim.o \
			stm32f10x_usart.o \
			stm32f10x_iwdg.o	stm32f10x_wwdg.o

all: $(PROGRAM).elf $(PROGRAM).bin

$(PROGRAM).elf: $(OBJS) libstm32.a
	$(LD) $(CPU_FLAGS) $(LDFLAGS) -o $@ $^ -lstm32

$(PROGRAM).bin: $(PROGRAM).elf
	$(OBJCOPY) -O binary $< $@ 

lib: libstm32.a

libstm32.a: $(LIB_OBJS)
	$(AR) rcs $@ $^

clean:
	rm -rf *.o *.a *.elf *.bin *.d

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(CC_DEPFLAGS) -c -o $@ $<


-include $(wildcard $(OBJS:.o=.d) $(TESTOBJS:.o=.d))
