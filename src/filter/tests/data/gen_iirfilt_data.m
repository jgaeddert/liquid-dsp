% 
% generate iirfilt data for autotests
%

clear all;
close all;

% 
% function to generate data
%
function gen_iirfilt_datafile(type,h_len,x_len);

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

    % 
    % generate low-pass butterworth filter coefficients
    %

    % generate complex roots
    n = h_len - 1;      % filter order
    r = mod(n, 2);      % odd/even order
    L = (n-r)/2;        % filter semi-length
    pa = zeros(1,n);    % analog poles (roots of a)
    for i=0:(L-1),
        theta = (2*(i+1) + n - 1)*pi / (2*n);
        pa(2*i+1) = exp( j*theta );
        pa(2*i+2) = exp(-j*theta );
    end;
    if r,
        pa(end) = -1;
    end;

    % convert analog poles/zeros/gain to digital
    fc = 0.1;
    m  = abs(tan(pi*fc));
    p = zeros(1,n);
    z = zeros(1,n);
    k = 1;
    for i=1:n,
        % compute digital zeros...
        z(i) = -1;

        % compute digital pols
        pm = pa(i)*m;
        p(i) = (1 + pm)/(1 - pm);

        % compute digital gain
        k = k*(1 - p(i)) / (1 - z(i));
    end;

    % expand roots
    b = [1];
    a = [1];
    k = 1;  % gain
    for i=1:n,
        b = conv(b, [1, -z(i)]);
        a = conv(a, [1, -p(i)]);
        k = k*(1 - p(i))/(1 - z(i));
    end;
    k = real(k);
    a = real(a);
    b = real(b) * k;

    % modulate coefficients for complex type
    if h_complex,
        for i=1:h_len,
            a(i) = a(i) * exp(j*2*pi*0.1*i);
            b(i) = b(i) * exp(j*2*pi*0.1*i);
        end;
    end;

    % generate input data
    if x_complex,
        x = 0.1*[randn(1,x_len) + 1i*randn(1,x_len)];
    else,
        x = 0.1*[randn(1,x_len)];
    end;

    % filter input
    y = filter(b,a,x);
    y_len = length(y);

    % print results
    % filename example: iirfilt_crcf_data_h12x44.c
    filename = ['iirfilt_' type 'f_data_h' num2str(h_len) 'x' num2str(x_len) '.c'];
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
    fprintf(fid,'// %s: autotest iirfilt data\n', filename);
    fprintf(fid,'//\n');
    fprintf(fid,'\n');
    if h_complex || x_complex || y_complex,
        fprintf(fid,'#include <complex.h>\n\n');
    end;

    % construct base name, e.g. 'iirfilt_crcf_test_h12x44'
    basename = ['iirfilt_' type 'f_data_h' num2str(h_len) 'x' num2str(x_len)];

    % save coefficients arrays
    if h_complex, fprintf(fid,'float complex ');
    else,         fprintf(fid,'float ');
    end;
    fprintf(fid,'%s_b[] = {\n', basename);
    for i=1:h_len,
        if h_complex, fprintf(fid,'  %16.12f + %16.12f*_Complex_I', real(b(i)), imag(b(i)));
        else,         fprintf(fid,'  %16.12f', b(i));
        end;

        if i==h_len,  fprintf(fid,'};\n\n');
        else,         fprintf(fid,',\n');
        end;
    end;

    % save coefficients arrays
    if h_complex, fprintf(fid,'float complex ');
    else,         fprintf(fid,'float ');
    end;
    fprintf(fid,'%s_a[] = {\n', basename);
    for i=1:h_len,
        if h_complex, fprintf(fid,'  %16.12f + %16.12f*_Complex_I', real(a(i)), imag(a(i)));
        else,         fprintf(fid,'  %16.12f', a(i));
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
% generate test data
%

% rrrf data
gen_iirfilt_datafile('rrr', 3, 64);
gen_iirfilt_datafile('rrr', 5, 64);
gen_iirfilt_datafile('rrr', 7, 64);

% crcf data
gen_iirfilt_datafile('crc', 3, 64);
gen_iirfilt_datafile('crc', 5, 64);
gen_iirfilt_datafile('crc', 7, 64);

% cccf data
gen_iirfilt_datafile('ccc', 3, 64);
gen_iirfilt_datafile('ccc', 5, 64);
gen_iirfilt_datafile('ccc', 7, 64);


