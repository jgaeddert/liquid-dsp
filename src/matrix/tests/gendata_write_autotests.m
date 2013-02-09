%
% Write matrix autotest data
%

clear all;
close all;

% global options
dirname = 'data/';  % output directory name
randn('seed',0.0);  % set random seed for normal distribution
rand('seed',0.0);   % set random seed for uniform distribution

%
% matrix_add
%
x = randn(5,4);
y = randn(5,4);
z = x + y;
basename = 'matrixf_data_add';
filename = [dirname basename '.c'];
fid = fopen(filename,'w');
gendata_write_header(fid,'data for testing matrix addition');
gendata_write_matrix(fid,x,[basename '_x']);
gendata_write_matrix(fid,y,[basename '_y']);
gendata_write_matrix(fid,z,[basename '_z']);
fclose(fid);

%
% matrix_mul
%
x = randn(5,4);
y = randn(4,3);
z = x*y;
basename = 'matrixf_data_mul';
filename = [dirname basename '.c'];
fid = fopen(filename,'w');
gendata_write_header(fid,'data for testing matrix multiplication');
gendata_write_matrix(fid,x,[basename '_x']);
gendata_write_matrix(fid,y,[basename '_y']);
gendata_write_matrix(fid,z,[basename '_z']);
fclose(fid);

