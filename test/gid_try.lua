local getgid = require('process').getgid;
local getgname = require('process').getgname;
local setgid = require('process').setgid;
local setegid = require('process').setegid;
local setregid = require('process').setregid;
local gid, gname;

gid = ifNil( getgid() );
gname = ifNil( getgname( gid ) );
ifNotEqual( gname, getgname() );
ifNotEqual( gid, getgid( gname ) );

ifNotNil( getgid('invalid gid') );

ifNotNil( setgid( gid ) );
ifNotNil( setegid( gid ) );

ifNotNil( setgid( gname ) );
ifNil( setgid('invalid gid') );

ifNotNil( setegid( gname ) );
ifNil( setegid('invalid gid') );

ifNotNil( setregid( gid, gid ) );
ifNotNil( setregid( gname, gname ) );
ifNil( setregid( 'invalid gid', 'invalid gid' ) );

ifNotEqual( gid, getgid() );

