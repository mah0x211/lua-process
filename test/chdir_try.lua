local getcwd = require('process').getcwd;
local chdir = require('process').chdir;
-- save cwd
local cwd = getcwd();
local chd;

ifNotNil( chdir('/') );
chd = getcwd();
-- revert to cwd
ifNotNil( chdir( cwd ) );

ifEqual( cwd, chd );
