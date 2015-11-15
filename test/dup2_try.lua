local dup2 = require('process').dup2;
local new = 10;
local fd;

ifNotNil( dup2( 3, new ) );
for i = 2, 0, -1 do
    fd = ifNil( dup2( i, new + i ) );
    ifNotEqual( fd, new + i );
end
