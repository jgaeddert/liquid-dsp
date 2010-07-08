
clear all;
close all;

a = floor(rand(1) * 2^16);
b = floor(rand(1) * 2^16);

as = dec2bin(a,16);
bs = dec2bin(b,16);
ap = [];
bp = [];
for i=1:16,
    ap = [ap str2num(as(i))];
    bp = [bp str2num(bs(i))];
end;

bit = mod(sum(ap.*bp),2);

printf('CONTEND_EQUALITY( liquid_bdotprod(0x%.4x, 0x%.4x), %1u);\n', a,b,bit);

