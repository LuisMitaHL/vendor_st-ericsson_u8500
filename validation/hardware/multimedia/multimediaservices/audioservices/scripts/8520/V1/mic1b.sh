amixer cset iface=MIXER,name='LineIn Right' Disabled 2>/dev/null
amixer cset iface=MIXER,name='LineIn Left' Disabled 2>/dev/null
amixer cset iface=MIXER,name='Mic 1' Enabled 2>/dev/null
amixer cset iface=MIXER,name='Mic 2' Disabled 2>/dev/null
amixer cset iface=MIXER,name='DMic 1' Disabled 2>/dev/null
amixer cset iface=MIXER,name='DMic 2' Disabled 2>/dev/null
amixer cset iface=MIXER,name='DMic 3' Disabled 2>/dev/null
amixer cset iface=MIXER,name='DMic 4' Disabled 2>/dev/null
amixer cset iface=MIXER,name='DMic 5' Disabled 2>/dev/null
amixer cset iface=MIXER,name='DMic 6' Disabled 2>/dev/null

amixer cset iface=MIXER,name='Mic 1A or 1B Select Capture Route' 'Mic 1B' 2>/dev/null
amixer cset iface=MIXER,name='AD 3 Select Capture Route' 'Mic 1' 2>/dev/null
amixer cset iface=MIXER,name='AD 6 Select Capture Route' 'Mic 1' 2>/dev/null
amixer cset iface=MIXER,name='Digital Interface AD To Slot 0 Map' 'AD_OUT3' 2>/dev/null
amixer cset iface=MIXER,name='Digital Interface AD To Slot 1 Map' 'AD_OUT3' 2>/dev/null
