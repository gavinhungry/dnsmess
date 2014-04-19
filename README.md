dnsmess
=======
An IPv4 DNS lookup/reverse lookup tool with full request/response output.  The
DNS server used is hardcoded to [Google Public DNS](https://developers.google.com/speed/public-dns)
(see `dnsmess.h`).

Usage
-----
    $ make
    $ dnsmess [DOMAIN | IP]

Example
-------

    $ dnsmess google.com

    DNS Query (28 bytes):
    47 4C 01 00 00 01 00 00 00 00 00 00 06 67 6F 6F  GL...........goo
    67 6C 65 03 63 6F 6D 00 00 01 00 01 -- -- -- --  gle.com.....

    DNS Response (204 bytes):
    47 4C 81 80 00 01 00 0B 00 00 00 00 06 67 6F 6F  GL...........goo
    67 6C 65 03 63 6F 6D 00 00 01 00 01 C0 0C 00 01  gle.com.........
    00 01 00 00 00 35 00 04 AD C2 21 06 C0 0C 00 01  .....5....!.....
    00 01 00 00 00 35 00 04 AD C2 21 0E C0 0C 00 01  .....5....!.....
    00 01 00 00 00 35 00 04 AD C2 21 07 C0 0C 00 01  .....5....!.....
    00 01 00 00 00 35 00 04 AD C2 21 02 C0 0C 00 01  .....5....!.....
    00 01 00 00 00 35 00 04 AD C2 21 00 C0 0C 00 01  .....5....!.....
    00 01 00 00 00 35 00 04 AD C2 21 03 C0 0C 00 01  .....5....!.....
    00 01 00 00 00 35 00 04 AD C2 21 04 C0 0C 00 01  .....5....!.....
    00 01 00 00 00 35 00 04 AD C2 21 05 C0 0C 00 01  .....5....!.....
    00 01 00 00 00 35 00 04 AD C2 21 08 C0 0C 00 01  .....5....!.....
    00 01 00 00 00 35 00 04 AD C2 21 09 C0 0C 00 01  .....5....!.....
    00 01 00 00 00 35 00 04 AD C2 21 01 -- -- -- --  .....5....!.

    IPv4 address(es) for google.com:
      173.194.33.6
      173.194.33.14
      173.194.33.7
      173.194.33.2
      173.194.33.0
      173.194.33.3
      173.194.33.4
      173.194.33.5
      173.194.33.8
      173.194.33.9
      173.194.33.1

### Reverse DNS

    $ dnsmess 173.194.33.6

    DNS Query (43 bytes):
    47 4C 01 00 00 01 00 00 00 00 00 00 01 36 02 33  GL...........6.3
    33 03 31 39 34 03 31 37 33 07 69 6E 2D 61 64 64  3.194.173.in-add
    72 04 61 72 70 61 00 00 0C 00 01 -- -- -- -- --  r.arpa.....

    DNS Response (81 bytes):
    47 4C 81 80 00 01 00 01 00 00 00 00 01 36 02 33  GL...........6.3
    33 03 31 39 34 03 31 37 33 07 69 6E 2D 61 64 64  3.194.173.in-add
    72 04 61 72 70 61 00 00 0C 00 01 C0 0C 00 0C 00  r.arpa..........
    01 00 00 54 5F 00 1A 0E 73 65 61 30 39 73 30 31  ...T_...sea09s01
    2D 69 6E 2D 66 36 05 31 65 31 30 30 03 6E 65 74  -in-f6.1e100.net
    00 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --  .

    Domain name(s) for 173.194.33.6:
      sea09s01-in-f6.1e100.net

License
-------
dnsmess is released under the terms of the [MIT license](http://tldrlegal.com/license/mit-license).
