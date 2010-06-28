======================================
 frame-64 description
======================================

section             length          Description
ramp-up             64 symbols      BPSK phasing pattern
phasing pattern     64 symbols
p/n sequence        64 symbols      BPSK p/n sequence (m-sequence)
header              224 symbols     QPSK packet header
    header data     24 bytes        header data
    header crc      4 bytes         header cyclic redundancy check
payload             544 symbols     QPSK data payload
    payload data    64 bytes        payload data
    payload crc     4 bytes         payload cyclic redundancy check
ramp-dn             64 symbols      ramp down

total:              1024 symbols

Both the header and payload include a mandatory 32-bit crc with 1/2 rate
forward error-correction code.

