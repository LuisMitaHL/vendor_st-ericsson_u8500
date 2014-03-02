amixer cset iface=MIXER,name='LineIn Right' Disabled
amixer cset iface=MIXER,name='LineIn Left' Disabled
amixer cset iface=MIXER,name='Mic 1' Disabled
amixer cset iface=MIXER,name='Mic 2' Disabled
amixer cset iface=MIXER,name='DMic 1' Disabled
amixer cset iface=MIXER,name='DMic 2' Disabled
amixer cset iface=MIXER,name='DMic 3' Disabled
amixer cset iface=MIXER,name='DMic 4' Disabled
amixer cset iface=MIXER,name='DMic 5' Enabled
amixer cset iface=MIXER,name='DMic 6' Enabled

amixer cset iface=MIXER,name='AD 5 Select Capture Route' 'DMic 5'
amixer cset iface=MIXER,name='AD 6 Select Capture Route' 'DMic 6'
amixer cset iface=MIXER,name='Digital Interface AD To Slot 0 Map' 'AD_OUT5'
amixer cset iface=MIXER,name='Digital Interface AD To Slot 1 Map' 'AD_OUT6'
amixer cset iface=MIXER,name='Digital Interface AD To Slot 2 Map' 'AD_OUT5'
amixer cset iface=MIXER,name='Digital Interface AD To Slot 3 Map' 'AD_OUT6'
amixer cset iface=MIXER,name='Digital Interface AD To Slot 4 Map' 'AD_OUT5'
amixer cset iface=MIXER,name='Digital Interface AD To Slot 5 Map' 'AD_OUT6'
amixer cset iface=MIXER,name='Digital Interface AD To Slot 6 Map' 'AD_OUT5'
amixer cset iface=MIXER,name='Digital Interface AD To Slot 7 Map' 'AD_OUT6'
amixer cset iface=MIXER,name='Digital Interface AD To Slot 8 Map' 'AD_OUT5'
amixer cset iface=MIXER,name='Digital Interface AD To Slot 9 Map' 'AD_OUT6'
