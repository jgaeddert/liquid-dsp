%
% write_matrix(fid,x,comment)
%
% write matrix 'x' to file descriptor 'fid' with optional comment
%
function gendata_write_matrix(fid,x,varname,comment)

if nargin < 3,
    error('must specify output file descriptor, input matrix, and variable name');
elseif nargin < 4,
    comment = varname;
end;

% other options
one_line_per_element = 1;   % display one line per element?
print_index_as_comment = 1; % print index of element as comment?

% get size of matrix
[rows cols] = size(x);

% even though matrix might have two dimensions, we store it as
% a one-dimensional array
n = rows*cols;

% determine if matrix is complex or not
if iscomplex(x),
    type = 'float complex';
else,
    type = 'float';
end;

% write array comment
fprintf(fid,'// %s [size: %u x %u]\n', comment, rows, cols);

% write variable declaration
fprintf(fid,'%s %s[] = {\n',type, varname);

% write array data
n=0;
for r=1:rows,
    for c=1:cols,
        n = n+1;
        if iscomplex(x),
            fprintf(fid,'  %16.12f + %16.12f*_Complex_I', real(x(r,c)), imag(x(r,c)));
        else,
            fprintf(fid,'  %16.12f', real(x(r,c)));
        end;

        if print_index_as_comment,
            fprintf(fid,' /* (%2u,%2u) */', r-1, c-1);
        end;

        if r==rows && c==cols,
            fprintf(fid,'};\n');
        elseif c==cols || one_line_per_element,
            fprintf(fid,',\n');
        else,
            fprintf(fid,', ');
        end;
    end;
end;
fprintf(fid,'\n');

