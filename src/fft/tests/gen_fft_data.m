% generate fft data for autotests

n = round(n);

x = randn(1,n) + j*randn(1,n);
y = fft(x);

% print results

filename = ['fft_data_' num2str(n) '.h'];
fid = fopen(filename,'w');

%fprintf(fid,'// %s : auto-generated file\n', filename);
fprintf(fid,'\n');

fprintf(fid,'#ifndef __LIQUID_AUTOTEST_FFT_DATA_%u__\n', n);
fprintf(fid,'#define __LIQUID_AUTOTEST_FFT_DATA_%u__\n', n);
fprintf(fid,'\n');

fprintf(fid,'// %d-point fft data\n', n);

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

fprintf(fid,'#endif // __LIQUID_AUTOTEST_FFT_DATA_%u__\n\n', n);

fclose(fid);
printf('results written to %s\n', filename);
