#-------------------------------------------------------------
# Lab. processadores I
# MakEfile para compilar o programas no kit msp430
#-----------------------------------------------------------
# Nomes especificos para o projeto
NAME		= teste
CSOURCES	= teste.c mprintf.c lcd_LP.c
ASOURCES	=
HEADERS		= mprintf.h lcd_LP.h

# Nomes das ferramentas
CPU             = msp430x2619
CFLAGS          = -mmcu=$(CPU) -O2 -Wall
AFLAGS			= -mmcu=$(CPU)
CC              = msp430-gcc
AS				= msp430-as
BSLCMD			= msp430-bsl --invert-reset --invert-test -epr -r

# Alvos e diretivas genericas
# Define object files.
COBJ = $(CSOURCES:.c=.o)
AOBJ = $(ASOURCES:.S=.o)
OBJECTS = $(COBJ) $(AOBJ)

all: $(NAME).elf $(NAME).hex $(NAME).lst

# Chama o compilador c
%.o: %.c $(HEADERS)
	$(CC) -c $(CFLAGS) -o $@ $<

# Chama o assembler
%.o: %.S
	$(AS) $(AFLAGS) -o $@ $<

$(NAME).elf: $(OBJECTS)
	$(CC) -mmcu=$(CPU) -o $@ $(OBJECTS)

$(NAME).hex: $(NAME).elf
	msp430-objcopy -O ihex $^ $@
#Gera o arquivo de listagem (programa em assembly)
$(NAME).lst: $(NAME).elf
	msp430-objdump -dSt $^ >$@
# Transmite para o kit (so em Linux)
bsl: $(NAME).hex
	msp430-bsl-demo /dev/ttyUSB0 $(NAME).hex
# Transmite usando o msp430-bsl em python (usar no Windows)
#gmake bsl[numero da com]
bsl1: $(NAME).hex
	$(BSLCMD) -c5 $(NAME).hex
bsl2: $(NAME).hex
	$(BSLCMD) -c5 $(NAME).hex
bsl3: $(NAME).hex
	$(BSLCMD) -c2 $(NAME).hex
bsl4: $(NAME).hex
	$(BSLCMD) -c3 $(NAME).hex
bsl5: $(NAME).hex
	$(BSLCMD) -c4 $(NAME).hex
bsl6: $(NAME).hex
	$(BSLCMD) -c5 $(NAME).hex
bsl7: $(NAME).hex
	$(BSLCMD) -c6 $(NAME).hex
# Apaga os arquivos gerados pela compilacao
clean:
	rm -f $(NAME).elf $(NAME).hex $(NAME).lst $(OBJECTS) *~
