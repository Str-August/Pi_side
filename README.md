# Pi_side
app -> pi -> sbus -> adruidno
# pi -> sbus
- Kieu du lieu truyen package 25 byte
    Byte[0]: SBUS Header, 0x0F
    Byte[1-22]: 16 servo channels, 11 bits per servo channel
    Byte[23]:
    Bit 7: digital channel 17 (0x80)
    Bit 6: digital channel 18 (0x40)
    Bit 5: frame lost (0x20)
    Bit 4: failsafe activated (0x10)
    Bit 0 - 3: n/a
    Byte[24]: SBUS End Byte, 0x00
- get_data -> encapsulate package -> tranfer with a default frequency