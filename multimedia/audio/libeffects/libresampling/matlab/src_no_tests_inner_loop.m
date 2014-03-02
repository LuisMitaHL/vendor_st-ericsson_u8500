clear;
close all;
nbiter=25;
fixin=1;
M=4;
M=input('M: ');
L=12;
L=input('L: ');
nbinsamples=32;
temp=input('nbinsamples: ');
[dummy sizvalue]=size(temp);
if (sizvalue ~= 0)
	nbinsamples=temp;
	fixin=1;
else
	nboutsamples=32;
	temp=input('nboutsamples: ');
	[dummy sizvalue]=size(temp);
	if (sizvalue ~= 0)
		nboutsamples=temp;
		fixin=0;
	else
		'specify either nbinsamples or nboutsamples'
		break;
		
	end
end

%-------------------------- existing src with tests in repeat loop ------------------------------
offset=0;
inval=0;
outval=[];
for iter=1:nbiter
	if (fixin~=0)
		[nb_generated,predicted_final_offset]=calc_nbout(nbinsamples,M,L,offset);
		loopsiz=nbinsamples+nb_generated;
	else
		[nb_consumed,predicted_final_offset]=calc_nbin(nboutsamples,M,L,offset);
		loopsiz=nboutsamples+nb_consumed;
	end
	nbout=0;
	nbin=0;
	for i=1:loopsiz
		if (offset<M)
			%------------ convolution --------
			offset=offset+L;
			nbout=nbout+1;
			outval=[outval,inval];
		else
			%------------ update -------------
			offset=offset-M;
			nbin=nbin+1;
			inval=inval+1;
		end
	end
	res=['nbin: ',num2str(nbin),' nbout: ',num2str(nbout),' predicted_final_offset: ',num2str(predicted_final_offset),' offset: ',num2str(offset)];
	res
end
figure;
plot(outval,'r*');
hold on;
plot(outval,'b');
hold off;
grid on;
zoom on;

%-------------------------- proposed src with no tests in main repeat loop ------------------------------
offset=0;
inval=0;
outval=[];
for iter=1:nbiter
	if (fixin~=0)
		[nb_generated,predicted_final_offset]=calc_nbout(nbinsamples,M,L,offset);
		loopsiz=nbinsamples+nb_generated;
		conv_loopsiz=nb_generated;
	else
		[nb_consumed,predicted_final_offset]=calc_nbin(nboutsamples,M,L,offset);
		loopsiz=nboutsamples+nb_consumed;
		conv_loopsiz=nboutsamples;
	end
	nbout=0;
	nbin=0;
	%---------- update to first conv ---------
	if (offset>=M)
		[incr,offset]=src_state(M,L,offset);
		loopsiz=loopsiz-incr;
		nbin=nbin+incr;
		inval=inval+incr;
	end
	%---------------- main loop --------------------
	for i=1:conv_loopsiz
		%-------------- conv part -----------
		[incr,offset]=src_state(M,L,offset);
		nbout=nbout+1;
		outval=[outval,inval];
		loopsiz=loopsiz-1;
		%--------------- upd part ------------
		nbin=nbin+incr;
		inval=inval+incr;
		loopsiz=loopsiz-incr;
	end
	%----------- may have updated too far so need to back up -----
	nbin=nbin+loopsiz;
	inval=inval+loopsiz;
	offset=predicted_final_offset;
	res=['nbin: ',num2str(nbin),' nbout: ',num2str(nbout),' predicted_final_offset: ',num2str(predicted_final_offset),' offset: ',num2str(offset)];
	res
end
figure;
plot(outval,'r*');
hold on;
plot(outval,'b');
hold off;
grid on;
zoom on;


