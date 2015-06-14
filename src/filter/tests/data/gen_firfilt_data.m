% 
% generate firfilt data for autotests
%

clear all;
close all;

% 
% function to generate data
%
function gen_firfilt_datafile(type,h_len,x_len);

    % determine type
    x_complex = 0;
    h_complex = 0;
    y_complex = 0;
    if strcmp(type,'rrr'),
        x_complex = 0;
        h_complex = 0;
        y_complex = 0;
    elseif strcmp(type,'crc'),
        x_complex = 1;
        h_complex = 0;
        y_complex = 1;
    elseif strcmp(type,'ccc'),
        x_complex = 1;
        h_complex = 1;
        y_complex = 1;
    else,
        error(['invalid/unsupported type (' type ')']);
    end;

    % generate filter coefficients
    if h_complex,
        h = 0.1*[randn(1,h_len) + 1i*randn(1,h_len)];
    else,
        h = 0.1*[randn(1,h_len)];
    end;

    % generate input data
    if x_complex,
        x = 0.1*[randn(1,x_len) + 1i*randn(1,x_len)];
    else,
        x = 0.1*[randn(1,x_len)];
    end;

    % filter input
    y = filter(h,1,x);
    y_len = length(y);

    % print results
    % filename example: firfilt_crcf_data_h12x44.c
    filename = ['firfilt_' type 'f_data_h' num2str(h_len) 'x' num2str(x_len) '.c'];
    fid = fopen(filename,'w');

    fprintf(fid,'/* Copyright (c) 2007 - 2015 Joseph Gaeddert\n');
    fprintf(fid,' *\n');
    fprintf(fid,' * Permission is hereby granted, free of charge, to any person obtaining a copy\n');
    fprintf(fid,' * of this software and associated documentation files (the "Software"), to deal\n');
    fprintf(fid,' * in the Software without restriction, including without limitation the rights\n');
    fprintf(fid,' * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n');
    fprintf(fid,' * copies of the Software, and to permit persons to whom the Software is\n');
    fprintf(fid,' * furnished to do so, subject to the following conditions:\n');
    fprintf(fid,' * \n');
    fprintf(fid,' * The above copyright notice and this permission notice shall be included in\n');
    fprintf(fid,' * all copies or substantial portions of the Software.\n');
    fprintf(fid,' *\n');
    fprintf(fid,' * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n');
    fprintf(fid,' * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n');
    fprintf(fid,' * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n');
    fprintf(fid,' * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n');
    fprintf(fid,' * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n');
    fprintf(fid,' * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN\n');
    fprintf(fid,' * THE SOFTWARE.\n');
    fprintf(fid,' */\n');
    fprintf(fid,'\n');
    fprintf(fid,'//\n');
    fprintf(fid,'// %s: autotest firfilt data\n', filename);
    fprintf(fid,'//\n');
    fprintf(fid,'\n');
    if h_complex || x_complex || y_complex,
        fprintf(fid,'#include <complex.h>\n\n');
    end;

    % construct base name, e.g. 'firfilt_crcf_test_h12x44'
    basename = ['firfilt_' type 'f_data_h' num2str(h_len) 'x' num2str(x_len)];

    % save coefficients array
    if h_complex, fprintf(fid,'float complex ');
    else,         fprintf(fid,'float ');
    end;
    fprintf(fid,'%s_h[] = {\n', basename);
    for i=1:h_len,
        if h_complex, fprintf(fid,'  %16.12f + %16.12f*_Complex_I', real(h(i)), imag(h(i)));
        else,         fprintf(fid,'  %16.12f', h(i));
        end;

        if i==h_len,  fprintf(fid,'};\n\n');
        else,         fprintf(fid,',\n');
        end;
    end;

    % save input array
    if x_complex, fprintf(fid,'float complex ');
    else,         fprintf(fid,'float ');
    end;
    fprintf(fid,'%s_x[] = {\n', basename);
    for i=1:x_len,
        if x_complex, fprintf(fid,'  %16.12f + %16.12f*_Complex_I', real(x(i)), imag(x(i)));
        else,         fprintf(fid,'  %16.12f', x(i));
        end;

        if i==x_len,  fprintf(fid,'};\n\n');
        else,         fprintf(fid,',\n');
        end;
    end;

    % save output array
    if y_complex, fprintf(fid,'float complex ');
    else,         fprintf(fid,'float ');
    end;
    fprintf(fid,'%s_y[] = {\n', basename);
    for i=1:y_len,
        if y_complex, fprintf(fid,'  %16.12f + %16.12f*_Complex_I', real(y(i)), imag(y(i)));
        else,         fprintf(fid,'  %16.12f', y(i));
        end;

        if i==y_len,  fprintf(fid,'};\n\n');
        else,         fprintf(fid,',\n');
        end;
    end;

    fclose(fid);
    printf('results written to %s\n', filename);

endfunction;

%
% generate firfilt data for autotests
%

% rrrf data
gen_firfilt_datafile('rrr',  4,   8);
gen_firfilt_datafile('rrr',  7,  16);
gen_firfilt_datafile('rrr', 13,  32);
gen_firfilt_datafile('rrr', 23,  64);

% crcf data
gen_firfilt_datafile('crc',  4,   8);
gen_firfilt_datafile('crc',  7,  16);
gen_firfilt_datafile('crc', 13,  32);
gen_firfilt_datafile('crc', 23,  64);

% cccf data
gen_firfilt_datafile('ccc',  4,   8);
gen_firfilt_datafile('ccc',  7,  16);
gen_firfilt_datafile('ccc', 13,  32);
gen_firfilt_datafile('ccc', 23,  64);

