local signal = require('signal');
local cjson = require('cjson');
local fork = require('process').fork;
local exec = require('process').exec;
local waitpid = require('process').waitpid;
local env = require('process').getenv();
local cmd = './exec_test.lua';
local argv = { 'hello', 'world' };
local cmp = cjson.encode(cjson.decode(cjson.encode({
    [1] = "hello",
    [2] = "world",
    [-1] = "lua",
    [0] = cmd,
    stdin = argv,
})));
local pid, msg;

-- set env
cmd = ifNil( exec( cmd, argv, env, './' ) );

pid = ifNil( cmd:pid() );
ifNotEqual( type( pid ), 'number' );

-- send msg: should append LF
msg = cjson.encode( argv ) .. '\n';
ifNotEqual( cmd:stdin( msg ), #msg );

-- read json from stdout
msg = cjson.decode( cmd:stdout() );
msg = cjson.encode( msg );
ifNotEqual( cmp, msg );

-- read json from stderr
msg = cjson.decode( cmd:stderr() );
msg = cjson.encode( msg );
ifNotEqual( cmp, msg );

-- close test
local function close( sec )
    local chd = ifNil( fork() );
    
    if chd == 0 then
        if sec then
            sleep(sec);
        end
        cmd:kill();
        os.exit();
    end
end

close(1);
local status = waitpid( pid );
ifNotEqual( status.pid, pid );
ifNotEqual( status.termsig, signal.SIGTERM );

