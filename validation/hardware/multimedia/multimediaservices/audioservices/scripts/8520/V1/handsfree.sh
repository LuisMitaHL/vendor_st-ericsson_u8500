amixer cset iface=MIXER,name='Headset Left' Disabled 2>/dev/null
amixer cset iface=MIXER,name='Headset Right' Disabled 2>/dev/null
amixer cset iface=MIXER,name='Earpiece' Disabled 2>/dev/null
amixer cset iface=MIXER,name='IHF Left' 'Enabled' 2>/dev/null
amixer cset iface=MIXER,name='IHF Right' 'Enabled' 2>/dev/null
amixer cset iface=MIXER,name='Vibra' 'Disabled' 2>/dev/null
amixer cset iface=MIXER,name='USB Headset Left' Disabled 2>/dev/null
amixer cset iface=MIXER,name='USB Headset Right' Disabled 2>/dev/null


amixer cset iface=MIXER,name='IHF Left Source Playback Route' 'Audio Path' 2>/dev/null
amixer cset iface=MIXER,name='IHF Right Source Playback Route' 'Audio Path' 2>/dev/null
amixer cset iface=MIXER,name='IHF or LineOut Select' 'IHF' 2>/dev/null
