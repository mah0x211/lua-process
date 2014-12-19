local sleep = require('process').sleep;
local sec = os.time();

sleep(2);
ifNotEqual( os.time() - sec, 2 );
