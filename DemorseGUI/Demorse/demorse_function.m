function decoded_str = demorse_function(filename)

[x, fs] = audioread(filename);


% decoded string
decoded_string = '';

%zero pad the signal
x = [zeros(fs,1); x; zeros(fs,1)];

% find the frequency of the signal
ff = getChirpFrequency(x);

% use the wavelet as a bandpass filter
waveletSizes = fs * centfrq('morl') ./ ff;
sig = cwt(x,waveletSizes,'morl');

sig = abs(sig);

% do a blur filter five times the frequency length
sigF = conv(sig, ones(floor(10 * fs / ff),1),'same');

% find all places where we transition from background noise to beep
seg = (sigF > (0.25 * mean(sigF))) - 0.5;
segPts = find(sign(seg(2:end)) ~= sign(seg(1:end-1)))

% find the length of each beep or gap
segIntervals = diff(segPts) ./ fs;

beeps = segIntervals(1:2:end);
gaps = segIntervals(2:2:end);

% crude method for estimating length of dots for dashes
[N,X] = hist(beeps,10);
[~,idx] = sort(N,'descend');
thresh = mean(X(idx(1:2)));
beeps = beeps > thresh;

[N,X] = hist(gaps,20);
[~,idx] = sort(N,'descend');
gapThresh = sort(X(idx(1:3)));
gapThresh = gapThresh(1:2) + diff(gapThresh)./2;
trueGaps = (gaps > gapThresh(2)) + (gaps > gapThresh(1));

decoder = getDecoder();

% now we get to interpret the message
letter = beeps(1);
beeps = beeps(2:end);
for n = 1:length(trueGaps)
    switch trueGaps(n)
        case 0
           % dot/dash split
           letter = [letter beeps(1)];
           beeps = beeps(2:end);
        case 1
           % letter break
           fprintf('%s',decode(letter,decoder));
           decoded_string = [decoded_string decode(letter, decoder)];
           letter = beeps(1);
           beeps = beeps(2:end);
        case 2
           % word break
           fprintf('%s',decode(letter,decoder));
           decoded_string = [decoded_string decode(letter, decoder)];
           letter = beeps(1);
           beeps = beeps(2:end);
           fprintf(' ');
           decoded_string = [decoded_string ' ']
    end      
end

% print the last letter
fprintf('%s\n',decode(letter,decoder));
decoded_string = [decoded_string decode(letter, decoder)];

decoded_str = decoded_string;

end

function c = decode(str,decoder) 
    out(str==1) = '-';
    out(str==0) = '.';    

    if isKey(decoder,out)
       c = decoder(out); 
    else
        c = '???';
    end
end

function [freq] = getChirpFrequency(sig) 

limit = [300 10000]; %  frequency range allowed

fs = 44000;

% to save time only do the fft on the first 5 seconds
sig = sig(fs:min(length(sig),5 * fs));

len = length(sig);
cutoff = floor(len * limit / fs); 

f = abs(fft(sig));
f = f(cutoff(1):cutoff(2));

[~,idx] = max(f);
idx = idx + cutoff(1);

freq = idx * fs ./ len ;

end

function [decoder] = getDecoder() 
    
    text = {'A','B','C','D','E','F','G','H','I','J','K','L',...
        'M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',...
        '0','1','2','3','4','5','6','7','8','9','.',',','?','='};
    morse = {'.-','-...','-.-.','-..','.','..-.','--.','....',...
             '..','.---', '-.-','.-..', '--', '-.', '---', '.--.', ...
            '--.-', '.-.', '...', '-', '..-', '...-', '.--', '-..-', ...
            '-.--', '--..', '----', '.----', '..---', '...--', '....-', ...
            '.....', '-....', '--...', '---..', '----.', '.-.-.-', ...
            '--..--', '..--..', '-...-'};

    decoder = containers.Map(morse,text);

end