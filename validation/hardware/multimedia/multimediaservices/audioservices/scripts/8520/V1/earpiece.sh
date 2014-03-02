amixer cset iface=MIXER,name='Headset Left' Disabled 2>/dev/null
amixer cset iface=MIXER,name='Headset Right' Disabled 2>/dev/null
amixer cset iface=MIXER,name='Earpiece' Enabled 2>/dev/null
amixer cset iface=MIXER,name='IHF Left' 'Disabled' 2>/dev/null
amixer cset iface=MIXER,name='IHF Right' 'Disabled' 2>/dev/null
amixer cset iface=MIXER,name='Vibra' 'Disabled' 2>/dev/null
amixer cset iface=MIXER,name='USB Headset Left' Disabled 2>/dev/null
amixer cset iface=MIXER,name='USB Headset Right' Disabled 2>/dev/null


amixer cset iface=MIXER,name='IHF Left Source Playback Route' 'Audio Path' 2>/dev/null
amixer cset iface=MIXER,name='Earpiece or LineOut Mono Source' 'Headset Left' 2>/dev/null

amixer cset iface=MIXER,name='ANC to Earpiece Playback Switch' off 2>/dev/null
amixer cset iface=MIXER,name='Headset Filter Playback Switch' 'Sinc 1' 2>/dev/null
