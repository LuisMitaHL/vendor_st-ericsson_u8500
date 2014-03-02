amixer cset iface=MIXER,name='Headset Left' Disabled 2>/dev/null
amixer cset iface=MIXER,name='Headset Right' Disabled 2>/dev/null
amixer cset iface=MIXER,name='Earpiece' Disabled 2>/dev/null
amixer cset iface=MIXER,name='IHF Left' 'Disabled' 2>/dev/null
amixer cset iface=MIXER,name='IHF Right' 'Disabled' 2>/dev/null
amixer cset iface=MIXER,name='Vibra' 'Enabled' 2>/dev/null
amixer cset iface=MIXER,name='USB Headset Left' Disabled 2>/dev/null
amixer cset iface=MIXER,name='USB Headset Right' Disabled 2>/dev/null


amixer cset iface=MIXER,name='Vibra Controller Playback Route' 'Audio Path' 2>/dev/null
amixer cset iface=MIXER,name='Digital Interface DA 5 From Slot Map' 'SLOT9' 2>/dev/null
