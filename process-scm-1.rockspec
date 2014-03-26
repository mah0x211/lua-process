package = "process"
version = "scm-1"
source = {
    url = "git://github.com/mah0x211/lua-process.git"
}
description = {
    summary = "process module",
    homepage = "https://github.com/mah0x211/lua-process",
    license = "MIT/X11",
    maintainer = "Masatoshi Teruya"
}
dependencies = {
    "lua >= 5.1"
}
build = {
    type = "builtin",
    modules = {
        process = {
            sources = { "process.c" },
        }
    }
}

