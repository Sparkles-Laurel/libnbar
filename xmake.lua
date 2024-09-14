target("nbar")
    set_kind("shared")
    add_deps("sodium")
    add_files("src/*.c")