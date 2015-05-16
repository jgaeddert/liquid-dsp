% generate nco data for autotests

% options
p = 7;          % base frequency integer
f = 1/sqrt(p);  % frequency
n = 256;        % number of points

% compute sincos data
v = exp(j*[0:(n-1)]*f);

% print results

basename = ['nco_sincos_fsqrt1_' num2str(p)];
filename = ['data/' basename '.c'];
fid      = fopen(filename,'w');

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
fprintf(fid,'// autotest nco sincos data for f=%.12f\n', f);
fprintf(fid,'//\n');

fprintf(fid,'\n');
fprintf(fid,'#include <complex.h>\n');
fprintf(fid,'\n');

fprintf(fid,'float complex %s[%u] = {\n', basename, n);
for i=1:n,
    %fprintf(fid,'  {%16.12f, %16.12f}', real(v(i)), imag(v(i)));
    fprintf(fid,'  %16.12f + _Complex_I*%16.12f', real(v(i)), imag(v(i)));
    if i==n,
        fprintf(fid,'};\n');
    else,
        fprintf(fid,',\n');
    end;
end;

fprintf(fid,'\n');

fclose(fid);
printf('results written to %s\n', filename);
