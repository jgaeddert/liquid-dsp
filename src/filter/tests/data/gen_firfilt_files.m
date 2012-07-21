% generate firfilt data for autotests

% rrrf data
gen_firfilt_data('rrr',  4,   8);
gen_firfilt_data('rrr',  7,  16);
gen_firfilt_data('rrr', 13,  32);
gen_firfilt_data('rrr', 23,  64);

% crcf data
gen_firfilt_data('crc',  4,   8);
gen_firfilt_data('crc',  7,  16);
gen_firfilt_data('crc', 13,  32);
gen_firfilt_data('crc', 23,  64);

% cccf data
gen_firfilt_data('ccc',  4,   8);
gen_firfilt_data('ccc',  7,  16);
gen_firfilt_data('ccc', 13,  32);
gen_firfilt_data('ccc', 23,  64);

