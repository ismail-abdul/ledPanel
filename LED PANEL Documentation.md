LED PANEL Documentation


Consider panel to have a 32 x 32 integer plane.
In realilty the panel is 64 x 16 (x, y). 

We can imagine that data is fed in via top right corner of the panel. And when data overflows from any given row, it is stored in the row that is 16 places below in the "second panel".

Each pixel is a collection of 3 consecutive bits. Each of which represent red, green and blue. In that order.