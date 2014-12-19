local fork = require('process').fork;
local pid = ifNil( fork() );

if pid == 0 then
    os.exit();
end

