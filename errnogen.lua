
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
    local fmtMethod = [[
#ifdef %s
static int is%s_lua( lua_State *L ){
    return iserrno_lua( L, %s );
}
#endif

]];

    local fmtMethodDef = [[
#ifdef %s
        { "is%s", is%s_lua },
#endif

]];
    local method = '';
    local methodDef = '';
    local _, v;

    for i, v in ipairs( arr ) do
        method = method .. fmtMethod:format( v, v, v );
        methodDef = methodDef .. fmtMethodDef:format( v, v, v );
    end

    return {
        IMPL = method,
        DECL = methodDef
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
