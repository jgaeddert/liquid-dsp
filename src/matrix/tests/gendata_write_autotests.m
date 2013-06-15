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
% real floating-point data
%

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
% matrix_cgsolve
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
basename = 'matrixf_data_cgsolve';
filename = [dirname basename '.c'];
fid = fopen(filename,'w');
gendata_write_header(fid,'data for testing conjugate gradient solver');
gendata_write_matrix(fid,A,[basename '_A']);
gendata_write_matrix(fid,x,[basename '_x']);
gendata_write_matrix(fid,b,[basename '_b']);
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
A = randn(8,8);
basename = 'matrixf_data_ludecomp';
filename = [dirname basename '.c'];
fid = fopen(filename,'w');
gendata_write_header(fid,'data for testing L/U decomposition');
gendata_write_matrix(fid,A,[basename '_A']);
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
% matrix_transmul
% 
% transpose_mul
% hermitian_mul
% mul_transpose
% mul_hermitian
%
x = randn(5,4);
xxT = x   * x';
xxH = x   * x.';
xTx = x'  * x;
xHx = x.' * x;
basename = 'matrixf_data_transmul';
filename = [dirname basename '.c'];
fid = fopen(filename,'w');
gendata_write_header(fid,'data for testing multiply/transpose');
gendata_write_matrix(fid,x,  [basename '_x']);
gendata_write_matrix(fid,xxT,[basename '_xxT']);
gendata_write_matrix(fid,xxH,[basename '_xxH']);
gendata_write_matrix(fid,xTx,[basename '_xTx']);
gendata_write_matrix(fid,xHx,[basename '_xHx']);
fclose(fid);





%
% complex floating-point data
%

%
% matrix_add
%
x = randn(5,4) + j*randn(5,4);
y = randn(5,4) + j*randn(5,4);
z = x + y;
basename = 'matrixcf_data_add';
filename = [dirname basename '.c'];
fid = fopen(filename,'w');
gendata_write_header(fid,'data for testing matrix addition');
gendata_write_matrix(fid,x,[basename '_x']);
gendata_write_matrix(fid,y,[basename '_y']);
gendata_write_matrix(fid,z,[basename '_z']);
fclose(fid);

%
% matrix_aug
%
x = randn(5,4) + j*randn(5,4);
y = randn(5,3) + j*randn(5,3);
z = [x y];
basename = 'matrixcf_data_aug';
filename = [dirname basename '.c'];
fid = fopen(filename,'w');
gendata_write_header(fid,'data for testing matrix augmentation');
gendata_write_matrix(fid,x,[basename '_x']);
gendata_write_matrix(fid,y,[basename '_y']);
gendata_write_matrix(fid,z,[basename '_z']);
fclose(fid);

%
% matrix_chol
%
L = [   1.01,       0,          0,          0;
       -1.42+0.25j, 0.5,        0,          0;
        0.32-1.23j, 2.01+0.78j, 0.3,        0;
       -1.02+1.02j,-0.32-0.03j,-1.65+2.01j, 1.07];
A = L * L';
basename = 'matrixcf_data_chol';
filename = [dirname basename '.c'];
fid = fopen(filename,'w');
gendata_write_header(fid,'data for testing Cholesky decomposition');
gendata_write_matrix(fid,L,[basename '_L']);
gendata_write_matrix(fid,A,[basename '_A']);
fclose(fid);

%
% matrix_inv
%
x = randn(5,5) +j*randn(5,5);
y = inv(x);
basename = 'matrixcf_data_inv';
filename = [dirname basename '.c'];
fid = fopen(filename,'w');
gendata_write_header(fid,'data for testing matrix inversion');
gendata_write_matrix(fid,x,[basename '_x']);
gendata_write_matrix(fid,y,[basename '_y']);
fclose(fid);

%
% matrix_linsolve
%
A = randn(5,5) + j*randn(5,5);
x = randn(5,1) + j*randn(5,1);
b = A*x;
basename = 'matrixcf_data_linsolve';
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
A = randn(8,8) + j*randn(8,8);
basename = 'matrixcf_data_ludecomp';
filename = [dirname basename '.c'];
fid = fopen(filename,'w');
gendata_write_header(fid,'data for testing L/U decomposition');
gendata_write_matrix(fid,A,[basename '_A']);
fclose(fid);

%
% matrix_mul
%
x = randn(5,4) + j*randn(5,4);
y = randn(4,3) + j*randn(4,3);
z = x*y;
basename = 'matrixcf_data_mul';
filename = [dirname basename '.c'];
fid = fopen(filename,'w');
gendata_write_header(fid,'data for testing matrix multiplication');
gendata_write_matrix(fid,x,[basename '_x']);
gendata_write_matrix(fid,y,[basename '_y']);
gendata_write_matrix(fid,z,[basename '_z']);
fclose(fid);

%
% matrix_qrdecomp
%
A = [  2.11402 - 0.57604i,  0.41750 + 1.00833i, -0.96264 - 3.62196i, -0.20679 - 1.02668i,
       0.00854 + 1.61626i,  0.84695 - 0.32736i, -1.01862 - 1.10786i, -1.78877 + 1.84456i,
      -2.97901 - 1.30384i,  0.52289 + 1.89110i,  1.32576 - 0.36737i,  0.04717 + 0.20628i,
       0.28970 + 0.64247i, -0.55916 + 0.68302i,  1.40615 + 0.62398i, -0.12767 - 0.53997i];
[Q R] = qr(A);
basename = 'matrixcf_data_qrdecomp';
filename = [dirname basename '.c'];
fid = fopen(filename,'w');
gendata_write_header(fid,'data for testing Q/R decomposition');
gendata_write_matrix(fid,A,[basename '_A']);
gendata_write_matrix(fid,Q,[basename '_Q']);
gendata_write_matrix(fid,R,[basename '_R']);
fclose(fid);

%
% matrix_transmul
%
% transpose_mul
% hermitian_mul
% mul_transpose
% mul_hermitian
%
x = randn(5,4) + j*randn(5,4);
xxT = x   * x';
xxH = x   * x.';
xTx = x'  * x;
xHx = x.' * x;
basename = 'matrixcf_data_transmul';
filename = [dirname basename '.c'];
fid = fopen(filename,'w');
gendata_write_header(fid,'data for testing multiply/transpose');
gendata_write_matrix(fid,x,  [basename '_x']);
gendata_write_matrix(fid,xxT,[basename '_xxT']);
gendata_write_matrix(fid,xxH,[basename '_xxH']);
gendata_write_matrix(fid,xTx,[basename '_xTx']);
gendata_write_matrix(fid,xHx,[basename '_xHx']);
fclose(fid);

