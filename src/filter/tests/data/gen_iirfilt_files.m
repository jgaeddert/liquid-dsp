% generate iirfilt data for autotests

% rrrf data
gen_iirfilt_data('rrr', 3, 64);
gen_iirfilt_data('rrr', 5, 64);
gen_iirfilt_data('rrr', 7, 64);

% crcf data
gen_iirfilt_data('crc', 3, 64);
gen_iirfilt_data('crc', 5, 64);
gen_iirfilt_data('crc', 7, 64);

% cccf data
gen_iirfilt_data('ccc', 3, 64);
gen_iirfilt_data('ccc', 5, 64);
gen_iirfilt_data('ccc', 7, 64);

