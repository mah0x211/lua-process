#!/usr/bin/env lua

local cjson = require('cjson');
local sleep = require('process').sleep;
local msg = _G.arg;

msg.env = os.getenv('EXEC_TEST');
msg.stdin = cjson.decode( io.input():read() );
msg = cjson.encode( msg );
-- stdout
print( msg );
-- stderr
io.stderr:write( msg );
-- flush
io.flush();
-- busy loop
while true do
    sleep(1);
end
