local signal = require('signal');
local kill = require('signal').kill;
local process = require('process');
local fork = process.fork;
local waitpid = process.waitpid;
local status, pid;

local function wait( ... )
    local pid = ifNil( fork() );
    local status;
    
    if pid == 0 then
        sleep(1);
        os.exit();
    else
        return pid, ifNil( waitpid( pid, ... ) );
    end
end


local function sendSignal( pid, signo, sec )
    local chd = ifNil( fork() );
    
    if chd == 0 then
        if sec then
            sleep(sec);
        end
        kill( pid, signo );
        os.exit();
    end
end


local function waitSigStop()
    local pid = ifNil( fork() );
    local status;
    
    if pid == 0 then
        sleep(2);
        os.exit();
    else
        sendSignal( pid, signal.SIGSTOP, 1 );
        return pid, ifNil( waitpid( pid, process.WUNTRACED ) );
    end
end


local function waitSigContinued()
    local pid = ifNil( fork() );
    local status;
    
    if pid == 0 then
        sleep(2);
        os.exit();
    else
        sendSignal( pid, signal.SIGSTOP );
        sendSignal( pid, signal.SIGCONT, 1 );
        return pid, ifNil( waitpid( pid, process.WCONTINUED ) );
    end
end


local function waitSigTerm()
    local pid = ifNil( fork() );
    local status;
    
    if pid == 0 then
        sleep(2);
        os.exit();
    else
        sendSignal( pid, signal.SIGQUIT, 1 );
        return pid, ifNil( waitpid( pid ) );
    end
end


-- wait exit
pid, status = wait();
ifNotEqual( status.pid, pid );
ifNotEqual( type( status.exit ), 'number' );

-- wait nohang
pid, status = wait( process.WNOHANG );
ifNotEqual( status.pid, pid );
ifNotTrue( status.nohang );

-- wait stop
pid, status = waitSigStop();
ifNotEqual( status.pid, pid );
ifNotEqual( type( status.stopsig ), 'number' );

-- wait stop and continue
pid, status = waitSigContinued();
ifNotEqual( status.pid, pid );
ifNotTrue( status.continue );

-- wait term signal
pid, status = waitSigTerm();
ifNotEqual( status.pid, pid );
ifNotEqual( type( status.termsig ), 'number' );


