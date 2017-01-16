import getopt, sys, os, fnmatch
import re
#-----------------------------------------------------------------------------
#  Offset  Description
#-----------------------------------------------------------------------------
#  0       identifier.  : 5Ah    
#  1       CPU select  0: Download target is CPU0
#                      1: Download target is CPU1
#                      2: Download target is CPU2
#  3:2     Reserved Reserved.
#  7:4     Block Size Source Block size in bytes to be downloaded. 
#          Byte 4 is the LSB; byte 7 is the MSB.
#          
#  11:8    Packet Size The source block can be broken into smaller packets, each of
#          which has one checksum byte at the end. The packet size number
#          does not include the checksum byte. If the block size is not a
#          multiple of (PacketSize+1), the last portion of the block (after all
#          packets are transferred) does not have checksum.
#  15:12   Source Address The Source Address is the Flash address of the source block. Byte
#          12 is the LSB; byte 15 is the MSB. The address should be within
#          the Flash address space. The whole block must fit in the Flash
#          space from this address.
#  19:16   Destination Address The Destination Address is the download destination address of
#          the code. Byte 16 is the LSB; byte 19 is the MSB. The Destination
#          Address can be a CPU0/CPU1 ATCM or an SPM address.
#  23:20   Execution Address After the block is downloaded and no error is detected, the ROM
#          passes control to this address. Byte 20 is the LSB; byte 23 is the MSB.
#          The Execution Address can be a CPU0/CPU1 ATCM or an SPM address.
#  30:24   Reserved Reserved.
#  31      Header Checksum Header Checksum (code size+31):32 Code to be downloaded
#-----------------------------------------------------------------------------
def MakeDownHeader(cpuId, srcSize, srcAddress=0x20, destAddress=0, execAddress=0, bootfw=0):

    #print ("Make UART header, cpuId=%d srcSze=%d" %(cpuId, srcSize))

    header = bytearray([0]*32)    # signature
    
    if bootfw == 0:
        header[0] = 0x99
    else:
        header[0] = 0x5A
        
    header[1] = cpuId
    header = SetU32(header, 4, srcSize+1)
    header = SetU32(header, 8, srcSize)
    header = SetU32(header,12, srcAddress)
    header = SetU32(header,16, destAddress)
    header = SetU32(header,20, execAddress)
    header[31] = sum(header[0:31])%256
   
    return header

#-----------------------------------------------------------------------------
#  Offset  Description
#-----------------------------------------------------------------------------
#  0       identifier.  : 5Ah    
#  1       CPU select  0: Download target is CPU0
#                      1: Download target is CPU1
#                      2: Download target is CPU2
#  3:2     Reserved Reserved.
#  7:4     Block Size Source Block size in bytes to be downloaded. 
#          Byte 4 is the LSB; byte 7 is the MSB.
#          
#  11:8    Reserved 
#  15:12   Reserved
#  19:16   Destination Address The Destination Address is the download destination address of
#          the code. Byte 16 is the LSB; byte 19 is the MSB. The Destination
#          Address can be a CPU0/CPU1 ATCM or an SPM address.
#  23:20   Execution Address After the block is downloaded and no error is detected, the ROM
#          passes control to this address. Byte 20 is the LSB; byte 23 is the MSB.
#          The Execution Address can be a CPU0/CPU1 ATCM or an SPM address.
#  27:24   Reserved
#  31:28   Header 32bit Checksum
#-----------------------------------------------------------------------------
def MakePcieDownHeader(cpuId, srcSize, srcAddress=0x20, destAddress=0, execAddress=0, bootfw=0):

    #print ("Make PCIe header, cpuId=%d srcSze=%d" %(cpuId, srcSize))

    header = bytearray([0]*32)    # signature
    if bootfw == 0:
        header[0] = 0x99
    else:
        header[0] = 0x5A
   
    header[1] = cpuId
    header = SetU32(header,  4, srcSize)
    header = SetU32(header, 16, destAddress)
    header = SetU32(header, 20, execAddress)
    header = SetU32(header, 28, CheckSumU32(header, 0, 28))
   
    return header

#-----------------------------------------------------------------------------
#  
#-----------------------------------------------------------------------------
def GetU32(barray, index):

    return barray[index+0] + 256*barray[index+1] + 256*256*barray[index+2] + 256*256*256*barray[index+3]

#-----------------------------------------------------------------------------
#  
#-----------------------------------------------------------------------------
def SetU32(barray, index, value):
    barray[index+0] = ((value>>0)%256)
    barray[index+1] = ((value>>8)%256)
    barray[index+2] = ((value>>16)%256)
    barray[index+3] = ((value>>24)%256)

    return barray

#-----------------------------------------------------------------------------
#  
#-----------------------------------------------------------------------------
def SetU16(barray, index, value):
    barray[index+0] = ((value>>0)%256)
    barray[index+1] = ((value>>8)%256)

    return barray

#-----------------------------------------------------------------------------
#  
#-----------------------------------------------------------------------------
def CheckSumU8(data):  
    return sum(data[0:len(data)-1])
    
#-----------------------------------------------------------------------------
#  
#-----------------------------------------------------------------------------
def CheckSumU16(data):
    return 0
    

#-----------------------------------------------------------------------------
#  return 32bit checksum
#-----------------------------------------------------------------------------
def CheckSumU32(data, startIndex, end1Index):
    sum32=0
    
    for index in range(startIndex, end1Index, 4):
        sum32 = sum32 + GetU32(data, index)
        
    return sum32 & 0xFFFFFFFF
    
#-----------------------------------------------------------------------------
#  
#-----------------------------------------------------------------------------
def FileSize(file):
    return os.path.getsize(file)
    
#-----------------------------------------------------------------------------
#  
#-----------------------------------------------------------------------------
def AlignFileSize(inFile, outFile, align=512, padData=0):

    file = open(inFile, "rb")
    data = bytearray(file.read())
    file.close()

    file = open(outFile, "wb")
    file.write(data)

    padSize = (align - len(data)%align)%align
    
    file.write(chr(padData)*padSize)
        
    file.close()
    
    return 0

#-----------------------------------------------------------------------------
#  
#-----------------------------------------------------------------------------
def ByteCheckSumPadding(barrary, align=32, padData=0xFF):

    inSize = len(barrary)
    outSize = inSize + 32
    padSize = (align - 1 - outSize%align)%align
    
    for i in range(0, padSize):
        barrary.append(padData)

    block_size = len(barrary)
    chkSum = sum(barrary)
    barrary.append(chkSum%256)

    return padSize    

#-----------------------------------------------------------------------------
#  Convert intel hexa formatted file to
#-----------------------------------------------------------------------------
def ConvertIntelHexToBinary(inFile, cpuId, fileOffset, destAddress, align, padData, bootfw=0):

    file = open(inFile, "r")
    text = file.readlines()
    file.close()

    codeBlock = bytearray()
    codeAll   = bytearray()

    address32 = 0
    lastAddress = 0

    if bootfw == 0:
        blockStartAddress = 0
    else:
        blockStartAddress = destAddress
    
    blockSize = 0
    blockIndex = 0

    for line in text:
        if line[0]!=':':
            sys.stderr.write("Invalid Start code ':' %s\n" %(line))
            sys.exit(1)
            
        byteCount  = int(line[1:3],16)
        address16  = int(line[3:7],16)
        recordType = int(line[7:9],16)
        checksum   = int(line[-2:],16)

        if recordType == 0:
            currAddress = address16 + address32
            
            if (currAddress != lastAddress):    # new code block, non-continuous
                if (blockSize != len(codeBlock)):
                    sys.stderr.write("ERROR : blockSize[%X] is not equal codeBlock[%X].\n" %(blockSize, len(codeBlock)))
                    sys.exit(1)
                if (blockSize != 0):
                    print("FW block%2u [%08X..%08X] %8u bytes" %(blockIndex, blockStartAddress, blockStartAddress+blockSize-1, blockSize))
                    blockIndex = blockIndex+1
                    padSize = ByteCheckSumPadding(codeBlock, align, padData)
                    header = MakeDownHeader(cpuId, blockSize + padSize, fileOffset, blockStartAddress, blockStartAddress, bootfw)
                    
                    codeAll = codeAll + header + codeBlock
                    codeBlock = bytearray()

                blockSize = 0
                blockStartAddress = currAddress
                
            for index in range(byteCount):
                strIndex = index*2 + 9
                byteData = int(line[strIndex:strIndex+2],16)
                currAddress = currAddress + 1
                codeBlock.append(byteData)
                blockSize = blockSize + 1

            lastAddress = currAddress
        elif recordType == 1:   # End Of File record
            break
        elif recordType == 4:   # Extended Linear Address Record
            address32 = int(line[9:13],16) * 0x10000
        elif recordType == 5:   # Start Linear Address Record
            address32 = int(line[9:17],16)
        else:
            print("Warning - not supported record type %d" %(recordType))

    if (blockSize != 0):
        print("FW block%2u [%08X..%08X] %8u bytes" %(blockIndex, blockStartAddress, blockStartAddress+blockSize-1, blockSize))
        
        if(bootfw == 2):        # For building PCIe download firmware
            codeAll = codeAll + codeBlock
        else:
            padSize = ByteCheckSumPadding(codeBlock, align, padData)        
            header = MakeDownHeader(cpuId, blockSize + padSize, fileOffset, blockStartAddress, blockStartAddress, bootfw)
            codeAll = codeAll + header + codeBlock
            
    return codeAll

#-----------------------------------------------------------------------------
#  
#-----------------------------------------------------------------------------
def MakeDownFile(inFile, outFile, cpuId=0, srcAddress=0x20,destAddress=0, execAddress=0, align=512, padData=0, bootfw=0):

    if fnmatch.fnmatch(inFile, '*.hex'):
        data = ConvertIntelHexToBinary(inFile, cpuId, srcAddress, destAddress, align, padData ,bootfw)
    else:
        file = open(inFile, "rb")
        data = bytearray(file.read())
        file.close()
    
    file = open(outFile, "wb")
    file.write(data)
    file.close()

    outSize = FileSize(outFile)

    #print ("Make UART/SFlash Downloadable file")
    #print ("Input  File = %s, size=%d" %(inFile, inSize))
    #print ("Output File = %s, size=%d" %(outFile, outSize))
    print ("CPU=%d srcAddr=%08Xh destAddr=%08Xh execAddr=%08Xh" %(cpuId, srcAddress, destAddress, execAddress))
    #print ("(header(32)+src(%d)+pad(%d)+chksum(1))" %(inSize, padSize))
    print("===============================")
    
    return 0

#-----------------------------------------------------------------------------
#  
#-----------------------------------------------------------------------------
def MakePcieDownFile(inFile, outFile, cpuId=0, srcAddress=0x20, destAddress=0, execAddress=0, align=512, padData=0, bootfw=0):

    if fnmatch.fnmatch(inFile, '*.hex'):
        data = ConvertIntelHexToBinary(inFile, cpuId, srcAddress, destAddress, align, padData ,bootfw)
    else:
        file = open(inFile, "rb")
        data = bytearray(file.read())
        file.close()
    
    inSize = len(data)
    outSize = inSize + 32
    padSize = (align - 4 - outSize%align)%align
    
    header = MakePcieDownHeader(cpuId, inSize+padSize+4, srcAddress, destAddress, execAddress, bootfw)

    for i in range(0, padSize):
        data.append(padData)

    chkSum32 = CheckSumU32(data, 0, len(data))
    
    data.append((chkSum32>>0)%256)
    data.append((chkSum32>>8)%256)
    data.append((chkSum32>>16)%256)
    data.append((chkSum32>>24)%256)
    
    file = open(outFile, "wb")
    file.write(header)
    file.write(data)
    file.close()

    outSize = FileSize(outFile)

    print ("Make PCIe Downloadable file")
    print ("Input  File = %s, size=%d" %(inFile, inSize))
    print ("Output File = %s, size=%d" %(outFile, outSize))
    print ("CPU=%d srcAddr=%08Xh destAddr=%08Xh execAddr=%08Xh" %(cpuId, srcAddress, destAddress, execAddress))
    #print ("(header(32)+src(%d)+pad(%d)+chksum(1))" %(inSize, padSize))
    
    return 0

#-----------------------------------------------------------------------------
#  
#-----------------------------------------------------------------------------
def Usage():
    print ("Marvell download file generation tool")
    print ("Usage: mvtool [options] input_file output_file")
    print ("Options")
    print ("-h             help")
    print ("-c cpuId       downlad target cpu id (0=cpu0, 1=cpu1)")
    print ("-s address     source address (default=0x20)")
    print ("-d address     destination address to be downloaded(default=0)")
    print ("-e address     execution address (default=0)")
    print ("-a size        align size of output file(default=1)")
    print ("-p data        padding data pattern (default=00h)")
    print ("-b bootfw      BOOT FW or not (default=00h)")
    print
    return

#=============================================================================
#  
#=============================================================================
def py_main(argc, argv):
    try:
        opts, args = getopt.getopt(argv[1:], "hc:s:d:e:a:p:b:", \
                     ["help", "cpu=", "source=", "dest=", "execute=", "align=", "pad=", "bootfw="])
    except (getopt.GetoptError, err):
        print (str(err))
        Usage()
        sys.exit(1)

    cpuId = 0
    srcAddress = 0x20
    destAddress = 0
    execAddress = 0
    align = 1
    padData = 0xFF
    bootfw = 0
    pcieDown = 0
    
    for o, a in opts:
        if o == "-h":
            Usage()
            sys.exit(1)
        elif o == "-c": 
            cpuId = eval(a)
        elif o == "-s": 
            srcAddress = eval(a)
        elif o == "-d": 
            destAddress = eval(a)
        elif o == "-e": 
            execAddress = eval(a)
        elif o == "-a": 
            align = eval(a)
        elif o == "-p": 
            padData = eval(a)
        elif o == "-b":
            bootfw = eval(a)
            if(bootfw == 2):
                pcieDown = 1
        else:
            Usage()
            sys.exit(1)

    if len(args)!=2:
        Usage()
        sys.exit(1)
        
    if (pcieDown == 1):
        MakePcieDownFile(args[0], args[1], cpuId, srcAddress, destAddress, execAddress, 512, padData, bootfw)
    else:
        MakeDownFile(args[0], args[1], cpuId, srcAddress, destAddress, execAddress, align, padData, bootfw)
    

#=============================================================================
#  
#=============================================================================
if __name__ == "__main__":
    py_main(len(sys.argv)-1, sys.argv)        

