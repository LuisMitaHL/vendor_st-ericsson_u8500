amixer cset iface=MIXER,name='LineIn Right' Disabled 2>/dev/null
amixer cset iface=MIXER,name='LineIn Left' Disabled 2>/dev/null
amixer cset iface=MIXER,name='Mic 1' Disabled 2>/dev/null
amixer cset iface=MIXER,name='Mic 2' Disabled 2>/dev/null
amixer cset iface=MIXER,name='DMic 1' Disabled 2>/dev/null
amixer cset iface=MIXER,name='DMic 2' Disabled 2>/dev/null
amixer cset iface=MIXER,name='DMic 3' Enabled 2>/dev/null
amixer cset iface=MIXER,name='DMic 4' Enabled 2>/dev/null
amixer cset iface=MIXER,name='DMic 5' Disabled 2>/dev/null
amixer cset iface=MIXER,name='DMic 6' Disabled 2>/dev/null

amixer cset iface=MIXER,name='AD 3 Select Capture Route' 'DMic 3' 2>/dev/null
amixer cset iface=MIXER,name='Digital Interface AD To Slot 8 Map' 'AD_OUT3' 2>/dev/null
amixer cset iface=MIXER,name='Digital Interface AD To Slot 9 Map' 'AD_OUT4' 2>/dev/null
