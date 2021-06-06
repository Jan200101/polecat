get_filename_component(bin_name ${bin_in} NAME)
string(REGEX REPLACE "\\.| |-" "_" bin_name ${bin_name})

set(c_out ${bin_name}.c)
set(h_out ${bin_name}.h)

file(READ ${bin_in} filedata HEX)

string(REGEX REPLACE  "([0-9a-f][0-9a-f])" "0x\\1," filedata ${filedata})

file(WRITE ${c_out} "#include <stdlib.h>\nconst unsigned char ${bin_name}[] = {${filedata}};\nconst size_t ${bin_name}_size = sizeof(${bin_name});\n")
file(WRITE ${h_out} "extern const unsigned char ${bin_name}[];\nextern const size_t ${bin_name}_size;\n")
