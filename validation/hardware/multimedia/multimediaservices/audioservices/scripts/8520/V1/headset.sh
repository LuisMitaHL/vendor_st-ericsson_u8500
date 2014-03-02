#!/bin/sh
# NCP Enable
	sk-ab w 0x0D0C 0x80
# DA1 et DA2 paths enable
	sk-ab w 0x0D03 0xc0
# ADC3 (ADCMic) power-up
	sk-ab w 0x0D07 0xb4
# HsL & HsR drivers power-up
	sk-ab w 0x0D08 0x30
# HsL & HsR DACs and digital paths power-up
	sk-ab w 0x0D09 0x30
# Unmute all
	sk-ab w 0x0D0A 0x00
# HsL & HsR analog gains +4dB
	sk-ab w 0x0D16 0x00
# DA path 1 <- slot 8
#amixer cset iface=MIXER,name='Digital Interface DA 1 From Slot Map' 'SLOT8' 2>/dev/null
# DA path 2 <- slot 9
#amixer cset iface=MIXER,name='Digital Interface DA 2 From Slot Map' 'SLOT9' 2>/dev/null
# Digital multiplexer
	sk-ab w 0x0D40 0xC0
# DA1 digital gain 0dB
	sk-ab w 0x0D47 0x00
# DA2 digital gain 0dB
	sk-ab w 0x0D48 0x00
# HSL/EAR Dgain 0dB
	sk-ab w 0x0D4F 0x08
# HSR Dgain  0dB
	sk-ab w 0x0D50 0x08
# General Audio power-up
	sk-ab w 0x0D00 0x88



