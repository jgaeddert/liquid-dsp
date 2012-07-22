% generate iirfilt data for autotests

% rrrf data
gen_iirfilt_data('rrr', 7, 64);

% crcf data
gen_iirfilt_data('crc', 7, 64);

% cccf data
gen_iirfilt_data('ccc', 7, 64);

