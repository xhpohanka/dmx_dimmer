
STD_PERIPH_DIR ?= /home/honza/_dev/stm32/STM32F10x_StdPeriph_Lib_V3.5.0
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

CFLAGS  = -fno-common -mcpu=cortex-m3 -mthumb -DSTM32F10X_HD=1 -DUSE_STDPERIPH_DRIVER=1
CFLAGS += -I$(STD_PERIPH_INC) -I$(CMSIS_INC) -I$(CORE_SUP) -I.
CFLAGS += -g -O1 

LDFLAGS = -T stm32_flash.ld -L.

CC_DEPFLAGS = -MMD -MF $(@:.o=.d) -MT $@

vpath %.c $(STD_PERIPH_DIR)/Libraries/STM32F10x_StdPeriph_Driver/src

OBJS = main.o stm32f10x_it.o system_stm32f10x.o startup_stm32f10x_hd.o

LIB_OBJS =  stm32f10x_dbgmcu.o	stm32f10x_pwr.o \
        	misc.o				stm32f10x_dma.o		stm32f10x_rcc.o \
			stm32f10x_adc.o		stm32f10x_exti.o	stm32f10x_rtc.o \
			stm32f10x_bkp.o		stm32f10x_flash.o	stm32f10x_sdio.o \
			stm32f10x_can.o		stm32f10x_fsmc.o	stm32f10x_spi.o \
			stm32f10x_cec.o		stm32f10x_gpio.o	stm32f10x_tim.o \
			stm32f10x_crc.o		stm32f10x_i2c.o		stm32f10x_usart.o \
			stm32f10x_dac.o		stm32f10x_iwdg.o	stm32f10x_wwdg.o

all: $(PROGRAM).elf $(PROGRAM).bin

$(PROGRAM).elf: $(OBJS) libstm32.a
	$(LD) $(LDFLAGS) -o $@ $^ -lstm32

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
