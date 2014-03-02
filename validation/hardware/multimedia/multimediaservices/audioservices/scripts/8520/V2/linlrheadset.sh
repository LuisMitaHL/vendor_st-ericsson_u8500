sk-ab w 0x020B 0x18
#   AudioClk Enable
#sk-ab w 0x1013 0x54
#   VER_V1
sk-ab w 0x0380 0x26
#   Vintcore12
sk-ab W 0xD00 0x00
sk-ab W 0xD0C 0x80
#  Power up for headset /Anaconf5*/
sk-ab W 0xD02 0xC0
#  Digital AD channels enable*/
sk-ab W 0xD03 0xC0
#  Digital DA channels enable*/
sk-ab W 0xD04 0x02
#  Analog and DMIC*/
sk-ab W 0xD05 0x3C
#  Line In Conf*/
sk-ab W 0xD07 0xF3
# Analog to Digital Converters enabled */
sk-ab W 0xD08 0x30
#  Analog output enabled */
sk-ab W 0xD09 0x30
#  Digital output selection*/
sk-ab W 0xD0A 0x00
#  Mute enabled*/
sk-ab W 0xD0C 0x81
#  Power up for headset /Anaconf5*/
sk-ab W 0xD0F 0xFF
#  Class D configuration*/
sk-ab W 0xD10 0x3C
#  PWM VBINL*/
sk-ab W 0xD11 0x28
#  PWM VBIPL*/
#sk-ab W 0xD12 0x3C
#  PWM VBINR*/
sk-ab W 0xD13 0x28
#  PWM VBIPR*/
sk-ab W 0xD1A 0x0F
#   AD channel Filters configuration*/
sk-ab W 0xD1B 0x87
#  TDM configuration*/
sk-ab W 0xD1C 0x15
#  TDM configuration*/
sk-ab W 0xD1D 0x02
#  TDM loopback control*/
sk-ab W 0xD1E 0x02
#  TDM 0/1 */
sk-ab W 0xD1F 0x10
#  TDM audio Interface Slot assignement*/
sk-ab W 0xD33 0x80
#  slot selection for DA path1*/
sk-ab W 0xD34 0x01
#  slot selection for DA path 2*/
sk-ab W 0xD35 0x88
#  slot selection for DA path 3*/
sk-ab W 0xD36 0x08
#  slot selection for DA path 4*/
sk-ab W 0xD37 0x88
#  slot selection for DA path 5*/
sk-ab W 0xD38 0x08
#  slot selection for DA path 6*/
sk-ab W 0xD3F 0xC0
#  Multiplexers  lsb register*/
sk-ab W 0xD40 0xc4
#  multiplexeur msb*/
sk-ab W 0x383 0x2a
sk-ab W 0xD00 0x88

