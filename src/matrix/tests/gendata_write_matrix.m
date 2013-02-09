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
for i=1:n,
    if iscomplex(x),
        fprintf(fid,'  %16.12f + %16.12f*_Complex_I', real(x(i)), imag(x(i)));
    else,
        fprintf(fid,'  %16.12f', real(x(i)));
    end;

    if print_index_as_comment,
        c = mod(i-1,cols);  % row
        r = (i-c-1)/cols;   % column
        fprintf(fid,' /* (%2u,%2u) */', r, c);
    end;

    if i==n,
        fprintf(fid,'};\n');
    elseif mod(i,cols)==0 || one_line_per_element,
        fprintf(fid,',\n');
    else,
        fprintf(fid,', ');
    end;
end;
fprintf(fid,'\n');

