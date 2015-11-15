local dup = require('process').dup;

ifNotNil( dup(3) );
for i = 2, 0, -1 do
    ifNil( dup(i) );
end
