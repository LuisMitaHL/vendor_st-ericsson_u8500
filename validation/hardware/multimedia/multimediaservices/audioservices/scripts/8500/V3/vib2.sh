amixer cset iface=MIXER,name='Headset Left' Disabled
amixer cset iface=MIXER,name='Headset Right' Disabled
amixer cset iface=MIXER,name='Earpiece' Disabled
amixer cset iface=MIXER,name='IHF Left' Disabled
amixer cset iface=MIXER,name='IHF Right' Disabled
amixer cset iface=MIXER,name='Vibra 1' Disabled
amixer cset iface=MIXER,name='Vibra 2' Enabled

amixer cset iface=MIXER,name='Vibra 2 Playback Route' 'Audio Path'
amixer cset iface=MIXER,name='Digital Interface DA 5 From Slot Map' 'SLOT9'
