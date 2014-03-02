# power up audio feature */
sk-ab w 0x0D00 0x88
# general config of AUD_IF 1 */
sk-ab w 0x0D1B 0x97
# AB AUD_IF 1 master */
sk-ab w 0x0D1D 0x22
# AUD_IF 1 configuration - I2S */
sk-ab w 0x0D1E 0x18
# AD_OUT3 (MIC1 path) configuration to AUD_IF 1
amixer cset iface=MIXER,name='Digital Interface AD To Slot 16 Map' 'AD_OUT3'
amixer cset iface=MIXER,name='Digital Interface AD To Slot 17 Map' 'AD_OUT3'
	
