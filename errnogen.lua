
local function openFile()
    local file = io.open('./errno.txt');
    local errno, _, k, v;
    local tbl = {};
    local arr = {};

    -- remove duplicate
    for _ in file:lines() do
        errno = _:match('^E[A-Z0-9]+$' );
        if not errno then
            error( 'invalid line: ' .. _ );
        end
        tbl[errno] = true;
    end

    for _ in pairs( tbl ) do
        arr[#arr+1] = _;
    end

    table.sort( arr );

    return arr;
end


local function codeGen( arr )
    local fmtConstants = [[
#ifdef %s
    lstate_num2tbl( L, "%s", %s );
#endif

]];
    local errnoConstants = '';
    local _, v;

    for i, v in ipairs( arr ) do
        errnoConstants = errnoConstants .. fmtConstants:format( v, v, v );
    end

    return {
        DECL = errnoConstants
    };
end


local function inject( tbl )
    local file = io.open('./process_tmpl.c'):read('*a');
    local replace = function( match )
        return tbl[match];
    end

    file = file:gsub( '#define GEN_ERRNO_(%w+)\n', replace );
    io.open( './process.c', 'w' ):write( file );
end

inject( codeGen( openFile() ) );
