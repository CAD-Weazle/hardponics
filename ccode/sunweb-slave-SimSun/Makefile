# sources
CSRC += system_stm32f0xx.c
CSRC += main.c
CSRC += power.c  
CSRC += clock.c 
CSRC += ports.c
CSRC += ticker.c
CSRC += rtc.c
CSRC += flash.c
CSRC += uart1.c
CSRC += uart2.c
CSRC += printf.c
CSRC += printf2.c
//CSRC += ./cmath/powten.c ./cmath/atof.c ./cmath/dtoa.c 
CSRC += cli.c
CSRC += adc.c
CSRC += leds.c
CSRC += ntc.c
CSRC += simsun.c
CSRC += sunweb-slave.c
CSRC += dump.c

# project name
TARGET=simsun

###################################################

CCPATH  = d:/gcc/bin/
CC      = $(CCPATH)arm-none-eabi-gcc
OBJCOPY = $(CCPATH)arm-none-eabi-objcopy
OBJDUMP = $(CCPATH)arm-none-eabi-objdump
SIZE    = $(CCPATH)arm-none-eabi-size

CFLAGS  = -std=gnu99 -g -O2 -Wall -TSTM32F042F6Px_FLASH.ld  # own board
#CFLAGS  = -std=gnu99 -g -O2 -Wall -TSTM32F042K6Tx_FLASH.ld  # Nucleo-32 board
CFLAGS += -mlittle-endian -mthumb -mthumb-interwork -nostartfiles -mcpu=cortex-m0 -mthumb-interwork
CFLAGS += -fsingle-precision-constant 
CFLAGS += -mfloat-abi=soft
CFLAGS += -DSTM32F042x6

###################################################

vpath %.c src
vpath %.a lib

ROOT=$(shell pwd)

CFLAGS += -Iinc -Iinc/hal -Ilib -Ilib/inc  -Ilibraries/inc -Iinc/core -Iown_inc
#CFLAGS += -MD -MP -MF .dep/$(@F).d -- ottie
#CFLAGS += -I../platform/MCU/STM32L1xx/peripherals/libraries
#CFLAGS += -I../platform/MCU/STM32L1xx/peripherals/libraries/inc
#CFLAGS += -I../platform/ARM_CM4/CMSIS 

AFLAGS = -Iinc

ASRC += startup_stm32f042x6.S  # add startup file to build

OBJS = $(CSRC:.c=.o) $(ASRC:.s=.o)

GENDEPFLAGS = -MD -MP -MF .dep/$(@F).d

#-include $(wildcard .dep/*) -- ottie

###################################################

.PHONY: lib proj

#JAR = ../main.hex
HEX = $(TARGET).hex
ELF = $(TARGET).elf
DIS = $(TARGET).dis

COMM = ../comm/comm.hex

all: proj

list:
	echo C sources
	@echo $(CSRC)
	echo
	echo ASM sources
	@echo $(ASRC)
	echo
	echo Objects
	@echo $(OBJS)

lib:
	$(MAKE) -C lib

#proj: $(ELF) $(DIS) $(HEX) # -- trips over 'arm-none-eabi-objdump', remove $(DIS)
proj: $(ELF) $(HEX)

$(DIS): $(ELF)
	$(OBJDUMP) -d $(ELF) > $(DIS)
	$(OBJDUMP) -D $(ELF) > $(DIS).all

$(ALL): $(HEX) $(COMM)
	./ihex -i $(HEX) -i $(COMM) -V 0.1.1 -o $(ALL)

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex --set-start=0 $(ELF) $(HEX)

$(ELF): $(OBJS) 
	$(CC) $(LDFLAGS) $(GENDEPFLAGS) $(CFLAGS) $(OBJS) -o $@ -lm -Wl,-Map=simsun.map

%.o: %.c
	@echo $(CC) $@
	@$(CC) $(GENDEPFLAGS) $(CFLAGS) -c $^ -o $@ 

%.o: %.S
	@echo $(CC) $@
	@$(CC) $(GENDEPFLAGS) $(AFLAGS) -c $^ -o $@ 

debug: 
	openocd -f stm32f4discovery.cfg

prog: proj
	openocd -f stm32f4discovery.cfg -c "program $(HEX) reset exit"
	@modem term

progall: proj
	openocd -f stm32f4discovery.cfg -c "program $(ALL) reset exit"
	@modem term

clean:
	rm -f *.o net/*.o
	rm -f $(ELF) $(DIS)
	rm -f $(TARGET).bin
	
