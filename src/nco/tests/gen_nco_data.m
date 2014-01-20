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
