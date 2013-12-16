    .text
    .global get_sum

get_sum:
    vadd.f32  d0, d0, d1
    vadd.f32  s0, s0 ,s1
    bx         lr
   .type get_sum, function
   .size get_sum, .-get_sum

