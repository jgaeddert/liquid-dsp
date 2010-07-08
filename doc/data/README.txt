=========================================================================

  doc/data/

=========================================================================

This directory contains some useful data relevant to signal processing.

doc/data/msequence
    This directory contains all maximal-length sequence generator
    polynomials of length m=2 (n=3) through m=15 (n=32767).  Each file
    is unique to the size of the shift register (m) and lists each
    generator polynomial's hexadecimal, octal, and binary
    representation.  The values are suitable for use directly in
    liquid's msequence initialization methods and do not need shifting,
    reordering, or additional manipulation.  For more information on
    m-sequence generation, refer to the section on the 'sequence' module
    in the guide.

