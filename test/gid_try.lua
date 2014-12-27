local getgid = require('process').getgid;
local setgid = require('process').setgid;
local gid;

ifNil( getgid() );
gid = ifNil( getgid('staff') );
ifNotNil( setgid( gid ) );
ifNotNil( setgid('staff') );
ifNotEqual( gid, getgid() );

