import sys, argparse, os
#Generates enum from config file

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("generated_file", help="""The path of the generated
                        header file""")
    parser.add_argument("config_file", help="The path of the config file")
    parser.add_argument("enum_name", help="The name of the enum to create")
    args = parser.parse_args()

    configFile = open(args.config_file, 'r')
    genFile = open(args.generated_file.replace(".","_gen."), 'w')

    headerGuard = os.path.basename(args.generated_file)
    headerGuard = "_{0}_".format(headerGuard.upper().replace(".","_GEN_"))
    genFile.write("#ifndef {0}\n".format(headerGuard))
    genFile.write("#define {0}\n\n".format(headerGuard))
    genFile.write("enum {0} {{\n".format(args.enum_name))

    for line in filter(lambda line: line.strip()[0] is not "#", configFile):
        genFile.write("  " + line.strip() + ",\n")

    genFile.write("};\n")
    genFile.write("#endif  //{0}\n".format(headerGuard))

if __name__ == "__main__":
    main()
