function [numerator,denominator]=getnphase(fin,fout,phasenum,maxperr);
%function [numerator,denominator]=getnphase(fin,fout,phasenum,maxperr)

[dummy numphase]=size(phasenum);
found=0;
for iter4=1:numphase
	num4=phasenum(iter4);
	for den4=num4:-1:1
		%------------------------------------------------------------------------------------
		for iter3=1:numphase
			num3=phasenum(iter3);
				for den3=num3:-1:1
					%--------------------------------------------------------------------
					for iter2=1:numphase
					num2=phasenum(iter2);
					for den2=num2:-1:1;
						%----------------------------------------------------
						for iter1=1:numphase
							num1=phasenum(iter1);
							for den1=num1:-1:1
								ratio=num1*num2*num3*num4/(den1*den2*den3*den4);
								foutfound=fin*ratio;
								perr=100*abs(foutfound-fout)/fout;
								if (perr<maxperr)
									found=1;
									break;
								end
							end
							if (found==1)
								break;
							end
							den1=phasenum(iter1);
							for num1=num1:-1:1
								ratio=num1*num2*num3*num4/(den1*den2*den3*den4);
								foutfound=fin*ratio;
								perr=100*abs(foutfound-fout)/fout;
								if (perr<maxperr)
									found=1;
									break;
								end
							end
							if (found==1)
								break;
							end
						end
						%----------------------------------------------------
						if (found==1)
							break;
						end
					end
					if (found==1)
						break;
					end
					den2=phasenum(iter2);
					for num2=num2:-1:1
						%----------------------------------------------------
						for iter1=1:numphase
							num1=phasenum(iter1);
							for den1=num1:-1:1
								ratio=num1*num2*num3*num4/(den1*den2*den3*den4);
								foutfound=fin*ratio;
								perr=100*abs(foutfound-fout)/fout;
								if (perr<maxperr)
									found=1;
									break;
								end
							end
							if (found==1)
								break;
							end
							den1=phasenum(iter1);
							for num1=num1:-1:1
								ratio=num1*num2*num3*num4/(den1*den2*den3*den4);
								foutfound=fin*ratio;
								perr=100*abs(foutfound-fout)/fout;
								if (perr<maxperr)
									found=1;
									break;
								end
							end
							if (found==1)
								break;
							end
						end
						%----------------------------------------------------
						if (found==1)
							break;
						end
					end
					if (found==1)
						break;
					end
				end
				%--------------------------------------------------------------------
				if (found==1)
					break;
				end
			end
			if (found==1)
				break;
			end
			den3=phasenum(iter3);
			for num3=num3:-1:1
				%--------------------------------------------------------------------
				for iter2=1:numphase
					num2=phasenum(iter2);
					for den2=num2:-1:1;
						%----------------------------------------------------
						for iter1=1:numphase
							num1=phasenum(iter1);
							for den1=num1:-1:1
								ratio=num1*num2*num3*num4/(den1*den2*den3*den4);
								foutfound=fin*ratio;
								perr=100*abs(foutfound-fout)/fout;
								if (perr<maxperr)
									found=1;
									break;
								end
							end
							if (found==1)
								break;
							end
							den1=phasenum(iter1);
							for num1=num1:-1:1
								ratio=num1*num2*num3*num4/(den1*den2*den3*den4);
								foutfound=fin*ratio;
								perr=100*abs(foutfound-fout)/fout;
								if (perr<maxperr)
									found=1;
									break;
								end
							end
							if (found==1)
								break;
							end
						end
						%----------------------------------------------------
						if (found==1)
							break;
						end
					end
					if (found==1)
						break;
					end
					den2=phasenum(iter2);
					for num2=num2:-1:1
						%----------------------------------------------------
						for iter1=1:numphase
							num1=phasenum(iter1);
							for den1=num1:-1:1
								ratio=num1*num2*num3*num4/(den1*den2*den3*den4);
								foutfound=fin*ratio;
								perr=100*abs(foutfound-fout)/fout;
								if (perr<maxperr)
									found=1;
									break;
								end
							end
							if (found==1)
								break;
							end
							den1=phasenum(iter1);
							for num1=num1:-1:1
								ratio=num1*num2*num3*num4/(den1*den2*den3*den4);
								foutfound=fin*ratio;
								perr=100*abs(foutfound-fout)/fout;
								if (perr<maxperr)
									found=1;
									break;
								end
							end
							if (found==1)
								break;
							end
						end
						%----------------------------------------------------
						if (found==1)
							break;
						end
					end
					if (found==1)
						break;
					end
				end
				%--------------------------------------------------------------------
				if (found==1)
					break;
				end
			end
			if (found==1)
				break;
			end
		end
		%-------------------------------------------------------------------------------
		if (found==1)
			break;
		end
	end
	if (found==1)
		break;
	end
	den4=phasenum(iter4);
	for num4=num4:-1:1
		%------------------------------------------------------------------------------------
		for iter3=1:numphase
			num3=phasenum(iter3);
				for den3=num3:-1:1
					%--------------------------------------------------------------------
					for iter2=1:numphase
					num2=phasenum(iter2);
					for den2=num2:-1:1;
						%----------------------------------------------------
						for iter1=1:numphase
							num1=phasenum(iter1);
							for den1=num1:-1:1
								ratio=num1*num2*num3*num4/(den1*den2*den3*den4);
								foutfound=fin*ratio;
								perr=100*abs(foutfound-fout)/fout;
								if (perr<maxperr)
									found=1;
									break;
								end
							end
							if (found==1)
								break;
							end
							den1=phasenum(iter1);
							for num1=num1:-1:1
								ratio=num1*num2*num3*num4/(den1*den2*den3*den4);
								foutfound=fin*ratio;
								perr=100*abs(foutfound-fout)/fout;
								if (perr<maxperr)
									found=1;
									break;
								end
							end
							if (found==1)
								break;
							end
						end
						%----------------------------------------------------
						if (found==1)
							break;
						end
					end
					if (found==1)
						break;
					end
					den2=phasenum(iter2);
					for num2=num2:-1:1
						%----------------------------------------------------
						for iter1=1:numphase
							num1=phasenum(iter1);
							for den1=num1:-1:1
								ratio=num1*num2*num3*num4/(den1*den2*den3*den4);
								foutfound=fin*ratio;
								perr=100*abs(foutfound-fout)/fout;
								if (perr<maxperr)
									found=1;
									break;
								end
							end
							if (found==1)
								break;
							end
							den1=phasenum(iter1);
							for num1=num1:-1:1
								ratio=num1*num2*num3*num4/(den1*den2*den3*den4);
								foutfound=fin*ratio;
								perr=100*abs(foutfound-fout)/fout;
								if (perr<maxperr)
									found=1;
									break;
								end
							end
							if (found==1)
								break;
							end
						end
						%----------------------------------------------------
						if (found==1)
							break;
						end
					end
					if (found==1)
						break;
					end
				end
				%--------------------------------------------------------------------
				if (found==1)
					break;
				end
			end
			if (found==1)
				break;
			end
			den3=phasenum(iter3);
			for num3=num3:-1:1
				%--------------------------------------------------------------------
				for iter2=1:numphase
					num2=phasenum(iter2);
					for den2=num2:-1:1;
						%----------------------------------------------------
						for iter1=1:numphase
							num1=phasenum(iter1);
							for den1=num1:-1:1
								ratio=num1*num2*num3*num4/(den1*den2*den3*den4);
								foutfound=fin*ratio;
								perr=100*abs(foutfound-fout)/fout;
								if (perr<maxperr)
									found=1;
									break;
								end
							end
							if (found==1)
								break;
							end
							den1=phasenum(iter1);
							for num1=num1:-1:1
								ratio=num1*num2*num3*num4/(den1*den2*den3*den4);
								foutfound=fin*ratio;
								perr=100*abs(foutfound-fout)/fout;
								if (perr<maxperr)
									found=1;
									break;
								end
							end
							if (found==1)
								break;
							end
						end
						%----------------------------------------------------
						if (found==1)
							break;
						end
					end
					if (found==1)
						break;
					end
					den2=phasenum(iter2);
					for num2=num2:-1:1
						%----------------------------------------------------
						for iter1=1:numphase
							num1=phasenum(iter1);
							for den1=num1:-1:1
								ratio=num1*num2*num3*num4/(den1*den2*den3*den4);
								foutfound=fin*ratio;
								perr=100*abs(foutfound-fout)/fout;
								if (perr<maxperr)
									found=1;
									break;
								end
							end
							if (found==1)
								break;
							end
							den1=phasenum(iter1);
							for num1=num1:-1:1
								ratio=num1*num2*num3*num4/(den1*den2*den3*den4);
								foutfound=fin*ratio;
								perr=100*abs(foutfound-fout)/fout;
								if (perr<maxperr)
									found=1;
									break;
								end
							end
							if (found==1)
								break;
							end
						end
						%----------------------------------------------------
						if (found==1)
							break;
						end
					end
					if (found==1)
						break;
					end
				end
				%--------------------------------------------------------------------
				if (found==1)
					break;
				end
			end
			if (found==1)
				break;
			end
		end
		%------------------------------------------------------------------------------------
		if (found==1)
			break;
		end
	end
	if (found==1)
		break;
	end
end
if (found==1)
	coefperphase=100;
	numerator=[num1,num2,num3,num4];
	denominator=[den1,den2,den3,den4];
	numstage=0;
	ratio=1;
	for k=1:4
		if (numerator(k)==denominator(k))
			break;
		end
		ratio=ratio*numerator(k)/denominator(k);
		numstage=numstage+1;
	end
	num=numerator(1:numstage);
	den=denominator(1:numstage);
	numerator=[ ];
	denominator=[ ];
	if (numstage>0)
		%------------------- all possible orders of cascaded filters --------
		if (numstage==1)
			numorder=1;
			ord=1;
		end 
		if (numstage==2)
			numorder=2;
			ord=[1 2;2 1];
		end
		if (numstage==3)
			numorder=6;
			ord=[1 2 3;2 1 3;3 1 2;1 3 2; 3 2 1; 2 3 1];
		end
		if (numstage==4)
			numorder=24;
			ord=[1 2 3 4;2 1 3 4;3 1 2 4;1 3 2 4; 3 2 1 4; 2 3 1 4;
		    	 1 2 4 3;2 1 4 3;4 1 2 3;1 4 2 3; 4 2 1 3; 2 4 1 3;
		    	 1 4 3 2;4 1 3 2;3 1 4 2;1 3 4 2; 3 4 1 2; 4 3 1 2;
		    	 4 2 3 1;2 4 3 1;3 4 2 1;4 3 2 1; 3 2 4 1; 2 3 4 1];
		end
		%------------------- find best cascades optimizing mips without reducing global cutoff frequency ----
		fmin=.99999999*min(fin,fin*ratio);
		bestkmips=1e10;
		bestindx=0;
		for j=1:numorder
			nm=[num([ord(j,:)])];
			dn=[den([ord(j,:)])];
			found=1;
			kmips=0;
			nsamp=1;
			freq=fin;
			for (i=1:numstage)
				freq=freq*nm(i)/dn(i);
				if (freq<fmin)
					found=0;
					break;
				end
				nsamp=nsamp*nm(i)/dn(i);
				nphs=max(nm(i),dn(i));
				ncoefs=nphs*coefperphase;
				if (nm(i)>dn(i))
					kmips=kmips+(ncoefs/nphs)*(nm(i)/dn(i))*nsamp;
				else
					kmips=kmips+(ncoefs/nphs)*nsamp;
				end
			end
			if (found==1)
				if (kmips<bestkmips)
					bestindx=j;
					bestkmips=kmips;
				end
			end
		end
		numerator=  [num([ord(bestindx,:)])];
		denominator=[den([ord(bestindx,:)])];
	end
else
	numerator=[ ];
	denominator=[ ];
end


