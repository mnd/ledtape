CC=sdcc
LD=sdld
CFLAGS += -I../libstm8/include/
LDFLAGS += -L../libstm8/lib/ -lstm8.lib
OBJECT=ledtape
OBJECTS=ledtape.ihx

.PHONY: all clean flash distclean

all: $(OBJECTS)

distclean:
	rm -f *~ *.asm *.cdb *.lk *.lst *.map *.rel *.rst *.sym
clean: distclean
	rm -f $(OBJECTS)

flash: $(OBJECT).ihx
	../stm8flash/stm8flash -cstlink -pstm8l150 -w $(OBJECT).ihx

%.ihx: %.c
	$(CC) -lstm8 -mstm8 --out-fmt-ihx $(CFLAGS) $(LDFLAGS) $<
