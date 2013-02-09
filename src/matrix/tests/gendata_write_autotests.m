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
% matrix_mul
%
x = randn(5,4);
y = randn(4,3);
z = x*y;
basename = 'matrixf_data_mul';
filename = [dirname basename '.c'];
%fid = fopen(filename,'w');
fid = stdout;
gendata_write_header(fid,filename);
gendata_write_matrix(fid,x,[basename '_x']);
gendata_write_matrix(fid,y,[basename '_y']);
gendata_write_matrix(fid,z,[basename '_z']);
%fclose(fid);

