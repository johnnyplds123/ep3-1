import getopt, sys, os, fnmatch

#-----------------------------------------------------------------------------
#  
#-----------------------------------------------------------------------------
def Usage():
    print ("Marvell concatenation copy tool")
    print ("Usage: mvconcat {input_files} output_file")
    return

#=============================================================================
#  
#=============================================================================
def py_main(argc, argv):

    if (argc<2):
        Usage()
        exit(1)

    file_out = open(argv[argc-1], "wb")

    for i in range(0, argc-1):
        if os.path.isfile(argv[i]):
            file_in = open(argv[i], "rb")
            data = file_in.read()
            file_in.close()
            file_out.write(data)
            #print ("file concatenation %s" %(argv[i]))
        else:
            print ('File %s not exist!' % argv[i])

    file_out.close()            

#=============================================================================
#  
#=============================================================================
if __name__ == "__main__":
    py_main(len(sys.argv)-1, sys.argv[1:])        

