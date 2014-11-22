% generate fft data for autotests

function gen_fft_data(n);
n = round(n);

x = randn(1,n) + j*randn(1,n);
y = fft(x);

% print results

filename = ['fft_data_' num2str(n) '.c'];
fid = fopen(filename,'w');

fprintf(fid,'/*\n');
fprintf(fid,' * Copyright (c) 2007 - 2014 Joseph Gaeddert\n');
fprintf(fid,' *\n');
fprintf(fid,' * This file is part of liquid.\n');
fprintf(fid,' *\n');
fprintf(fid,' * liquid is free software: you can redistribute it and/or modify\n');
fprintf(fid,' * it under the terms of the GNU General Public License as published by\n');
fprintf(fid,' * the Free Software Foundation, either version 3 of the License, or\n');
fprintf(fid,' * (at your option) any later version.\n');
fprintf(fid,' *\n');
fprintf(fid,' * liquid is distributed in the hope that it will be useful,\n');
fprintf(fid,' * but WITHOUT ANY WARRANTY; without even the implied warranty of\n');
fprintf(fid,' * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n');
fprintf(fid,' * GNU General Public License for more details.\n');
fprintf(fid,' *\n');
fprintf(fid,' * You should have received a copy of the GNU General Public License\n');
fprintf(fid,' * along with liquid.  If not, see <http://www.gnu.org/licenses/>.\n');
fprintf(fid,' */\n');
fprintf(fid,'\n');
fprintf(fid,'//\n');
fprintf(fid,'// autotest fft data for %u-point transform\n', n);
fprintf(fid,'//\n');
fprintf(fid,'\n');
fprintf(fid,'#include <complex.h>\n');
fprintf(fid,'\n');

fprintf(fid,'float complex fft_test_x%d[] = {\n',n);
for i=1:n,
    fprintf(fid,'  %16.12f + %16.12f*_Complex_I', real(x(i)), imag(x(i)));
    if i==n,
        fprintf(fid,'};\n');
    else,
        fprintf(fid,',\n');
    end;
end;

fprintf(fid,'\n');

fprintf(fid,'float complex fft_test_y%d[] = {\n',n);
for i=1:n,
    fprintf(fid,'  %16.12f + %16.12f*_Complex_I', real(y(i)), imag(y(i)));
    if i==n,
        fprintf(fid,'};\n');
    else,
        fprintf(fid,',\n');
    end;
end;

fprintf(fid,'\n');

fclose(fid);
printf('results written to %s\n', filename);
