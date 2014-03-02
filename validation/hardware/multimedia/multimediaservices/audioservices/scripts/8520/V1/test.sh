echo "======> Testing volumes scripts..." 
echo "======> volumes.sh execution..."
echo "======> volumes.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/volumes.sh >> usr_local_bin_audio_result.txt

echo "======> Testing all inputs scripts..." 
echo "======> linein.sh execution..." 
echo "======> linein.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/linein.sh >> usr_local_bin_audio_result.txt
echo "======> dmic12.sh execution..."
echo "======> dmic12.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/dmic12.sh >> usr_local_bin_audio_result.txt
echo "======> dmic34.sh execution..." 
echo "======> dmic34.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/dmic34.sh >> usr_local_bin_audio_result.txt
echo "======> dmic56.sh execution..."
echo "======> dmic56.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/dmic56.sh >> usr_local_bin_audio_result.txt
echo "======> mic1a.sh execution..."
echo "======> mic1a.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/mic1a.sh >> usr_local_bin_audio_result.txt
echo "======> mic1b.sh execution..."
echo "======> mic1b.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/mic1b.sh >> usr_local_bin_audio_result.txt
echo "======> mic2.sh execution..." 
echo "======> mic2.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/mic2.sh >> usr_local_bin_audio_result.txt

echo "======> Testing all outputs scripts..." 
echo "======> earpiece.sh execution..." 
echo "======> earpiece.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/earpiece.sh >> usr_local_bin_audio_result.txt
echo "======> handsfree.sh execution..." 
echo "======> handsfree.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/handsfree.sh >> usr_local_bin_audio_result.txt
echo "======> headset.sh execution..."
echo "======> headset.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/headset.sh >> usr_local_bin_audio_result.txt
echo "======> vib1.sh execution..."
echo "======> vib1.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/vib1.sh >> usr_local_bin_audio_result.txt
echo "======> usbhs.sh execution..."
echo "======> usbhs.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/usbhs.sh >> usr_local_bin_audio_result.txt
echo "======> usbck.sh execution..."
echo "======> usbck.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/usbck.sh >> usr_local_bin_audio_result.txt

echo "======> Testing Analog Loop scripts..." 
echo "======> analog_loop_on.sh execution..." 
echo "======> analog_loop_on.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/analog_loop_on.sh >> usr_local_bin_audio_result.txt
read -p "Appuyer sur une touche pour continuer ..."
echo "======> analog_loop_off.sh execution..."
echo "======> analog_loop_off.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/analog_loop_off.sh >> usr_local_bin_audio_result.txt


echo "======> Testing AB Digital Loop scripts..." 
echo "======> dmic12headset.sh execution..." 
echo "======> dmic12headset.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/dmic12headset.sh >> usr_local_bin_audio_result.txt
read -p "Appuyer sur une touche pour continuer ..."
echo "======> dmic34headset.sh execution..."
echo "======> dmic34headset.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/dmic34headset.sh >> usr_local_bin_audio_result.txt
read -p "Appuyer sur une touche pour continuer ..."
echo "======> linlrheadset.sh execution..."
echo "======> linlrheadset.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/linlrheadset.sh >> usr_local_bin_audio_result.txt
read -p "Appuyer sur une touche pour continuer ..."
echo "======> mic1aearpiece.sh execution..."
echo "======> mic1aearpiece.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/mic1aearpiece.sh >> usr_local_bin_audio_result.txt
read -p "Appuyer sur une touche pour continuer ..."
echo "======> mic1ahandsfree.sh execution..."
echo "======> mic1ahandsfree.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/mic1ahandsfree.sh >> usr_local_bin_audio_result.txt
read -p "Appuyer sur une touche pour continuer ..."
echo "======> mic1bearpiece.sh execution..."
echo "======> mic1bearpiece.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/mic1bearpiece.sh >> usr_local_bin_audio_result.txt
read -p "Appuyer sur une touche pour continuer ..."
echo "======> mic1bhandsfree.sh execution..."
echo "======> mic1bhandsfree.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/mic1bhandsfree.sh >> usr_local_bin_audio_result.txt
read -p "Appuyer sur une touche pour continuer ..."
echo "======> mic1bheadset.sh execution..."
echo "======> mic1bheadset.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/mic1bheadset.sh >> usr_local_bin_audio_result.txt
read -p "Appuyer sur une touche pour continuer ..."
echo "======> mic2headset.sh execution..."
echo "======> mic2headset.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/mic2headset.sh >> usr_local_bin_audio_result.txt
read -p "Appuyer sur une touche pour continuer ..."
echo "======> ab_digital_loop_off.sh execution..."
echo "======> ab_digital_loop_off.sh execution..." >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/ab_digital_loop_off.sh >> usr_local_bin_audio_result.txt

echo "======> Testing Playback ..." 
echo "======> To headset ..." 
/usr/local/bin/audio/headset.sh >> usr_local_bin_audio_result.txt
aplay -qS -r48000 -Dhw:0,1 /var/local/dthservices/dthlaudio/playback.wav&
read -p "Appuyer sur une touche pour continuer ..."
pidof aplay | xargs kill -9

echo "======> To handsfree ..." 
/usr/local/bin/audio/handsfree.sh >> usr_local_bin_audio_result.txt
aplay -qS -r48000 -Dhw:0,1 /var/local/dthservices/dthlaudio/playback.wav&
read -p "Appuyer sur une touche pour continuer ..."
pidof aplay | xargs kill -9

echo "======> To earpiec ..." 
/usr/local/bin/audio/earpiece.sh >> usr_local_bin_audio_result.txt
aplay -qS -r48000 -Dhw:0,1 /var/local/dthservices/dthlaudio/playback.wav&
read -p "Appuyer sur une touche pour continuer ..."
pidof aplay | xargs kill -9

echo "======> To USB headset..." 
/usr/local/bin/audio/usbhs.sh >> usr_local_bin_audio_result.txt
aplay -qS -r48000 -Dhw:0,1 /var/local/dthservices/dthlaudio/playback.wav&
read -p "Appuyer sur une touche pour continuer ..."
pidof aplay | xargs kill -9

echo "======> To USB CarKit..." 
/usr/local/bin/audio/usbck.sh >> usr_local_bin_audio_result.txt
aplay -qS -r48000 -Dhw:0,1 /var/local/dthservices/dthlaudio/playback.wav&
read -p "Appuyer sur une touche pour continuer ..."
pidof aplay | xargs kill -9


echo "======> Testing AP Digital Loop ..." 
echo "======> dmic12 to headset ..." 
/usr/local/bin/audio/dmic12.sh >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/headset.sh >> usr_local_bin_audio_result.txt
arecord -Dhw:0,2 -fS16 -r48000 -c2 | aplay -Dhw:0,1 -r48000 -c8 &
#arecord -Dhw:0,2 -fS16 -r48000 -c8 | aplay -Dhw:0,1 -r48000 -c8 &
read -p "Appuyer sur une touche pour continuer ..."
pidof aplay | xargs kill -9
pidof arecord | xargs kill -9

echo "======> dmic34 to headset ..." 
/usr/local/bin/audio/dmic34.sh >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/headset.sh >> usr_local_bin_audio_result.txt
arecord -Dhw:0,2 -fS16 -r48000 -c2 | aplay -Dhw:0,1 -r48000 -c8 &
#arecord -Dhw:0,2 -fS16 -r48000 -c8 | aplay -Dhw:0,1 -r48000 -c8 &
read -p "Appuyer sur une touche pour continuer ..."
pidof aplay | xargs kill -9
pidof arecord | xargs kill -9

echo "======> dmic56 to headset ..." 
/usr/local/bin/audio/dmic56.sh >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/headset.sh >> usr_local_bin_audio_result.txt
arecord -Dhw:0,2 -fS16 -r48000 -c2 | aplay -Dhw:0,1 -r48000 -c8 &
#arecord -Dhw:0,2 -fS16 -r48000 -c8 | aplay -Dhw:0,1 -r48000 -c8 &
read -p "Appuyer sur une touche pour continuer ..."
pidof aplay | xargs kill -9
pidof arecord | xargs kill -9

echo "======> mic1a to headset ..." 
/usr/local/bin/audio/mic1a.sh >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/headset.sh >> usr_local_bin_audio_result.txt
arecord -Dhw:0,2 -fS16 -r48000 -c2 | aplay -Dhw:0,1 -r48000 -c8 &
#arecord -Dhw:0,2 -fS16 -r48000 -c8 | aplay -Dhw:0,1 -r48000 -c8 &
read -p "Appuyer sur une touche pour continuer ..."
pidof aplay | xargs kill -9
pidof arecord | xargs kill -9

echo "======> mic1b to headset ..." 
/usr/local/bin/audio/mic1b.sh >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/headset.sh >> usr_local_bin_audio_result.txt
arecord -Dhw:0,2 -fS16 -r48000 -c2 | aplay -Dhw:0,1 -r48000 -c8 &
#arecord -Dhw:0,2 -fS16 -r48000 -c8 | aplay -Dhw:0,1 -r48000 -c8 &
read -p "Appuyer sur une touche pour continuer ..."
pidof aplay | xargs kill -9
pidof arecord | xargs kill -9

echo "======> mic2 to headset ..." 
/usr/local/bin/audio/mic2.sh >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/headset.sh >> usr_local_bin_audio_result.txt
arecord -Dhw:0,2 -fS16 -r48000 -c2 | aplay -Dhw:0,1 -r48000 -c8 &
#arecord -Dhw:0,2 -fS16 -r48000 -c8 | aplay -Dhw:0,1 -r48000 -c8 &
read -p "Appuyer sur une touche pour continuer ..."
pidof aplay | xargs kill -9
pidof arecord | xargs kill -9

echo "======> dmic12 to earpiece ..." 
/usr/local/bin/audio/dmic12.sh >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/earpiece.sh >> usr_local_bin_audio_result.txt
arecord -Dhw:0,2 -fS16 -r48000 -c2 | aplay -Dhw:0,1 -r48000 -c8 &
#arecord -Dhw:0,2 -fS16 -r48000 -c8 | aplay -Dhw:0,1 -r48000 -c8 &
read -p "Appuyer sur une touche pour continuer ..."
pidof aplay | xargs kill -9
pidof arecord | xargs kill -9

echo "======> mic1a to earpiece ..." 
/usr/local/bin/audio/mic1a.sh >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/earpiece.sh >> usr_local_bin_audio_result.txt
arecord -Dhw:0,2 -fS16 -r48000 -c2 | aplay -Dhw:0,1 -r48000 -c8 &
#arecord -Dhw:0,2 -fS16 -r48000 -c8 | aplay -Dhw:0,1 -r48000 -c8 &
read -p "Appuyer sur une touche pour continuer ..."
pidof aplay | xargs kill -9
pidof arecord | xargs kill -9

echo "======> mic1b to earpiece ..." 
/usr/local/bin/audio/mic1b.sh >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/earpiece.sh >> usr_local_bin_audio_result.txt
arecord -Dhw:0,2 -fS16 -r48000 -c2 | aplay -Dhw:0,1 -r48000 -c8 &
#arecord -Dhw:0,2 -fS16 -r48000 -c8 | aplay -Dhw:0,1 -r48000 -c8 &
read -p "Appuyer sur une touche pour continuer ..."
pidof aplay | xargs kill -9
pidof arecord | xargs kill -9

echo "======> dmic12 to handsfree ..." 
/usr/local/bin/audio/dmic12.sh >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/handsfree.sh >> usr_local_bin_audio_result.txt
arecord -Dhw:0,2 -fS16 -r48000 -c2 | aplay -Dhw:0,1 -r48000 -c8 &
#arecord -Dhw:0,2 -fS16 -r48000 -c8 | aplay -Dhw:0,1 -r48000 -c8 &
read -p "Appuyer sur une touche pour continuer ..."
pidof aplay | xargs kill -9
pidof arecord | xargs kill -9

echo "======> mic1a to handsfree ..." 
/usr/local/bin/audio/mic1a.sh >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/handsfree.sh >> usr_local_bin_audio_result.txt
arecord -Dhw:0,2 -fS16 -r48000 -c2 | aplay -Dhw:0,1 -r48000 -c8 &
#arecord -Dhw:0,2 -fS16 -r48000 -c8 | aplay -Dhw:0,1 -r48000 -c8 &
read -p "Appuyer sur une touche pour continuer ..."
pidof aplay | xargs kill -9
pidof arecord | xargs kill -9

echo "======> mic1b to handsfree ..." 
/usr/local/bin/audio/mic1b.sh >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/handsfree.sh >> usr_local_bin_audio_result.txt
arecord -Dhw:0,2 -fS16 -r48000 -c2 | aplay -Dhw:0,1 -r48000 -c8 &
#arecord -Dhw:0,2 -fS16 -r48000 -c8 | aplay -Dhw:0,1 -r48000 -c8 &
read -p "Appuyer sur une touche pour continuer ..."
pidof aplay | xargs kill -9
pidof arecord | xargs kill -9

echo "======> dmic12 to usb headset ..." 
/usr/local/bin/audio/dmic12.sh >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/usbhs.sh >> usr_local_bin_audio_result.txt
arecord -Dhw:0,2 -fS16 -r48000 -c2 | aplay -Dhw:0,1 -r48000 -c8 &
#arecord -Dhw:0,2 -fS16 -r48000 -c8 | aplay -Dhw:0,1 -r48000 -c8 &
read -p "Appuyer sur une touche pour continuer ..."
pidof aplay | xargs kill -9
pidof arecord | xargs kill -9

echo "======> mic1b to usb headset ..." 
/usr/local/bin/audio/mic1b.sh >> usr_local_bin_audio_result.txt
/usr/local/bin/audio/usbhs.sh >> usr_local_bin_audio_result.txt
arecord -Dhw:0,2 -fS16 -r48000 -c2 | aplay -Dhw:0,1 -r48000 -c8 &
#arecord -Dhw:0,2 -fS16 -r48000 -c8 | aplay -Dhw:0,1 -r48000 -c8 &
read -p "Appuyer sur une touche pour continuer ..."
pidof aplay | xargs kill -9
pidof arecord | xargs kill -9


