# cartesian-product-multiprocessing

This is a proof-of-concept that aims to remedy [this issue](https://github.com/Texera/texera/issues/1915).

This only uses 3 processes at a time; this number can easily be expanded to whatever the limitations of the Texera project allow. Uses linux syscalls.

**Test Numbers:** <br>
Left table size = 50,000 <br>
Right table size = 100,000 <br>
Cross-joined table will be of size **5,000,000,000**.



Computing this without multiprocessing took **12.353 seconds**, while using 3-process multiprocessing decreases this time to **3.51 seconds**.





