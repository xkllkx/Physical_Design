axis equal;
hold on;
grid on;
cap_x=[ 0 0 5175 5175 0 ];
cap_y=[ 0 10349 10349 0 0 ];
fill(cap_x,cap_y,'w');
block_x=[ 0 1832 1832 0 0 ];
block_y=[ 0 0 3186 3186 0 ];
fill(block_x,block_y,'c');
text(916,1593,'cc_21 (0)');
block_x=[ 0 1832 1832 0 0 ];
block_y=[ 3186 3186 6372 6372 3186 ];
fill(block_x,block_y,'c');
text(916,4779,'cc_22 (1)');
block_x=[ 1832 5018 5018 1832 1832 ];
block_y=[ 3652 3652 5484 5484 3652 ];
fill(block_x,block_y,'c');
text(3425,4568,'cc_23 (2)');
block_x=[ 1832 5018 5018 1832 1832 ];
block_y=[ 5484 5484 7316 7316 5484 ];
fill(block_x,block_y,'c');
text(3425,6400,'cc_24 (3)');
block_x=[ 0 1826 1826 0 0 ];
block_y=[ 6372 6372 9518 9518 6372 ];
fill(block_x,block_y,'c');
text(913,7945,'cc_11 (4)');
block_x=[ 1832 4978 4978 1832 1832 ];
block_y=[ 7316 7316 9142 9142 7316 ];
fill(block_x,block_y,'c');
text(3405,8229,'cc_12 (5)');
block_x=[ 1832 4978 4978 1832 1832 ];
block_y=[ 0 0 1826 1826 0 ];
fill(block_x,block_y,'c');
text(3405,913,'cc_13 (6)');
block_x=[ 1832 4978 4978 1832 1832 ];
block_y=[ 1826 1826 3652 3652 1826 ];
fill(block_x,block_y,'c');
text(3405,2739,'cc_14 (7)');
block_x=[ 1826 2652 2652 1826 1826 ];
block_y=[ 9142 9142 9428 9428 9142 ];
fill(block_x,block_y,'c');
text(2239,9285,'clk (8)');
