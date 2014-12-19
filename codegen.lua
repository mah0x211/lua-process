local inspect = require('util').inspect;

local TMPL = [=[
#ifdef %s
    lstate_num2tbl( L, "%s", %s );
#endif

]=];
local decls = {};
local decl, file, def, tbl, arr;

-- load declaration
for _, path in ipairs( _G.arg ) do
    if path:find('%.txt$') then
        file = assert( io.open( path ) );
        decl = ('GEN_%s_DECL'):format( path:match('/(.+).txt'):upper() );
        
        -- remove duplicate
        tbl = {};
        for _ in file:lines() do
            def = _:match('^[A-Z0-9_]+$' );
            if not def then
                error( 'invalid line: ' .. _ );
            end
            tbl[def] = true;
        end
        file:close();
        
        -- to array
        arr = {};
        for _ in pairs( tbl ) do
            arr[#arr+1] = _;
        end
        table.sort( arr );
        
        -- generate
        def = '';
        for _, v in ipairs( arr ) do
            def = def .. TMPL:format( v, v, v );
        end
        
        -- save decl
        decls[decl] = def;
    end
end

-- inject declaration
for _, path in ipairs( _G.arg ) do
    if path:find('%.c$') then
        file = assert( io.open( path ) ):read('*a');
        for k, v in pairs( decls ) do
            file = file:gsub( '#define (' .. k .. ')\n', v );
        end
        -- to src path
        path = path:gsub( '^tmpl/', 'src/' ):gsub( '_tmpl%.c', '.c' );
        assert( io.open( path, 'w' ) ):write( file );
    end
end

