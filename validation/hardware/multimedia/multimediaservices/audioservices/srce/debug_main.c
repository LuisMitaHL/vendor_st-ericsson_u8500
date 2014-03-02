#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "hats_audio.h"
#include "hats_audio.hi"

#include <fcntl.h>			/* O_RDWR */


int main()
{
	int pause;
	unsigned int status;
  //unsigned char data;	
  
  system("rm -f test.wav");
  status = ConvWavFile("/usr/share/sounds/alsa/Front_Left.wav","test_20b.wav");
/*******************************************************************************/
//test1:
  printf("\n\n*************** record  & playback *******************");
  pause = getchar();
  if( pause == 'p') {pause = getchar();goto test2;}
  
  printf("\n\n#########start a simple record on mic1B");
  status = SetSrc(AUDIO_DEVICE_0,CODEC_SRC_MICROPHONE_1B);
  status = StartPCMTransfer(AUDIO_DEVICE_0,CODEC_CAPTURE,NORMAL_CAPTURE_MODE,"test.wav");
  printf("\n STATUS must be 0: %d",status);
  pause = getchar();
  printf("\n\n#########stop a simple record on default organ");
  status = StopPCMTransfer(AUDIO_DEVICE_0,CODEC_CAPTURE);
  printf("\n STATUS must be 0: %d",status);

  pause = getchar();
  printf("\n\n#########start playback of recorded file in 20 bits,on default organ");
  status = ConvWavFile("test.wav","test_2.wav");
  status = StartPCMTransfer(AUDIO_DEVICE_0,CODEC_PLAYBACK,NORMAL_PLAYBACK_MODE,"test_2.wav");
  printf("\n STATUS must be 0: %d",status);
  pause = getchar();
  printf("\n\n#########stop playback on default organ ");
  status = StopPCMTransfer(AUDIO_DEVICE_0,CODEC_PLAYBACK);
  printf("\n STATUS must be 0: %d",status);
/*******************************************************************************/
test2:
  printf("\n\n*************** playback on HDMI *******************");
  pause = getchar();
  printf("\ngetchar = %c",pause);
  if( pause == 'p') {pause = getchar();goto test3;}
  printf("\n\n#########start a simple playback on HDMI organ");
  status = StartPCMTransfer(AUDIO_DEVICE_HDMI,CODEC_PLAYBACK,NORMAL_PLAYBACK_MODE,"test_20b.wav");
  printf("\n STATUS must be 0: %d",status);
  pause = getchar();
  printf("\n\n#########stop playback on HDMI ");
  status = StopPCMTransfer(AUDIO_DEVICE_HDMI,CODEC_PLAYBACK);
  printf("\n STATUS must be 0: %d",status);

/*******************************************************************************/
test3:
  printf("\n\n*************** 2 playback *******************");
  pause = getchar();
  if( pause == 'p') {pause = getchar();goto test4;}
  printf("\n\n#########start 2 playbacks \n");
  status = StartPCMTransfer(AUDIO_DEVICE_0,CODEC_PLAYBACK,NORMAL_PLAYBACK_MODE,"test_20b.wav");
  printf("\n STATUS must be 0: %d",status);
  status = StartPCMTransfer(AUDIO_DEVICE_0,CODEC_PLAYBACK,NORMAL_PLAYBACK_MODE,"test_20b.wav");
  printf("\n STATUS must be an error: %d",status);
  pause = getchar();
  printf("\n\n#########stop playback ");
  status =StopPCMTransfer(AUDIO_DEVICE_0,CODEC_PLAYBACK);
  printf("\n STATUS must be 0: %d",status);
  pause = getchar();

  printf("\n\n#########start 2 captures ");
  status = StartPCMTransfer(AUDIO_DEVICE_0,CODEC_CAPTURE,NORMAL_CAPTURE_MODE,"test_20b.wav");
  printf("\n STATUS must be 0: %d",status);
  status = StartPCMTransfer(AUDIO_DEVICE_0,CODEC_CAPTURE,NORMAL_CAPTURE_MODE,"test_20b.wav");
  printf("\n STATUS must not be 0: %d",status);
  pause = getchar();
  printf("\n\n#########stop capture ");
  status =StopPCMTransfer(AUDIO_DEVICE_0,CODEC_CAPTURE);
  printf("\n STATUS must be 0: %d",status);


/*******************************************************************************/
test4:
  printf("\n\n*************** loops *******************");
  pause = getchar();
  if( pause == 'p') {pause = getchar();goto test5;}
  printf("\n\n#########start a analog loop");
  status = StartPCMTransfer(AUDIO_DEVICE_0,CODEC_CAPTURE,ANALOG_LOOPBACK_MODE,"test_20b.wav");
  printf("\n STATUS must be 0: %d",status);
  pause = getchar();
  printf("\n\n#########stop analog loop ");
  status = StopPCMTransfer(AUDIO_DEVICE_0,CODEC_CAPTURE);
  printf("\n STATUS must be 0: %d",status);

  pause = getchar();
  printf("\n\n#########start a digital loop");
  status = StartPCMTransfer(AUDIO_DEVICE_0,CODEC_PLAYBACK,DIGITAL_LOOPBACK_MODE,"test_20b.wav");
  printf("\n STATUS must be 0: %d",status);
  pause = getchar();
  printf("\n\n#########stop digital loop ");
  status = StopPCMTransfer(AUDIO_DEVICE_0,CODEC_PLAYBACK);
  printf("\n STATUS must be 0: %d",status);

/*******************************************************************************/
test5:
  printf("\n\n*************** FM loops *******************");
  pause = getchar();
  if( pause == 'p') {pause = getchar();goto test6;}
  printf("\n\n######### start digital loopback from FM_RX to Headset");
  SetSrc(AUDIO_DEVICE_0,CODEC_SRC_FM_RX);
  status = SetSink(AUDIO_DEVICE_0,CODEC_DEST_HEADSET);
  status = StartPCMTransfer(AUDIO_DEVICE_0,CODEC_PLAYBACK,DIGITAL_LOOPBACK_MODE,"test_20b.wav");
  printf("\n STATUS must be 0: %d",status);
  pause = getchar();
  printf("\n\n######### stop digital loopback from FM_RX to Headset");
  status = StopPCMTransfer(AUDIO_DEVICE_0,CODEC_PLAYBACK);
  pause = getchar();

  printf("\n\n######### start digital loopback from dmic12 to FM_TX");
  SetSrc(AUDIO_DEVICE_0,CODEC_SRC_D_MICROPHONE_12);
  status = SetSink(AUDIO_DEVICE_0,CODEC_DEST_FM_TX);
  status = StartPCMTransfer(AUDIO_DEVICE_0,CODEC_PLAYBACK,DIGITAL_LOOPBACK_MODE,"test_20b.wav");
  printf("\n STATUS must be 0: %d",status);
  pause = getchar();
  printf("\n\n######### stop digital loopback from dmic12 to FM_TX");
  status = StopPCMTransfer(AUDIO_DEVICE_0,CODEC_PLAYBACK);

/*******************************************************************************/
test6:
  printf("\n\n*************** path management *******************");
  pause = getchar();
  if( pause == 'p') {pause = getchar();goto test7;}
  printf("\n\n#########sink Organ change");
  status = StartPCMTransfer(AUDIO_DEVICE_0,CODEC_PLAYBACK,DIGITAL_LOOPBACK_MODE,"test_20b.wav");
  printf("\n\n#########change to CODEC_DEST_EARPIECE");
  status = SetSink(AUDIO_DEVICE_0,CODEC_DEST_EARPIECE);
  printf("\n STATUS must be 0: %d",status);
  pause = getchar();
  printf("\n\n#########change to CODEC_DEST_HANDSFREE");
  status = SetSink(AUDIO_DEVICE_0,CODEC_DEST_HANDSFREE);
  printf("\n STATUS must be 0: %d",status);
  pause = getchar();
  printf("\n\n#########change to CODEC_DEST_VIBRATOR1");
  status = SetSink(AUDIO_DEVICE_0,CODEC_DEST_VIBRATOR1);
  printf("\n STATUS must be 0: %d",status);
  pause = getchar();
  printf("\n\n#########change to CODEC_DEST_VIBRATOR2");
  status = SetSink(AUDIO_DEVICE_0,CODEC_DEST_VIBRATOR2);
  printf("\n STATUS must be 0: %d",status);
  pause = getchar();
  printf("\n\n#########change to CODEC_DEST_HEADSET");
  status = SetSink(AUDIO_DEVICE_0,CODEC_DEST_HEADSET);
  printf("\n STATUS must be 0: %d",status);
  status = StopPCMTransfer(AUDIO_DEVICE_0,CODEC_PLAYBACK);
  printf("\n STATUS must be 0: %d",status);
  pause = getchar();
  printf("\n\n#########sink Organ change");
  status = StartPCMTransfer(AUDIO_DEVICE_0,CODEC_PLAYBACK,DIGITAL_LOOPBACK_MODE,"test_20b.wav");
  printf("\n\n#########change to CODEC_SRC_LINEIN");
  status = SetSrc(AUDIO_DEVICE_0,CODEC_SRC_LINEIN);
  printf("\n STATUS must be 0: %d",status);
  pause = getchar();
  printf("\n\n#########change to CODEC_SRC_MICROPHONE_1B");
  status = SetSrc(AUDIO_DEVICE_0,CODEC_SRC_MICROPHONE_1B);
  printf("\n STATUS must be 0: %d",status);
  pause = getchar();
  printf("\n\n#########change to CODEC_SRC_MICROPHONE_2");
  status = SetSrc(AUDIO_DEVICE_0,CODEC_SRC_MICROPHONE_2);
  printf("\n STATUS must be 0: %d",status);
  pause = getchar();
  printf("\n\n#########change to CODEC_SRC_D_MICROPHONE_12");
  status = SetSrc(AUDIO_DEVICE_0,CODEC_SRC_D_MICROPHONE_12);
  printf("\n STATUS must be 0: %d",status);
  pause = getchar();
  printf("\n\n#########change to CODEC_SRC_D_MICROPHONE_34");
  status = SetSrc(AUDIO_DEVICE_0,CODEC_SRC_D_MICROPHONE_34);
  printf("\n STATUS must be 0: %d",status);
  pause = getchar();
  printf("\n\n#########change to CODEC_SRC_D_MICROPHONE_56");
  status = SetSrc(AUDIO_DEVICE_0,CODEC_SRC_D_MICROPHONE_56);
  printf("\n STATUS must be 0: %d",status);
  pause = getchar();
  printf("\n\n#########change to CODEC_SRC_MICROPHONE_1A");
  status = SetSrc(AUDIO_DEVICE_0,CODEC_SRC_MICROPHONE_1A);
  printf("\n STATUS must be 0: %d",status);
  pause = getchar();
  status = StopPCMTransfer(AUDIO_DEVICE_0,CODEC_PLAYBACK);

/*******************************************************************************/
test7:

/*  status = SetVolume(AUDIO_DEVICE_0,CODEC_PLAYBACK,10,20);
  printf("\n STATUS must be 0: %d",status);
	pause = getchar();
  status = SetVolume(AUDIO_DEVICE_0,CODEC_CAPTURE,0,100);
  printf("\n STATUS must be 0: %d",status);
  status = SetVolume(AUDIO_DEVICE_0,CODEC_MASTER,0,100);
  printf("\n STATUS must be 0: %d",status);
  status = SetMute(AUDIO_DEVICE_0,CODEC_PLAYBACK,STREAM_MUTED);
	pause = getchar();
  printf("\n STATUS must be 0: %d",status);
  status = SetMute(AUDIO_DEVICE_0,CODEC_CAPTURE,STREAM_UNMUTED);
	pause = getchar();
  printf("\n STATUS must be 0: %d",status);
  status = SetMute(AUDIO_DEVICE_0,CODEC_MASTER,STREAM_UNMUTED);
	pause = getchar();
  printf("\n STATUS must be 0: %d",status);
  status = SetMultichannelMode(AUDIO_DEVICE_0,MULTI_CHANNEL_ENABLE);
	pause = getchar();
  printf("\n STATUS must be 0: %d",status);
  status = SetMultichannelMode(AUDIO_DEVICE_0,MULTI_CHANNEL_DISABLE);
	pause = getchar();
  printf("\n STATUS must be 0: %d",status);
*/ 

/* SPIWrite( 0x0D0C,0x80);
data =SPIRead( 0x0D0C);
data =SPIRead( 0x0D05);
data =SPIRead( 0x0D0A);
*/

  printf("\n\n*************** end tests*******************\n");


return 0;
}
