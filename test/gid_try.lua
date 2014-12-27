local getgid = require('process').getgid;
local setgid = require('process').setgid;
local setegid = require('process').setegid;
local gid;

ifNil( getgid() );
gid = ifNil( getgid('staff') );
ifNotNil( setgid( gid ) );
ifNotNil( setegid( gid ) );
ifNotNil( setgid('staff') );
ifNotNil( setegid('staff') );
ifNotEqual( gid, getgid() );

