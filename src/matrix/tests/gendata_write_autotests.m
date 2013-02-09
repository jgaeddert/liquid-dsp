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

%
% matrix_aug
%
x = randn(5,4);
y = randn(5,3);
z = [x y];
basename = 'matrixf_data_aug';
filename = [dirname basename '.c'];
fid = fopen(filename,'w');
gendata_write_header(fid,'data for testing matrix augmentation');
gendata_write_matrix(fid,x,[basename '_x']);
gendata_write_matrix(fid,y,[basename '_y']);
gendata_write_matrix(fid,z,[basename '_z']);
fclose(fid);

%
% matrix_inv
%
x = randn(5,5);
y = inv(x);
basename = 'matrixf_data_inv';
filename = [dirname basename '.c'];
fid = fopen(filename,'w');
gendata_write_header(fid,'data for testing matrix inversion');
gendata_write_matrix(fid,x,[basename '_x']);
gendata_write_matrix(fid,y,[basename '_y']);
fclose(fid);

%
% matrix_linsolve
%
A = randn(5,5);
x = randn(5,1);
b = A*x;
basename = 'matrixf_data_linsolve';
filename = [dirname basename '.c'];
fid = fopen(filename,'w');
gendata_write_header(fid,'data for testing linear solver');
gendata_write_matrix(fid,A,[basename '_A']);
gendata_write_matrix(fid,x,[basename '_x']);
gendata_write_matrix(fid,b,[basename '_b']);
fclose(fid);

%
% matrix_ludecomp
%
A = [   4,  -3,  -1,   3;
        1,   1,   0,   2;
       -1,   0,   1,   1;
       -2,  -5,   1,  -3];
[L U] = lu(A);
basename = 'matrixf_data_ludecomp';
filename = [dirname basename '.c'];
fid = fopen(filename,'w');
gendata_write_header(fid,'data for testing L/U decomposition');
gendata_write_matrix(fid,A,[basename '_A']);
gendata_write_matrix(fid,L,[basename '_L']);
gendata_write_matrix(fid,U,[basename '_U']);
fclose(fid);

%
% matrix_qrdecomp
%
A = [1 2 3 4; 5 5 7 8; 6 4 8 7; 1 0 3 1];
[Q R] = qr(A);
basename = 'matrixf_data_qrdecomp';
filename = [dirname basename '.c'];
fid = fopen(filename,'w');
gendata_write_header(fid,'data for testing Q/R decomposition');
gendata_write_matrix(fid,A,[basename '_A']);
gendata_write_matrix(fid,Q,[basename '_Q']);
gendata_write_matrix(fid,R,[basename '_R']);
fclose(fid);

%
% matrix_gramschmidt
%
A = [1 2 1; 0 2 0; 2 3 1; 1 1 0];
V = [sqrt(6)/6, sqrt(2)/6,    2/3 ;
     0          2*sqrt(2)/3, -1/3 ;
     sqrt(6)/3, 0,            0   ;
     sqrt(6)/6, -sqrt(2)/6,  -2/3 ];
basename = 'matrixf_data_gramschmidt';
filename = [dirname basename '.c'];
fid = fopen(filename,'w');
gendata_write_header(fid,'data for testing Gram-Schmidt orthonormalization');
gendata_write_matrix(fid,A,[basename '_A']);
gendata_write_matrix(fid,V,[basename '_V']);
fclose(fid);

%
% matrix_chol
%
L = [   1.01,   0,      0,      0;
       -1.42,   0.5,    0,      0;
        0.32,   2.01,   0.3,    0;
       -1.02,  -0.32,  -1.65,   1.07];
A = L * L';
basename = 'matrixf_data_chol';
filename = [dirname basename '.c'];
fid = fopen(filename,'w');
gendata_write_header(fid,'data for testing Cholesky decomposition');
gendata_write_matrix(fid,L,[basename '_L']);
gendata_write_matrix(fid,A,[basename '_A']);
fclose(fid);

%
% matrix_cgsolve8
%
A = [
  12.7229204   0.0000000  -7.9529123   0.0000000   4.1114998   0.0000000   0.0000000   0.0000000
   0.0000000   0.0651512   0.0000000  -0.2182598   0.0000000   0.0000000   0.0000000   0.0000000
  -7.9529123   0.0000000   5.0315852   0.0000000  -2.5700388  -0.1105457   0.0000000   0.0000000
   0.0000000  -0.2182598   0.0000000   0.7330456   0.0000000   0.0000000   0.0000000   0.0000000
   4.1114998   0.0000000  -2.5700388   0.0000000   1.3381329   0.2393810   0.0784302   0.0000000
   0.0000000   0.0000000  -0.1105457   0.0000000   0.2393810   7.4723883   1.9818947  -1.3733650
   0.0000000   0.0000000   0.0000000   0.0000000   0.0784302   1.9818947   3.4892726   0.0000000
   0.0000000   0.0000000   0.0000000   0.0000000   0.0000000  -1.3733650   0.0000000   2.0631149];
x = [
   0.1620522
  -0.0127203
   1.0433751
   0.0062052
   0.8781570
   0.1464129
   0.7825852
  -0.8257846];
b = A*x;
basename = 'matrixf_data_cgsolve8';
filename = [dirname basename '.c'];
fid = fopen(filename,'w');
gendata_write_header(fid,'data for testing conjugate gradient solver');
gendata_write_matrix(fid,A,[basename '_A']);
gendata_write_matrix(fid,x,[basename '_x']);
gendata_write_matrix(fid,b,[basename '_b']);
fclose(fid);

