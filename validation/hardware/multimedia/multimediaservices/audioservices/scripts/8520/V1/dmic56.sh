amixer cset iface=MIXER,name='LineIn Right' Disabled 2>/dev/null
amixer cset iface=MIXER,name='LineIn Left' Disabled 2>/dev/null
amixer cset iface=MIXER,name='Mic 1' Disabled 2>/dev/null
amixer cset iface=MIXER,name='Mic 2' Disabled 2>/dev/null
amixer cset iface=MIXER,name='DMic 1' Disabled 2>/dev/null
amixer cset iface=MIXER,name='DMic 2' Disabled 2>/dev/null
amixer cset iface=MIXER,name='DMic 3' Disabled 2>/dev/null
amixer cset iface=MIXER,name='DMic 4' Disabled 2>/dev/null
amixer cset iface=MIXER,name='DMic 5' Enabled 2>/dev/null
amixer cset iface=MIXER,name='DMic 6' Enabled 2>/dev/null

amixer cset iface=MIXER,name='AD 5 Select Capture Route' 'DMic 5' 2>/dev/null
amixer cset iface=MIXER,name='AD 6 Select Capture Route' 'DMic 6' 2>/dev/null
amixer cset iface=MIXER,name='Digital Interface AD To Slot 8 Map' 'AD_OUT5' 2>/dev/null
amixer cset iface=MIXER,name='Digital Interface AD To Slot 9 Map' 'AD_OUT6' 2>/dev/null

amixer cset iface=MIXER,name='HD Mic Master Gain Capture Volume' '31,31' 2>/dev/null
