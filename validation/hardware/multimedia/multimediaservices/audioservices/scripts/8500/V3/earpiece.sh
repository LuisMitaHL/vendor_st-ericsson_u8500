amixer cset iface=MIXER,name='Headset Left' Disabled
amixer cset iface=MIXER,name='Headset Right' Disabled
amixer cset iface=MIXER,name='Earpiece' Enabled
amixer cset iface=MIXER,name='IHF Left' Disabled
amixer cset iface=MIXER,name='IHF Right' Disabled
amixer cset iface=MIXER,name='Vibra 1' Disabled
amixer cset iface=MIXER,name='Vibra 2' Disabled

amixer cset iface=MIXER,name='IHF Left Source Playback Route' 'Audio Path'
amixer cset iface=MIXER,name='Earpiece or LineOut Mono Source' 'Headset Left'
amixer cset iface=MIXER,name='Digital Interface DA 1 From Slot Map' 'SLOT9'

#amixer cset iface=MIXER,name='ANC to Earpiece ' off
#amixer cset iface=MIXER,name='Headset Filter ' 'Sinc 1'


