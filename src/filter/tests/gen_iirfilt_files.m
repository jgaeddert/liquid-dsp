% generate iirfilt data for autotests

% rrrf data
gen_iirfilt_data('rrr',  4,   8);
gen_iirfilt_data('rrr',  7,  16);
gen_iirfilt_data('rrr', 13,  32);
gen_iirfilt_data('rrr', 23,  64);

% crcf data
gen_iirfilt_data('crc',  4,   8);
gen_iirfilt_data('crc',  7,  16);
gen_iirfilt_data('crc', 13,  32);
gen_iirfilt_data('crc', 23,  64);

% cccf data
gen_iirfilt_data('ccc',  4,   8);
gen_iirfilt_data('ccc',  7,  16);
gen_iirfilt_data('ccc', 13,  32);
gen_iirfilt_data('ccc', 23,  64);

