# GPIOs 
sk-ab w 0x1001 0xd6
sk-ab w 0x1011 0xd0
sk-ab w 0x1021 0x40
sk-ab w 0x1031 0x81
sk-ab w 0x1006 0x22
sk-ab w 0x1016 0x20
sk-ab w 0x1026 0x0
sk-ab w 0x1036 0x0
# Volumes
amixer -q cset iface=MIXER,name='LineIn Master Gain Capture Volume' 50 2>/dev/null
amixer -q cset iface=MIXER,name='Mic Master Gain Capture Volume' 50 2>/dev/null
amixer -q cset iface=MIXER,name='HD Mic Master Gain Capture Volume' 50 2>/dev/null
amixer -q cset iface=MIXER,name='Mic 1 Capture Volume' 15,15 2>/dev/null
amixer -q cset iface=MIXER,name='Mic 2 Capture Volume' 15,15 2>/dev/null
amixer -q cset iface=MIXER,name='LineIn Capture Volume' 10,10 2>/dev/null
amixer -q cset iface=MIXER,name='LineIn to Headset Bypass Playback Volume' 15,15 2>/dev/null
amixer -q cset iface=MIXER,name='Headset Master Gain Playback Volume' 42 2>/dev/null
amixer -q cset iface=MIXER,name='IHF Master Gain Playback Volume' 42 2>/dev/null
amixer -q cset iface=MIXER,name='Vibra Master Gain Playback Volume' 42 2>/dev/null
amixer -q cset iface=MIXER,name='Headset Digital Gain Playback Volume' 6,6 2>/dev/null
amixer -q cset iface=MIXER,name='Headset Gain Playback Volume' 10,10 2>/dev/null
amixer -q cset iface=MIXER,name='USB Headset Gain Playback Volume' 10,10 2>/dev/null

#HSL gain 0dB
#HSR gain  0dB
sk-ab w 0x0D16 0x22
#EAR Gain +8dB + USB MicSel
sk-ab w 0x0D75 0x10
#HFL gain 11dB
#HFR gain 11dB
sk-ab w 0x0D78 0x11
#Vibgain 11dB
sk-ab w 0x0D79 0x10
#DA1 gain  0dB
sk-ab w 0x0D47 0x00
#DA2 gain  0dB
sk-ab w 0x0D48 0x00
#DA3 gain  0dB
sk-ab w 0x0D49 0x00
#DA4 gain  0dB
sk-ab w 0x0D4A 0x00
#DA5 gain  0dB
sk-ab w 0x0D4B 0x00
#DA6 gain  0dB
sk-ab w 0x0D4C 0x00
#HSL/EAR 0dB
sk-ab w 0x0D4F 0x08
#HSR 0dB
sk-ab w 0x0D50 0x08
#MIC1 config diff
#Analog MIC1 gain  0 dB
sk-ab w 0x0D14 0x0
#MIC2 config diff
#Analog MIC2 gain  0 dB
sk-ab w 0x0D15 0x0
#LinL gain 0 dB
#LinR gain 0 dB
sk-ab w 0x0D17 0x55
#AD1 Gain  0 dB
sk-ab w 0x0D41 0x1F
#AD2 Gain  0 dB
sk-ab w 0x0D42 0x1F
#AD3 Gain  0 dB
sk-ab w 0x0D43 0x1F
#AD4 Gain  0 dB
sk-ab w 0x0D44 0x1F
#AD5 Gain  0 dB
sk-ab w 0x0D45 0x1F
#AD6 Gain  0 dB
sk-ab w 0x0D46 0x1F
#Output USB gains 0dB
sk-ab w 0xD73 0x22

