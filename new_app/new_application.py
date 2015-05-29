import sys
import os

def parse_vars(var_file):
    table = {}
    for line in var_file:
        bits = line.partition("=")
        table[bits[0]] = bits[2].rstrip()                     
    return table

# Recursive check to handle multiple variables in a line.
def replace_var(line, table):
    if '$' in line: # TODO: Should be able to handle multiple vars
        bits = line.partition("$")
        new_line = bits[0]
        var_name = bits[2].partition("$")[0]
        if table.get(var_name) != None:
            new_line = new_line + table[var_name]
            return new_line + replace_var(bits[2].partition("$")[2], table)
        else:
            return new_line + "$" + replace_var(bits[2], table)
    else:
        return line

def copy_file(original_file_name, new_file_name):
    orig_file = open(original_file_name, 'r')
    new_file = open(new_file_name, 'w')
    for line in orig_file:
        new_file.write(replace_var(line, var_table))
    orig_file.close()
    new_file.close()
 
if len(sys.argv) < 1:
    print "Not enough args."
    print "Usage: python new_application.py <table_file>"
else:
    var_f = open(sys.argv[1])
    var_table = parse_vars(var_f)

    top_dir = var_table["application_name"] + "_app"
    template_dir = "Template"

    os.system("mkdir " + top_dir)
    os.system("mkdir " + top_dir + "/fsw")
    os.system("mkdir " + top_dir + "/fsw/for_build")
    copy_file(template_dir + "/Makefile", top_dir + "/fsw/for_build/Makefile")

    os.system("mkdir " + top_dir + "/fsw/mission_inc")
    copy_file(template_dir + "/template_perfids.h", top_dir + "/fsw/mission_inc/" + var_table["application_name"] + "_app_perfids.h")

    os.system("mkdir " + top_dir + "/fsw/platform_inc")
    copy_file(template_dir + "/template_msgids.h", top_dir + "/fsw/platform_inc/" + var_table["application_name"] + "_app_msgids.h")

    os.system("mkdir " + top_dir + "/fsw/src")
    copy_file(template_dir + "/template_events.h", top_dir + "/fsw/src/" + var_table["application_name"] + "_app_events.h")
    copy_file(template_dir + "/template.h", top_dir + "/fsw/src/" + var_table["application_name"] + "_app.h")
    copy_file(template_dir + "/template_main.c", top_dir + "/fsw/src/" + var_table["application_name"] + "_app_main.c")
    copy_file(template_dir + "/template_msg.h", top_dir + "/fsw/src/" + var_table["application_name"] + "_app_msg.h")
    copy_file(template_dir + "/template_version.h", top_dir + "/fsw/src/" + var_table["application_name"] + "_app_version.h")



