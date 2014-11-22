%
% write_header(fid,comment)
%
% write copyright and header to file descriptor 'fid' with optional comment
%
function gendata_write_header(fid,comment)

if nargin < 2,
    comment = 'auto-generated data file';
end;

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
fprintf(fid,'// %s\n', comment);
fprintf(fid,'//\n');
fprintf(fid,'\n');
fprintf(fid,'#include <complex.h>\n');
fprintf(fid,'\n');

