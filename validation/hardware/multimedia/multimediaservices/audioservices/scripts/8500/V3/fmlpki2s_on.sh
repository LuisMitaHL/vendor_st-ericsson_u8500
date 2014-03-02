# power up audio feature
sk-ab w 0x0D00 0x88
# general config of AUD_IF 1
sk-ab w 0x0D1B 0x97
# AB AUD_IF 1 master
sk-ab w 0x0D1D 0x22
# AUD_IF 1 configuration - I2S 
sk-ab w 0x0D1E 0x18
# DA7 and DA8 (loopback path) configuration from AUD_IF 1
amixer cset iface=MIXER,name='Digital Interface DA 7 From Slot Map' 'SLOT24'
amixer cset iface=MIXER,name='Digital Interface DA 8 From Slot Map' 'SLOT25'
# AD_OUT3 and AD_OUT4 (loopback path) configuration to AUD_IF 1
amixer cset iface=MIXER,name='Digital Interface AD To Slot 16 Map' 'AD_OUT3'
amixer cset iface=MIXER,name='Digital Interface AD To Slot 17 Map' 'AD_OUT4'
#loopback register configuration (DA7 loopbacked to AD3 and DA8 loopbacked to AD4)
sk-ab w 0xD35 0x28
sk-ab w 0xD36 0x28
#check register values
sk-ab r 0xD27
sk-ab r 0xD33
sk-ab r 0xD34
sk-ab r 0xD35
sk-ab r 0xD36
sk-ab r 0xD37
sk-ab r 0xD38
sk-ab r 0xD39
sk-ab r 0xD3A

