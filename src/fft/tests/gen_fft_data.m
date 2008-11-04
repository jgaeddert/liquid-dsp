% generate fft data for autotests

x = randn(1,n) + j*randn(1,n);
y = fft(x);

% print results

printf('// %d-point fft data\n', n);

printf('float complex x%d[] = {\n',n);
for i=1:n,
    printf('%16.12f + %16.12f*J', real(x(i)), imag(x(i)));
    if i==n,
        printf('\n};\n');
    else,
        printf(',\n');
    end;
end;

printf('\n');

printf('float complex test%d[] = {\n',n);
for i=1:n,
    printf('%16.12f + %16.12f*J', real(y(i)), imag(y(i)));
    if i==n,
        printf('\n};\n');
    else,
        printf(',\n');
    end;
end;

