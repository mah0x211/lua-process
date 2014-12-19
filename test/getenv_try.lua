local getenv = require('process').getenv;
local env = ifNil( getenv() );

ifNotEqual( type( env ), 'table' );
