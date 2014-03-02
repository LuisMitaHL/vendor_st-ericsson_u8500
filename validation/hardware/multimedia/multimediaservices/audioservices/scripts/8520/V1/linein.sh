amixer cset iface=MIXER,name='LineIn Right' Enabled 2>/dev/null
amixer cset iface=MIXER,name='LineIn Left' Enabled 2>/dev/null
amixer cset iface=MIXER,name='Mic 1' Disabled 2>/dev/null
amixer cset iface=MIXER,name='Mic 2' Disabled 2>/dev/null
amixer cset iface=MIXER,name='DMic 1' Disabled 2>/dev/null
amixer cset iface=MIXER,name='DMic 2' Disabled 2>/dev/null
amixer cset iface=MIXER,name='DMic 3' Disabled 2>/dev/null
amixer cset iface=MIXER,name='DMic 4' Disabled 2>/dev/null
amixer cset iface=MIXER,name='DMic 5' Disabled 2>/dev/null
amixer cset iface=MIXER,name='DMic 6' Disabled 2>/dev/null

amixer cset iface=MIXER,name='Mic 2 or LINR Select Capture Route' 'LineIn Right' 2>/dev/null
amixer cset iface=MIXER,name='AD 1 Select Capture Route' 'LineIn Left' 2>/dev/null
amixer cset iface=MIXER,name='AD 2 Select Capture Route' 'LineIn Right' 2>/dev/null
amixer cset iface=MIXER,name='Digital Interface AD To Slot 0 Map' 'AD_OUT1' 2>/dev/null
amixer cset iface=MIXER,name='Digital Interface AD To Slot 1 Map' 'AD_OUT2' 2>/dev/null
