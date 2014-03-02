srconv_fir_72;
maxDelayFast=0;
maxDelayMedium=0;
maxDelaySlow=0;
maxDelay=0;
save delays maxDelayFast maxDelayMedium maxDelaySlow maxDelay ;
srconv2x_up
srconv2x_down
srconv3x_up
srconv3x_down
srconv4x_up
srconv4x_down
srconv6x_up
srconv6x_down
srconv12x_down
srconv12_44
srconv24_44
maxDelayFasta=maxDelayFast;
load delays.mat;
if (maxDelayFasta>maxDelayFast)
        maxDelayFast=maxDelayFasta;
end
save delays maxDelayFast maxDelayMedium maxDelaySlow maxDelay ;


srconv8_44a
maxDelayFasta=maxDelayFast;
load delays.mat;
if (maxDelayFasta>maxDelayFast)
        maxDelayFast=maxDelayFasta;
end
save delays maxDelayFast maxDelayMedium maxDelaySlow maxDelay ;

srconv16_44a
maxDelayFasta=maxDelayFast;
load delays.mat;
if (maxDelayFasta>maxDelayFast)
        maxDelayFast=maxDelayFasta;
end
save delays maxDelayFast maxDelayMedium maxDelaySlow maxDelay ;

srconv11_16;
maxDelayFasta=maxDelayFast;
load delays.mat;
if (maxDelayFasta>maxDelayFast)
        maxDelayFast=maxDelayFasta;
end
save delays maxDelayFast maxDelayMedium maxDelaySlow maxDelay ;
srconv22_48;
maxDelayFasta=maxDelayFast;
load delays.mat;
if (maxDelayFasta>maxDelayFast)
        maxDelayFast=maxDelayFasta;
end
save delays maxDelayFast maxDelayMedium maxDelaySlow maxDelay ;
srconv11_48;
srconv22_16;
maxDelayFasta=maxDelayFast;
load delays.mat;
if (maxDelayFasta>maxDelayFast)
        maxDelayFast=maxDelayFasta;
end
save delays maxDelayFast maxDelayMedium maxDelaySlow maxDelay ;
srconv64_44;
maxDelayFasta=maxDelayFast;
load delays.mat;
if (maxDelayFasta>maxDelayFast)
        maxDelayFast=maxDelayFasta;
end
save delays maxDelayFast maxDelayMedium maxDelaySlow maxDelay ;
srconv32_48;
maxDelayFasta=maxDelayFast;
load delays.mat;
if (maxDelayFasta>maxDelayFast)
        maxDelayFast=maxDelayFasta;
end
save delays maxDelayFast maxDelayMedium maxDelaySlow maxDelay ;


srconv22_8a
maxDelayFasta=maxDelayFast;
load delays.mat;
if (maxDelayFasta>maxDelayFast)
        maxDelayFast=maxDelayFasta;
end
save delays maxDelayFast maxDelayMedium maxDelaySlow maxDelay ;



%save delays maxDelayFast maxDelayMedium maxDelaySlow maxDelay ;

srconv44_8a;
maxDelayFasta=maxDelayFast;
load delays.mat;
if (maxDelayFasta>maxDelayFast)
        maxDelayFast=maxDelayFasta;
end
save delays maxDelayFast maxDelayMedium maxDelaySlow maxDelay ;


srconv88_8;
maxDelayFasta=maxDelayFast;
load delays.mat;
if (maxDelayFasta>maxDelayFast)
        maxDelayFast=maxDelayFasta;
end
save delays maxDelayFast maxDelayMedium maxDelaySlow maxDelay;
srconv64_8;
maxDelayFasta=maxDelayFast;
load delays.mat;
if (maxDelayFasta>maxDelayFast)
        maxDelayFast=maxDelayFasta;
end
save delays maxDelayFast maxDelayMedium maxDelaySlow maxDelay;
srconv88_48;
maxDelayFasta=maxDelayFast;
load delays.mat;
if (maxDelayFasta>maxDelayFast)
        maxDelayFast=maxDelayFasta;
end
save delays maxDelayFast maxDelayMedium maxDelaySlow maxDelay;

srconv96_44;
maxDelayFasta=maxDelayFast;
load delays.mat;
if (maxDelayFasta>maxDelayFast)
        maxDelayFast=maxDelayFasta;
end


srconv44_48LR;
maxDelayFasta=maxDelayFast;
load delays.mat;
if (maxDelayFasta>maxDelayFast)
        maxDelayFast=maxDelayFasta;
end




save delays maxDelayFast maxDelayMedium maxDelaySlow maxDelay ;
maxDelay = 0;
if( maxDelayFast > maxDelay )
  maxDelay = maxDelayFast
end
% write max delay to a h file
fid = openfile('resample_max_delay.h');
fprintf(fid, '#define RESAMPLE_MAX_DELAY        (%d) \n ',maxDelay);
fprintf(fid, '#define RESAMPLE_MAX_DELAY_SLOW   (%d) \n ',maxDelaySlow);
fprintf(fid, '#define RESAMPLE_MAX_DELAY_MEDIUM (%d) \n ',maxDelayMedium);
fprintf(fid, '#define RESAMPLE_MAX_DELAY_FAST   (%d) \n ',maxDelayFast);
fclose(fid);

srconv_iir_all;
%src_lomips;
srconv_low_latency;


srconv5_7H
srconv7_9H
srconv7_5H
srconv9_7H


