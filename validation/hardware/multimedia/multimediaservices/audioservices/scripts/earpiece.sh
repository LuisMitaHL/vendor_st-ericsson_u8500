#!/bin/sh
amixer cset iface=MIXER,name='Headset High Pass Playback Switch' Disabled
amixer cset iface=MIXER,name='Headset Low Power Playback Switch' Disabled
amixer cset iface=MIXER,name='Headset DAC Low Power Playback Switch' Disabled
amixer cset iface=MIXER,name='Headset DAC Drv Low Power Playback Switch' Disabled
amixer cset iface=MIXER,name='Earpiece DAC Low Power Playback Switch' Disabled
amixer cset iface=MIXER,name='Earpiece DAC Drv Low Power Playback Switch' Disabled
amixer cset iface=MIXER,name='Earpiece Common Mode Playback Switch' '0.95V'
amixer cset iface=MIXER,name='Headset Fade Speed Playback Switch' '2ms'
amixer cset iface=MIXER,name='Mic 1 Type Capture Switch' Differential
amixer cset iface=MIXER,name='Mic 2 Type Capture Switch' Differential
amixer cset iface=MIXER,name='Mic 1 Low Power Capture Switch' Disabled
amixer cset iface=MIXER,name='Mic 2 Low Power Capture Switch' Disabled
amixer cset iface=MIXER,name='LineIn High Pass Capture Switch' Enabled Enabled
amixer cset iface=MIXER,name='Mic High Pass Capture Switch' Enabled Enabled
amixer cset iface=MIXER,name='LineIn Mode Capture Switch' Audio Audio
amixer cset iface=MIXER,name='Mic Mode Capture Switch' Audio Audio
amixer cset iface=MIXER,name='Headset Mode Playback Switch' Audio
amixer cset iface=MIXER,name='IHF Mode Playback Switch' Audio
amixer cset iface=MIXER,name='Vibra Mode Playback Switch' Audio
amixer cset iface=MIXER,name='IHF and Headset Swap Playback Switch' Disabled
amixer cset iface=MIXER,name='IHF Low EMI Mode Playback Switch' Disabled Disabled
amixer cset iface=MIXER,name='Vibra Low EMI Mode Playback Switch' Disabled Disabled
amixer cset iface=MIXER,name='IHF FIR Bypass Playback Switch' Disabled Disabled
amixer cset iface=MIXER,name='Vibra FIR Bypass Playback Switch' Disabled Disabled
amixer cset iface=MIXER,name='IHF High Volume Playback Switch' Disabled Disabled
amixer cset iface=MIXER,name='Vibra High Volume Playback Switch' Disabled Disabled
amixer cset iface=MIXER,name='ClassD High Pass Gain Playback Volume' 8
amixer cset iface=MIXER,name='ClassD White Gain Playback Volume' 4
amixer cset iface=MIXER,name='LineIn Filter Capture Switch' 'Sinc 5' 'Sinc 5'
amixer cset iface=MIXER,name='Mic Filter Capture Switch' 'Sinc 5' 'Sinc 5'
amixer cset iface=MIXER,name='HD Mic Filter Capture Switch' 'Sinc 5' 'Sinc 5'
amixer cset iface=MIXER,name='Headset Source Playback Route' 'Audio Path' 'Audio Path'
amixer cset iface=MIXER,name='Headset Filter Playback Switch' 'Sinc 3'
amixer cset iface=MIXER,name='Digital Gain Fade Speed Switch' '1ms'
amixer cset iface=MIXER,name='Vibra PWM Duty Cycle N Playback Volume' 50 50
amixer cset iface=MIXER,name='Vibra PWM Duty Cycle P Playback Volume' 50 50
amixer cset iface=MIXER,name='Sidetone Playback Switch' Disabled
amixer cset iface=MIXER,name='IHF L and R Bridge Playback Route' Disabled
amixer cset iface=MIXER,name='Vibra 1 and 2 Bridge Playback Route' Disabled
amixer cset iface=MIXER,name='LineIn Master Gain Capture Volume' 60 60
amixer cset iface=MIXER,name='Mic Master Gain Capture Volume' 60 60
amixer cset iface=MIXER,name='HD Mic Master Gain Capture Volume' 0 0
amixer cset iface=MIXER,name='Headset Master Gain Playback Volume' 60 60
amixer cset iface=MIXER,name='IHF Master Gain Playback Volume' 0 0
amixer cset iface=MIXER,name='Vibra Master Gain Playback Volume' 0 0
amixer cset iface=MIXER,name='Analog Loopback Gain Playback Volume' 0 0
amixer cset iface=MIXER,name='Headset Digital Gain Playback Volume' 9 9
amixer cset iface=MIXER,name='Sidetone Digital Gain Playback Volume' 0 0
amixer cset iface=MIXER,name='Headset Gain Playback Volume' 10 10
amixer cset iface=MIXER,name='Mic 1 Capture Volume' 30
amixer cset iface=MIXER,name='Mic 2 Capture Volume' 30
amixer cset iface=MIXER,name='LineIn Capture Volume' 5 5
amixer cset iface=MIXER,name='LineIn to Headset Bypass Playback Volume' 0 0
amixer cset iface=MIXER,name='Digital Interface Master Generator Switch' Enabled
amixer cset iface=MIXER,name='Digital Interface 0 Bit-clock Switch' Disabled
amixer cset iface=MIXER,name='Digital Interface 1 Bit-clock Switch' Disabled
amixer cset iface=MIXER,name='Sidetone Right Source Playback Route' 'LineIn Right'
amixer cset iface=MIXER,name='Sidetone Left Source Playback Route' 'LineIn Left'
amixer cset iface=MIXER,name='ANC to Earpiece Playback Switch' off
amixer cset iface=MIXER,name='ANC Playback Switch' Disabled
amixer cset iface=MIXER,name='ANC Source Playback Route' 'Mic 1 / DMic 6'
amixer cset iface=MIXER,name='DMic 6 Capture Switch' off
amixer cset iface=MIXER,name='DMic 5 Capture Switch' off
amixer cset iface=MIXER,name='DMic 4 Capture Switch' off
amixer cset iface=MIXER,name='DMic 3 Capture Switch' off
amixer cset iface=MIXER,name='DMic 2 Capture Switch' off
amixer cset iface=MIXER,name='DMic 1 Capture Switch' off
amixer cset iface=MIXER,name='AD 6 Select Capture Route' 'DMic 6'
amixer cset iface=MIXER,name='AD 5 Select Capture Route' 'DMic 5'
amixer cset iface=MIXER,name='AD 3 Select Capture Route' 'Mic 1'
amixer cset iface=MIXER,name='Mic 1 Capture Switch' off
amixer cset iface=MIXER,name='Mic 1A or 1B Select Capture Route' 'Mic 1B'
amixer cset iface=MIXER,name='AD 2 Select Capture Route' 'LineIn Right'
amixer cset iface=MIXER,name='AD 1 Select Capture Route' 'DMic 1'
amixer cset iface=MIXER,name='Mic 2 or LINR Select Capture Route' 'Mic 2'
amixer cset iface=MIXER,name='Mic 2 Capture Switch' off
amixer cset iface=MIXER,name='LineIn Right Capture Switch' off
amixer cset iface=MIXER,name='LineIn Left Capture Switch' off
amixer cset iface=MIXER,name='Vibra 2 Controller Playback Route' 'Audio Path'
amixer cset iface=MIXER,name='Vibra 1 Controller Playback Route' 'Audio Path'
amixer cset iface=MIXER,name='IHF Right Playback Switch' off
amixer cset iface=MIXER,name='IHF Left Playback Switch' off
amixer cset iface=MIXER,name='IHF Right Source Playback Route' 'Audio Path'
amixer cset iface=MIXER,name='IHF Left Source Playback Route' 'Audio Path'
amixer cset iface=MIXER,name='Earpiece Playback Switch' on
amixer cset iface=MIXER,name='Earpiece Source Playback Route' 'Headset Left'
amixer cset iface=MIXER,name='Headset Right Playback Switch' off
amixer cset iface=MIXER,name='Headset Left Playback Switch' off
amixer cset iface=MIXER,name='If0 Input Select' BT_SCO
amixer cset iface=MIXER,name='If1 Input Select' BT_SCO
amixer cset iface=MIXER,name='If0 Output Select' BT_SCO
amixer cset iface=MIXER,name='If1 Output Select' BT_SCO
