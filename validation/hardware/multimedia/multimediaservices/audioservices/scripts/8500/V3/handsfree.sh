amixer cset iface=MIXER,name='Headset Left' Disabled
amixer cset iface=MIXER,name='Headset Right' Disabled
amixer cset iface=MIXER,name='Earpiece' Disabled
amixer cset iface=MIXER,name='IHF Left' Enabled
amixer cset iface=MIXER,name='IHF Right' Enabled
amixer cset iface=MIXER,name='Vibra 1' Disabled
amixer cset iface=MIXER,name='Vibra 2' Disabled

amixer cset iface=MIXER,name='IHF Left Source Playback Route' 'Audio Path'
amixer cset iface=MIXER,name='IHF Right Source Playback Route' 'Audio Path'
amixer cset iface=MIXER,name='IHF or LineOut Select' 'IHF'
amixer cset iface=MIXER,name='Digital Interface DA 3 From Slot Map' 'SLOT9'
amixer cset iface=MIXER,name='Digital Interface DA 4 From Slot Map' 'SLOT11'

