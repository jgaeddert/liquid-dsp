% generate fft data for autotests

n = [2, 3, 4, 5, 6, 7, 8, 9,...
     10, 16, 17, 20, 21, 22, 24, 26,...
     30, 32, 35, 36, 43, 48, 63, 64, 79, 92, 96,...
     120, 130, 157, 192, 317, 509]

for i=1:length(n),
    gen_fft_data(n(i));
end;

