target("nbar")
    set_kind("shared")
    add_packages("sodium")
    add_files("src/*.c")

target("man-pages")
    set_kind("phony")
    add_deps("nbar")
    on_build(function (target)
        local man_pages = {"libnbar.3", "nbar_fopen.3", "nbar_fclose.3", "nbar_archive_t.3", "nbar_archive_header_t.3"}
        os.mkdir("doc/man")
        for _, page in ipairs(man_pages) do
            os.copyfile("doc/" .. page, "doc/man/" .. page)
        end
    end)