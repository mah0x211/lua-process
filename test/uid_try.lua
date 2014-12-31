local getuid = require('process').getuid;
local getuname = require('process').getuname;
local setuid = require('process').setuid;
local seteuid = require('process').seteuid;
local setreuid = require('process').setreuid;
local uid, uname;

uid = ifNil( getuid() );
uname = ifNil( getuname( uid ) );
ifNotEqual( uname, getuname() );
ifNotEqual( uid, getuid( uname ) );
ifNotNil( getuid('invalid uid') );

ifNotNil( setuid( uid ) );
ifNotNil( seteuid( uid ) );

ifNotNil( setuid( uname ) );
ifNil( setuid('invalid uid') );

ifNotNil( seteuid( uname ) );
ifNil( seteuid('invalid uid') );

ifNotNil( setreuid( uid, uid ) );
ifNotNil( setreuid( uname, uname ) );
ifNil( setreuid( 'invalid uid', 'invalid uid' ) );

ifNotEqual( uid, getuid() );

