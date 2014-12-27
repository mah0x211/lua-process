local getgid = require('process').getgid;
local setgid = require('process').setgid;
local setegid = require('process').setegid;
local setregid = require('process').setregid;
local gid;

ifNil( getgid() );
gid = ifNil( getgid('staff') );
ifNotNil( getgid('invalid gid') );

ifNotNil( setgid( gid ) );
ifNotNil( setegid( gid ) );

ifNotNil( setgid('staff') );
ifNil( setgid('invalid gid') );

ifNotNil( setegid('staff') );
ifNil( setegid('invalid gid') );

ifNotNil( setregid( gid, gid ) );
ifNotNil( setregid( 'staff', 'staff' ) );
ifNil( setregid( 'invalid gid', 'invalid gid' ) );

ifNotEqual( gid, getgid() );

