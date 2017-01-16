import os, sys, binascii
import fnmatch
import platform
import re
import shutil
import multiprocessing
import datetime
import subprocess
import signal
from operator import itemgetter
from struct import *
sys.path.append('Build')

#-----------------------------------------------------------------------------
#  Build Firmware
#-----------------------------------------------------------------------------
#  > python build.py option
#
#    OPTION: plesae refer to Usage()
#
#-----------------------------------------------------------------------------
class BuildFirmwareClass():
    def __init__(self, parent = None):
        self.model_label      = ""
        self.bin_prefix       = "Eldora"
        self.bin_postfix      = ""
        self.ssdBinSize       =  96
        self.maxBootCodeSize  =  64 * 1024
        self.maxMainCodeSize  = 416 * 1024
        self.bootCodeSize     = 0
        self.mainCodeSize     = 0
        self.mainCodeTagSize  = 32;
        self.opRomCodeSize    = 56 * 1024;
        self.bootCodeOffset   = 0
        self.main1CodeOffset  = self.bootCodeOffset + self.maxBootCodeSize
        self.main2CodeOffset  = self.bootCodeOffset + self.maxBootCodeSize + self.maxMainCodeSize
        self.mainBkCodeOffset = 0xF6000;
        self.opRomCodeOffset  = 0xE0000;

        self.revision         = '00000000'
        self.out_dir          = '_out'
        self.bin_dir          = 'Bin'
        self.oprom_dir        = 'Bin/OPROM/'
        self.hidden_option    = None
        self.backAdapter      = None

        self.model_hash_cx2  = [{'Label':'CX2-8B',              'Check':True,  'Customer':'DQ',        'DRAM':'LPDDR3',  'DRAM_SIZE':'512',  'TCG':'NONE',   'OPROM' :'NONE',   'Vendor':'TSB', 'Nand':'MLC',     'FWType':'FW_NORMAL', 'FWSign':'NONE', 'FWName':FWName.Generation['CX2']+FWName.FormFactor['M.2_2280SS']+FWName.Capacity['Auto']+FWName.Customer['DQ']       +'01',  'FWRevSub':'0', 'ModelName':ModelName.Prefix['LITEON'] +'CX2-'+ModelName.FormFactor['M.2_2280SS']+ModelName.Nand['TSB_15NM_CMLC']+ModelName.Capacity['Auto']+ModelName.Postfix['NONE'], 'OP':'7',  'Capacity':'128 256 512' },
                                {'Label':'CX2-GB',              'Check':False, 'Customer':'DQ',        'DRAM':'LPDDR3',  'DRAM_SIZE':'1024', 'TCG':'NONE',   'OPROM' :'NONE',   'Vendor':'TSB', 'Nand':'MLC',     'FWType':'FW_NORMAL', 'FWSign':'NONE', 'FWName':FWName.Generation['CX2']+FWName.FormFactor['M.2_2280']  +FWName.Capacity['Auto']+FWName.Customer['DQ']       +'01',  'FWRevSub':'0', 'ModelName':ModelName.Prefix['LITEON'] +'CX2-'+ModelName.FormFactor['M.2_2280']  +ModelName.Nand['TSB_15NM_CMLC']+ModelName.Capacity['Auto']+ModelName.Postfix['NONE'], 'OP':'7',  'Capacity':'1024'    },
                                {'Label':'CX2-8B_Std_Dell',     'Check':False, 'Customer':'DELL',      'DRAM':'LPDDR3',  'DRAM_SIZE':'512',  'TCG':'NONE',   'OPROM' :'NONE',   'Vendor':'TSB', 'Nand':'MLC',     'FWType':'FW_NORMAL', 'FWSign':'NONE', 'FWName':FWName.Generation['CX2']+FWName.FormFactor['M.2_2280SS']+FWName.Capacity['Auto']+FWName.Customer['DQ']       +'01',  'FWRevSub':'0', 'ModelName':ModelName.Prefix['LITEON'] +'CX2-'+ModelName.FormFactor['M.2_2280SS']+ModelName.Nand['TSB_15NM_CMLC']+ModelName.Capacity['Auto']+ModelName.Postfix['NONE'], 'OP':'7',  'Capacity':'128 256 512' },
                                {'Label':'CX2-GB_Std_Dell',     'Check':False, 'Customer':'DELL',      'DRAM':'LPDDR3',  'DRAM_SIZE':'1024', 'TCG':'NONE',   'OPROM' :'NONE',   'Vendor':'TSB', 'Nand':'MLC',     'FWType':'FW_NORMAL', 'FWSign':'NONE', 'FWName':FWName.Generation['CX2']+FWName.FormFactor['M.2_2280']  +FWName.Capacity['Auto']+FWName.Customer['DQ']       +'01',  'FWRevSub':'0', 'ModelName':ModelName.Prefix['LITEON'] +'CX2-'+ModelName.FormFactor['M.2_2280']  +ModelName.Nand['TSB_15NM_CMLC']+ModelName.Capacity['Auto']+ModelName.Postfix['NONE'], 'OP':'7',  'Capacity':'1024'    },
                                {'Label':'CX2-8B_Dell',         'Check':False, 'Customer':'DELL',      'DRAM':'LPDDR3',  'DRAM_SIZE':'512',  'TCG':'NONE',   'OPROM' :'NONE',   'Vendor':'TSB', 'Nand':'MLC',     'FWType':'FW_NORMAL', 'FWSign':'NONE', 'FWName':FWName.Generation['CX2']+FWName.FormFactor['M.2_2280SS']+FWName.Capacity['Auto']+FWName.Customer['DELL']     +'01',  'FWRevSub':'0', 'ModelName':ModelName.Prefix['NONE']   +'CX2-'+ModelName.FormFactor['M.2_2280SS']+ModelName.Nand['TSB_15NM_CMLC']+ModelName.Capacity['Auto']+ModelName.Postfix['DELL'], 'OP':'7',  'Capacity':'128 256 512' },
                                {'Label':'CX2-GB_Dell',         'Check':False, 'Customer':'DELL',      'DRAM':'LPDDR3',  'DRAM_SIZE':'1024', 'TCG':'NONE',   'OPROM' :'NONE',   'Vendor':'TSB', 'Nand':'MLC',     'FWType':'FW_NORMAL', 'FWSign':'NONE', 'FWName':FWName.Generation['CX2']+FWName.FormFactor['M.2_2280']  +FWName.Capacity['Auto']+FWName.Customer['DELL']     +'01',  'FWRevSub':'0', 'ModelName':ModelName.Prefix['NONE']   +'CX2-'+ModelName.FormFactor['M.2_2280']  +ModelName.Nand['TSB_15NM_CMLC']+ModelName.Capacity['Auto']+ModelName.Postfix['DELL'], 'OP':'7',  'Capacity':'1024'    },
                                {'Label':'CX2-8B_HP',           'Check':False, 'Customer':'HP',        'DRAM':'LPDDR3',  'DRAM_SIZE':'512',  'TCG':'NONE',   'OPROM' :'NONE',   'Vendor':'TSB', 'Nand':'MLC',     'FWType':'FW_NORMAL', 'FWSign':'NONE', 'FWName':FWName.Generation['CX2']+FWName.FormFactor['M.2_2280SS']+FWName.Capacity['Auto']+FWName.Customer['HP']       +'01',  'FWRevSub':'0', 'ModelName':ModelName.Prefix['LITEON'] +'CX2-'+ModelName.FormFactor['M.2_2280SS']+ModelName.Nand['TSB_15NM_CMLC']+ModelName.Capacity['Auto']+ModelName.Postfix['HP']  , 'OP':'7',  'Capacity':'128 256 512' },
                                {'Label':'CX2-GB_HP',           'Check':False, 'Customer':'HP',        'DRAM':'LPDDR3',  'DRAM_SIZE':'1024', 'TCG':'NONE',   'OPROM' :'NONE',   'Vendor':'TSB', 'Nand':'MLC',     'FWType':'FW_NORMAL', 'FWSign':'NONE', 'FWName':FWName.Generation['CX2']+FWName.FormFactor['M.2_2280']  +FWName.Capacity['Auto']+FWName.Customer['HP']       +'01',  'FWRevSub':'0', 'ModelName':ModelName.Prefix['LITEON'] +'CX2-'+ModelName.FormFactor['M.2_2280']  +ModelName.Nand['TSB_15NM_CMLC']+ModelName.Capacity['Auto']+ModelName.Postfix['HP']  , 'OP':'7',  'Capacity':'1024'    },
                                {'Label':'Aging_CX2-8B',        'Check':False, 'Customer':'AG',        'DRAM':'LPDDR3',  'DRAM_SIZE':'512',  'TCG':'NONE',   'OPROM' :'NONE',   'Vendor':'TSB', 'Nand':'MLC',     'FWType':'FW_NORMAL', 'FWSign':'NONE', 'FWName':FWName.Generation['CX2']+FWName.FormFactor['M.2_2280SS']+FWName.Capacity['Auto']+FWName.Customer['AG']       +'01',  'FWRevSub':'0', 'ModelName':ModelName.Prefix['LITEON'] +'CX2-'+ModelName.FormFactor['M.2_2280SS']+ModelName.Nand['TSB_15NM_CMLC']+ModelName.Capacity['Auto']+ModelName.Postfix['NONE'], 'OP':'7',  'Capacity':'128 256 512' },
                                {'Label':'Aging_CX2-GB',        'Check':False, 'Customer':'AG',        'DRAM':'LPDDR3',  'DRAM_SIZE':'1024', 'TCG':'NONE',   'OPROM' :'NONE',   'Vendor':'TSB', 'Nand':'MLC',     'FWType':'FW_AGING',  'FWSign':'NONE', 'FWName':FWName.Generation['CX2']+FWName.FormFactor['M.2_2280']  +FWName.Capacity['Auto']+FWName.Customer['AG']       +'01',  'FWRevSub':'0', 'ModelName':ModelName.Prefix['LITEON'] +'CX2-'+ModelName.FormFactor['M.2_2280']  +ModelName.Nand['TSB_15NM_CMLC']+ModelName.Capacity['Auto']+ModelName.Postfix['NONE'], 'OP':'7',  'Capacity':'1024'        }]

        self.model_hash_ep2  = [{'Label':'EP2-KB',              'Check':True,  'Customer':'DQ',        'DRAM':'DDR3',    'DRAM_SIZE':'1024', 'TCG':'NONE',   'OPROM' :'NONE',   'Vendor':'TSB', 'Nand':'MLC',     'FWType':'FW_NORMAL', 'FWSign':'NONE', 'FWName':FWName.Generation['EP2']+FWName.FormFactor['M.2_22110'] +FWName.Capacity['Auto']+FWName.Customer['DQ']       +'01',  'FWRevSub':'0', 'ModelName':ModelName.Prefix['LITEON'] +'EP2-'+ModelName.FormFactor['M.2_22110'] +ModelName.Nand['TSB_15NM_CMLC']+ModelName.Capacity['Auto']+ModelName.Postfix['NONE'], 'OP':'14', 'Capacity':'240 480 960' },
                                {'Label':'EP2-KB_Std_Microsoft','Check':True,  'Customer':'MICROSOFT', 'DRAM':'DDR3',    'DRAM_SIZE':'1024', 'TCG':'EDRV',   'OPROM' :'NONE',   'Vendor':'TSB', 'Nand':'MLC',     'FWType':'FW_NORMAL', 'FWSign':'NONE', 'FWName':FWName.Generation['EP2']+FWName.FormFactor['M.2_22110'] +FWName.Capacity['Auto']+FWName.Customer['DQ']       +'01',  'FWRevSub':'0', 'ModelName':ModelName.Prefix['LITEON'] +'EP2-'+ModelName.FormFactor['M.2_22110'] +ModelName.Nand['TSB_15NM_CMLC']+ModelName.Capacity['Auto']+ModelName.Postfix['NONE'], 'OP':'14', 'Capacity':'240 480 960' },
                                {'Label':'EP2-KB_Microsoft',    'Check':False, 'Customer':'MICROSOFT', 'DRAM':'DDR3',    'DRAM_SIZE':'1024', 'TCG':'NONE',   'OPROM' :'NONE',   'Vendor':'TSB', 'Nand':'MLC',     'FWType':'FW_NORMAL', 'FWSign':'NONE', 'FWName':FWName.Generation['EP2']+FWName.FormFactor['M.2_22110'] +FWName.Capacity['Auto']+FWName.Customer['MICROSOFT']+'01',  'FWRevSub':'0', 'ModelName':ModelName.Prefix['LITEON'] +'EP2-'+ModelName.FormFactor['M.2_22110'] +ModelName.Nand['TSB_15NM_CMLC']+ModelName.Capacity['Auto']+ModelName.Postfix['NONE'], 'OP':'14', 'Capacity':'240 480 960' },
                                {'Label':'Aging_EP2-KB',        'Check':True,  'Customer':'AP',        'DRAM':'DDR3',    'DRAM_SIZE':'1024', 'TCG':'NONE',   'OPROM' :'NONE',   'Vendor':'TSB', 'Nand':'MLC',     'FWType':'FW_AGING',  'FWSign':'NONE', 'FWName':FWName.Generation['EP2']+FWName.FormFactor['M.2_22110'] +FWName.Capacity['Auto']+FWName.Customer['AP']       +'01',  'FWRevSub':'0', 'ModelName':ModelName.Prefix['LITEON'] +'EP2-'+ModelName.FormFactor['M.2_22110'] +ModelName.Nand['TSB_15NM_CMLC']+ModelName.Capacity['Auto']+ModelName.Postfix['NONE'], 'OP':'14', 'Capacity':'240 480 960' },
                                {'Label':'EP2-KB_U.2',          'Check':False, 'Customer':'DQ',        'DRAM':'DDR3',    'DRAM_SIZE':'2048', 'TCG':'NONE',   'OPROM' :'NONE',   'Vendor':'TSB', 'Nand':'MLC',     'FWType':'FW_NORMAL', 'FWSign':'NONE', 'FWName':FWName.Generation['EP2']+FWName.FormFactor['U.2']       +FWName.Capacity['Auto']+FWName.Customer['DQ']       +'01',  'FWRevSub':'0', 'ModelName':ModelName.Prefix['LITEON'] +'EP2-'+ModelName.FormFactor['U.2']       +ModelName.Nand['TSB_15NM_CMLC']+ModelName.Capacity['Auto']+ModelName.Postfix['NONE'], 'OP':'14', 'Capacity':'240 480 960 1920'}]

        self.model_hash_ep3  = [{'Label':'EP3-KB',              'Check':True,  'Customer':'DQ',        'DRAM':'DDR3',    'DRAM_SIZE':'2048', 'TCG':'NONE',   'OPROM' :'NONE',   'Vendor':'TSB', 'Nand':'3DTLCG2', 'FWType':'FW_NORMAL', 'FWSign':'NONE', 'FWName':FWName.Generation['EP3']+FWName.FormFactor['M.2_22110'] +FWName.Capacity['Auto']+FWName.Customer['DQ']       +'01',  'FWRevSub':'0', 'ModelName':ModelName.Prefix['LITEON'] +'EP3-'+ModelName.FormFactor['M.2_22110'] +ModelName.Nand['TSB_3D_TLC']   +ModelName.Capacity['Auto']+ModelName.Postfix['NONE'], 'OP':'14', 'Capacity':'960 1920'    },
                                {'Label':'Aging_EP3-KB',        'Check':True,  'Customer':'AP',        'DRAM':'DDR3',    'DRAM_SIZE':'2048', 'TCG':'NONE',   'OPROM' :'NONE',   'Vendor':'TSB', 'Nand':'3DTLCG2', 'FWType':'FW_AGING',  'FWSign':'NONE', 'FWName':FWName.Generation['EP3']+FWName.FormFactor['M.2_22110'] +FWName.Capacity['Auto']+FWName.Customer['AP']       +'01',  'FWRevSub':'0', 'ModelName':ModelName.Prefix['LITEON'] +'EP3-'+ModelName.FormFactor['M.2_22110'] +ModelName.Nand['TSB_3D_TLC']   +ModelName.Capacity['Auto']+ModelName.Postfix['NONE'], 'OP':'14', 'Capacity':'960 1920'    },
                                {'Label':'EP3-KB_DDR4',         'Check':True,  'Customer':'DQ',        'DRAM':'DDR4',    'DRAM_SIZE':'2048', 'TCG':'NONE',   'OPROM' :'NONE',   'Vendor':'TSB', 'Nand':'3DTLCG2', 'FWType':'FW_NORMAL', 'FWSign':'NONE', 'FWName':FWName.Generation['EP3']+FWName.FormFactor['M.2_22110'] +FWName.Capacity['Auto']+FWName.Customer['DQ']       +'01',  'FWRevSub':'0', 'ModelName':ModelName.Prefix['LITEON'] +'EP3-'+ModelName.FormFactor['M.2_22110'] +ModelName.Nand['TSB_3D_TLC']   +ModelName.Capacity['Auto']+ModelName.Postfix['NONE'], 'OP':'14', 'Capacity':'960 1920'    },
                                {'Label':'Aging_EP3-KB_DDR4',   'Check':True,  'Customer':'AP',        'DRAM':'DDR4',    'DRAM_SIZE':'2048', 'TCG':'NONE',   'OPROM' :'NONE',   'Vendor':'TSB', 'Nand':'3DTLCG2', 'FWType':'FW_AGING',  'FWSign':'NONE', 'FWName':FWName.Generation['EP3']+FWName.FormFactor['M.2_22110'] +FWName.Capacity['Auto']+FWName.Customer['AP']       +'01',  'FWRevSub':'0', 'ModelName':ModelName.Prefix['LITEON'] +'EP3-'+ModelName.FormFactor['M.2_22110'] +ModelName.Nand['TSB_3D_TLC']   +ModelName.Capacity['Auto']+ModelName.Postfix['NONE'], 'OP':'14', 'Capacity':'960 1920'    },
                                {'Label':'EP3-KB_U.2',          'Check':False, 'Customer':'DQ',        'DRAM':'DDR3',    'DRAM_SIZE':'2048', 'TCG':'NONE',   'OPROM' :'NONE',   'Vendor':'TSB', 'Nand':'3DTLCG2', 'FWType':'FW_NORMAL', 'FWSign':'NONE', 'FWName':FWName.Generation['EP3']+FWName.FormFactor['U.2']       +FWName.Capacity['Auto']+FWName.Customer['DQ']       +'01',  'FWRevSub':'0', 'ModelName':ModelName.Prefix['LITEON'] +'EP3-'+ModelName.FormFactor['U.2']       +ModelName.Nand['TSB_15NM_CMLC']+ModelName.Capacity['Auto']+ModelName.Postfix['NONE'], 'OP':'14', 'Capacity':'960 1920'    },
                                {'Label':'EP3-KB_DDR4',         'Check':False, 'Customer':'RD',        'DRAM':'DDR4',    'DRAM_SIZE':'2048', 'TCG':'NONE',   'OPROM' :'NONE',   'Vendor':'TSB', 'Nand':'3DTLCG2', 'FWType':'FW_NORMAL', 'FWSign':'NONE', 'FWName':FWName.Generation['EP3']+FWName.FormFactor['M.2_22110'] +FWName.Capacity['Auto']+FWName.Customer['RD']       +'01',  'FWRevSub':'0', 'ModelName':ModelName.Prefix['LITEON'] +'EP3-'+ModelName.FormFactor['M.2_22110'] +ModelName.Nand['TSB_3D_TLC']   +ModelName.Capacity['Auto']+ModelName.Postfix['NONE'], 'OP':'14', 'Capacity':'960 1920'    },
                                {'Label':'EP3-KB_DDR4_OPROM',   'Check':False, 'Customer':'DQ',        'DRAM':'DDR4',    'DRAM_SIZE':'2048', 'TCG':'NONE',   'OPROM' :'EP3',    'Vendor':'TSB', 'Nand':'3DTLCG2', 'FWType':'FW_NORMAL', 'FWSign':'NONE', 'FWName':FWName.Generation['EP3']+FWName.FormFactor['M.2_22110'] +FWName.Capacity['Auto']+FWName.Customer['DQ']       +'01',  'FWRevSub':'0', 'ModelName':ModelName.Prefix['LITEON'] +'EP3-'+ModelName.FormFactor['M.2_22110'] +ModelName.Nand['TSB_3D_TLC']   +ModelName.Capacity['Auto']+ModelName.Postfix['NONE'], 'OP':'14', 'Capacity':'960 1920'    }]

        self.generation_hash = [{'Label':'CX2', 'ModelList':self.model_hash_cx2},
                                {'Label':'EP2', 'ModelList':self.model_hash_ep2},
                                {'Label':'EP3', 'ModelList':self.model_hash_ep3}]

        os.environ["C_CORE"]    = "--cpu=cortex-r5"
        os.environ["_SOC_"]     = "1093"
        os.environ["_SOC_REV_"] = "0x0210"
        os.environ["_SHARE_"]   = "RTOS"
        os.environ["_HOST_"]    = "NVME"
        os.environ["_MEDIA_"]   = "NAND"
        os.environ["_OUT_"]     = self.out_dir
        os.environ["_MUTE_"]    = "DISABLE"

        signal.signal(signal.SIGINT, self.SigCtrlCHandler)

    def ReleaseBuild(self):
        BuildFirmwareObj.CheckHostId('modify')

        if self.hidden_option == "quick" and os.path.isdir(self.out_dir) and os.path.isfile(self.out_dir+'/config'):
            file = open(self.out_dir+'/config', 'r')
            gen_index   = int(file.readline())
            model_index = int(file.readline())
            file.close()

            generation = self.generation_hash[gen_index]
            model      = generation['ModelList'][model_index]
        else:
            self.CleanAll()
            os.mkdir(self.out_dir)

            generation = self.SelectGeneration(self.generation_hash)
            model = self.SelectModel(generation['ModelList'])

            file = open(self.out_dir+'/config', 'w+')
            file.write('%d\n' % self.generation_hash.index(generation))
            file.write('%d\n' % generation['ModelList'].index(model))
            file.write('%s\n' % model['Label'])
            file.close()

        os.environ["_GENERATION_"] = generation['Label']
        self.BuildFirmware(model)

        BuildFirmwareObj.CheckHostId('restore')

    def TestBuild(self):
        BuildFirmwareObj.CheckHostId('modify')

        os.environ["_MUTE_"] = "ENABLE"

        for generation in self.generation_hash:
            os.environ["_GENERATION_"] = generation['Label']

            for model in generation['ModelList']:
                if model['Check']:
                    self.CleanAll()
                    os.mkdir(self.out_dir)

                    print('==========================================================================')
                    Color.prGreen('= Test Build Model: %s' % model['Label'])
                    print('==========================================================================')

                    self.BuildFirmware(model)

        BuildFirmwareObj.CheckHostId('restore')

    def BuildFirmware(self, model):
        os.environ["_FW_TYPE_"]     = model['FWType']
        os.environ["_OP_"]          = model['OP']
        os.environ["_FW_REV_"]      = model['FWName']
        os.environ["_FW_REV_SUB_"]  = model['FWRevSub']
        os.environ["_MODEL_NAME_"]  = model['ModelName']
        os.environ["_CUSTOMER_"]    = model['Customer']
        os.environ["_DRAM_"]        = model['DRAM']
        os.environ["_DRAM_SIZE_"]   = model['DRAM_SIZE']
        os.environ["_VENDOR_"]      = model['Vendor']
        os.environ["_NAND_"]        = model['Nand']
        os.environ["_TCG_"]         = model['TCG']
        os.environ["_OPROM_"]       = model['OPROM']
        self.model_label            = model['Label']

        if self.hidden_option == "ramdisk":
            self.bin_postfix = "RamDisk"
            os.environ["_TCG_"]       = "NONE"
            os.environ["_MEDIA_"]     = "DRAM"
        elif self.hidden_option == "uart":
            self.bin_postfix = "Uart"
            os.environ["_TCG_"]       = "NONE"
            os.environ["_MEDIA_"]     = "DRAM"
            os.environ["_FW_TYPE_"]   = "FW_DNLD"
        elif self.hidden_option == "pcie":
            self.bin_postfix = "PCIe"
            os.environ["_TCG_"]       = "NONE"
            os.environ["_MEDIA_"]     = "DRAM"
            os.environ["_FW_TYPE_"]   = "FW_DNLD"

        file = open('Build/'+os.environ["_SOC_"]+'/scf_inc.h', 'w+')
        if os.environ["_FW_TYPE_"] == "FW_DNLD":
            file.writelines('#define SCF_LINEAR_REGION\n\n')
        else:
            file.writelines('\n\n')
        file.close()

        self.QuerySourceRevision()

        retValue = self.FindBootCodeAddress()
        if retValue == None:  BuildFail(ErrorCode.EcBootCodeAddrNotFound)

        retValue = self.CompileAndLink()
        if retValue != 0:   self.BuildFail(ErrorCode.EcCompileError)

        retValue = self.MakeImage(model)
        if retValue != 0:   self.BuildFail(ErrorCode.EcCodeSizeOverflow)

        print('--------------------------------------------------------------------------')
        print('WARNING! Stack Overflow Risk!')
        print('--------------------------------------------------------------------------')
        self.CheckStackSize(self.out_dir+'/bin/boot.htm', self.out_dir+'/bin/boot.stack', 150, 1024)
        self.CheckStackSize(self.out_dir+'/bin/cpu0.htm', self.out_dir+'/bin/cpu0.stack', 150, 1024)
        self.CheckStackSize(self.out_dir+'/bin/cpu1.htm', self.out_dir+'/bin/cpu1.stack', 150, 1024)
        self.CheckStackSize(self.out_dir+'/bin/cpu2.htm', self.out_dir+'/bin/cpu2.stack', 150, 1024)

        print('--------------------------------------------------------------------------')
        print('Object contain RW data that might be referred by PreOneTimeInit()')
        print('--------------------------------------------------------------------------')
        self.CheckObjectRef(self.out_dir+'/bin/boot.lst')
        self.CheckObjectRef(self.out_dir+'/bin/cpu0.lst')
        self.CheckObjectRef(self.out_dir+'/bin/cpu1.lst')
        self.CheckObjectRef(self.out_dir+'/bin/cpu2.lst')

        print('--------------------------------------------------------------------------')
        print('Summery [%s %s]' % (model["Label"], model['FWName']))
        print('--------------------------------------------------------------------------')
        self.PrintSummery(self.out_dir+'/bin/boot.lst')
        self.PrintSummery(self.out_dir+'/bin/cpu0.lst')
        self.PrintSummery(self.out_dir+'/bin/cpu1.lst')
        self.PrintSummery(self.out_dir+'/bin/cpu2.lst')

    def CompileAndLink(self):
        if (self.hidden_option != "uart" and self.hidden_option != "pcie"):
            retValue = os.system('make _CPU_=0 _CODE_TYPE_=CODE_BOOT -f Build/makefile.mak -j'+str(multiprocessing.cpu_count()))
            if retValue != 0:   return -1

        retValue = os.system('make _CPU_=0 _CODE_TYPE_=CODE_MAIN -f Build/makefile.mak -j'+str(multiprocessing.cpu_count()))
        if retValue != 0:   return -1

        if (self.hidden_option != "ramdisk" and self.hidden_option != "uart" and self.hidden_option != "pcie"):
            retValue = os.system('make _CPU_=1 _CODE_TYPE_=CODE_MAIN -f Build/makefile.mak -j'+str(multiprocessing.cpu_count()))
            if retValue != 0:   return -1

            retValue = os.system('make _CPU_=2 _CODE_TYPE_=CODE_MAIN -f Build/makefile.mak -j'+str(multiprocessing.cpu_count()))
            if retValue != 0:   return -1

        return 0

    def MakeImage(self, model):
        print('--------------------------------------------------------------------------')
        print('Create BOOT and MAIN Firmware')
        print('--------------------------------------------------------------------------')

        if(self.hidden_option == "uart" or self.hidden_option == "pcie"):
            if os.path.isfile(self.out_dir+'/bin/cpu0.hex'):
                if (self.hidden_option == "uart"):
                    os.system('python Build/mvtool.py -c0 -a32 -b1 -s32 _out/bin/cpu0.hex _out/bin/cpu0.dfw')
                elif (self.hidden_option == "pcie"):
                    os.system('python Build/mvtool.py -c0 -a32 -b2 -s32 _out/bin/cpu0.hex _out/bin/cpu0.dfw')
        else:
            os.system('python Build/mvtool.py -c0 -a4096 -b1 -s32 -d'+self.bootCodeAddr+' -e'+self.bootCodeAddr+' _out/bin/boot.hex _out/bin/boot.dfw')
            retValue = self.CheckBinarySize(self.out_dir+'/bin/boot.dfw', self.maxBootCodeSize, "Boot Code")
            if retValue != 0:   return -1

            if os.path.isfile(self.out_dir+'/bin/cpu0.hex'):
                os.system('python Build/mvtool.py -c0 -a32 -b0 -s32 _out/bin/cpu0.hex _out/bin/cpu0.dfw')

        if os.path.isfile(self.out_dir+'/bin/cpu1.hex'):
            os.system('python Build/mvtool.py -c1 -a32 -b0 -s32 _out/bin/cpu1.hex _out/bin/cpu1.dfw')
        if os.path.isfile(self.out_dir+'/bin/cpu2.hex'):
            os.system('python Build/mvtool.py -c2 -a32 -b0 -s32 _out/bin/cpu2.hex _out/bin/cpu2.dfw')

        os.system('python Build/mvcopy.py _out/bin/cpu0.dfw _out/bin/cpu1.dfw _out/bin/cpu2.dfw _out/bin/firmware.dfw')

        retValue = self.CheckBinarySize(self.out_dir+'/bin/firmware.dfw', self.maxMainCodeSize, "Main Code")
        if retValue != 0:   return -1

        if (self.hidden_option == "uart" or self.hidden_option == "pcie"):
            os.system('python Build/mvcopy.py _out/bin/firmware.dfw _out/bin/FW_IMG.dfw')
        else:
            self.DataPadding('_out/bin/firmware.dfw',  '_out/bin/firmware.dfw', self.mainCodeTagSize, 0, "tag")
            os.system('python Build/mvcopy.py _out/bin/boot.dfw _out/bin/firmware.dfw _out/bin/FW_IMG.dfw')
            self.QueryFrimwareVersion(self.out_dir+'/bin/FW_IMG.dfw', self.out_dir+'/bin/FW_IMG.dfw', model)
            self.AppendSsdBin(self.out_dir+'/bin/FW_IMG.dfw', model)
            if (model['OPROM'] != 'NONE'):
                self.CheckBinarySize(self.oprom_dir + os.environ["_GENERATION_"] +'_OPROM.bin', self.maxMainCodeSize, "OPROM Code")
                self.DataPadding(self.out_dir+'/bin/FW_IMG.dfw', self.out_dir+'/bin/FW_IMG.dfw', 0, 0, "OPROM")
            self.AppendFirmwareHeader(self.out_dir+'/bin/FW_IMG.dfw', model)
            self.DataPadding(self.out_dir+'/bin/FW_IMG.dfw', self.out_dir+'/bin/FW_IMG.dfw', 4096, (self.maxBootCodeSize + self.maxMainCodeSize) * 1024 - 4, "crc")

        os.system('python Build/mvcopy.py _out/bin/FW_IMG.dfw Bin/'+self.bin_prefix+'.bin')
        os.system('python Build/mvcopy.py _out/bin/FW_IMG.dfw Bin/'+self.bin_prefix+'_'+self.model_label+'_'+self.bin_postfix+'.bin')

        return 0

    def SelectGeneration(self, generation_hash):
        print('==== Generation List ====')
        print('Index  Generation')
        print('-------------------------')

        for list in generation_hash:
            print ('%2d)    %-12s' % (generation_hash.index(list), list['Label']))

        if (sys.version_info < (3, 0)):
            index = raw_input('>[0]') or "0"
        else:
            index = input('>[0]') or "0"

        if index.isdigit() == False or int(index) >= len(generation_hash):
            self.BuildFail(ErrorCode.EcGenerationError)

        return generation_hash[int(index)]

    def SelectModel(self, model_hash):
        print('=================== Model List ==============================================')
        print('Index  %-21s %-11s %-9s %-7s %-6s %-9s' % ('Model', 'Customer', 'DRAM', 'TCG', 'OPROM', 'FW'))
        print('-----------------------------------------------------------------------------')

        for list in model_hash:
            print ('%2d)    %-21s %-11s %-9s %-7s %-6s %-9s' % (model_hash.index(list), list['Label'], list['Customer'], list['DRAM'], list['TCG'],  list['OPROM'], list['FWName']))

        if (sys.version_info < (3, 0)):
            index = raw_input('>[0]') or "0"
        else:
            index = input('>[0]') or "0"

        if index.isdigit() == False or int(index) >= len(model_hash):
            self.BuildFail(ErrorCode.EcModelError)

        return model_hash[int(index)]

    def QuerySourceRevision(self):
        if platform.system() == 'Windows':
            if os.path.isdir('./.git'):
                file = open('.git/HEAD', 'r')
                line = file.readline().rstrip('\n')
                match = re.split('ref: refs/heads/', line)
                file.close()

                file = open('.git/refs/heads/'+match[1], 'r')
                self.revision = file.read(8).upper()
                file.close()

        elif platform.system() == 'Linux':
            if os.path.isdir('./.git'):
                self.revision = (subprocess.check_output( 'git rev-parse HEAD', shell = True ).rstrip())[0:8]

        print ('Source Revision:'+str(self.revision))

        os.environ['_SRC_REV_'] = str(self.revision)

    def QueryFrimwareVersion(self, src, dest, model):
        file = open(src, "rb")
        data = bytearray(file.read())
        file.close()

        bootHeaderOffset = 32
        bootFWVerOffert  = self.bootCodeSize - 16

        data[bootFWVerOffert:bootFWVerOffert + 7] = map(ord, model['FWName'])

        chkSum = sum(data[bootHeaderOffset:self.bootCodeSize - 1])
        data[self.bootCodeSize - 1] = chkSum % 256

        file = open(dest, "wb")
        file.write(data)
        file.close()

        print('Firmware Version: %s' % model['FWName'])

    def FindBootCodeAddress(self):
        pattern = re.compile(r"^#define\s+BOOT_ITCM_ADDR\s+(\w+)")
        for i, line in enumerate(open('Share/Hal/'+os.environ["_SOC_"]+'/MemoryMap.h', 'r')):
            match = re.match(pattern, line);
            if match:
                self.bootCodeAddr = match.group(1)
                break

        os.environ['_BOOT_ADDR_'] = self.bootCodeAddr

        return self.bootCodeAddr

    def AppendFirmwareHeader(self, src, model):
        section_hash     = [{'Tag':'BOOT', 'Offset': self.bootCodeOffset,   'Size': self.bootCodeSize,                                           'Update':1},
                            {'Tag':'FW01', 'Offset': self.main1CodeOffset,  'Size': self.mainCodeSize + self.mainCodeTagSize + self.ssdBinSize,  'Update':0},
                            {'Tag':'FW02', 'Offset': self.main2CodeOffset,  'Size': self.mainCodeSize + self.mainCodeTagSize + self.ssdBinSize,  'Update':0},
                            {'Tag':'FWBK', 'Offset': self.mainBkCodeOffset, 'Size': self.mainCodeSize + self.mainCodeTagSize + self.ssdBinSize,  'Update':1},
                            {'Tag':'OPRM', 'Offset': self.opRomCodeOffset,  'Size': self.opRomCodeSize,                                          'Update':1 if model['OPROM'] != 'NONE' else 0}]

        cssOffset = 1024

        headerData = bytearray(4096)

        if model['FWSign'] != 'NONE':
            headerData[0:1024] = [0xFF] * 1024

            headerData[ 0: 4] = [0x6E, 0x76, 0x6D, 0x00]         #Module Type, 'nvm'
            headerData[12:16] = [0x45, 0x50, 0x33, 0x00]         #Module ID, 'EP3'

            headerData[16:20] = [0x4C, 0x54, 0x4E, 0x00]         #Module Vendor, 'LTN'
            headerData[20:24] = [0x20, 0x16, 0xFF, 0xFF]         #Date
            headerData[24:28] = [0xFF, 0xFF, 0xFF, 0xFF]         #Module Size

            import FwSign
            FwSign.Sign("_out/bin/FW_IMG.dfw", headerData, model['FWSign'])

        # Section Summery
        offset = cssOffset

        ### Section Header ###
        # Tag
        headerData[offset      : offset +  4] = map(ord, 'FWHD')
        # Version
        headerData[offset + 4  : offset +  6] = bytearray(unpack("2B", pack("H", 1)))
        # section count
        headerData[offset + 6  : offset +  8] = bytearray(unpack("2B", pack("H", len(section_hash))))
        # Reserved
        headerData[offset + 12 : offset + 16] = map(ord, 'RSVD')
        # Find and apeend vendor, nand nubmer
        if os.path.isfile('Build/BuildCommon.h'):
            file = open('Build/BuildCommon.h', 'r')

            lines = file.read().splitlines()
            file.close()

            for line in lines:
                match = re.match(r"^#define DRAM_"+model['DRAM']+"\s+(\w+)", line)
                if match:
                    headerData[offset + 16] = int(match.group(1), 16)

                match = re.match(r"^#define VENDOR_"+model['Vendor']+"\s+(\d+)", line)
                if match:
                    headerData[offset + 17] = int(match.group(1))

                match = re.match(r"^#define NAND_"+model['Nand']+"\s+(\d+)", line)
                if match:
                    headerData[offset + 18]  = int(match.group(1))
        else:
            self.BuildFail(ErrorCode.EcBuildCommonNotFound)

        # Append each section info
        for section in section_hash:
            offset = cssOffset + 16 * (section_hash.index(section) + 2)
            headerData[offset     : offset +  4] = map(ord, section['Tag'])
            headerData[offset + 4 : offset +  8] = bytearray(unpack("4B", pack("I",section['Offset'])))
            headerData[offset + 8 : offset + 12] = bytearray(unpack("4B", pack("I",section['Size'])))
            headerData[offset + 12: offset + 16] = bytearray(unpack("4B", pack("I",section['Update'])))

        file = open(src, "rb+")
        fwData = bytearray(file.read())

        headerData += fwData

        file.seek(0)
        file.write(headerData)
        file.close()

    def AppendSsdBin(self, src ,model):
        month_str = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec']

        SsdBinFmt       = { 'Tag'           :'SsdBinTagNVMe',
                            'SubRevision'   :model['FWRevSub'],
                            'ModelName'     :model['ModelName'],
                            'FWRevision'    :model['FWName'].replace('#', '$'),
                            'OEMFWRevision' :model['FWName'].replace('#', '$')}

        SsdBinOffset    = { 'Tag'           :0,
                            'SubRevision'   :15,
                            'ModelName'     :16,
                            'FWRevision'    :56,
                            'OEMFWRevision' :64,
                            'Date'          :72,
                            'Capacity'      :84}

        file = open(src, "rb+")
        data = bytearray(file.read())

        ssdBinData = bytearray(self.ssdBinSize)

        ssdBinData[SsdBinOffset['Tag']            :SsdBinOffset['Tag']           + len(SsdBinFmt['Tag'])]           = map(ord, SsdBinFmt['Tag'])
        ssdBinData[SsdBinOffset['SubRevision']    :SsdBinOffset['SubRevision']   + len(SsdBinFmt['SubRevision'])]   = map(ord, SsdBinFmt['SubRevision'])
        ssdBinData[SsdBinOffset['ModelName']      :SsdBinOffset['ModelName']     + len(SsdBinFmt['ModelName'])]     = map(ord, SsdBinFmt['ModelName'])
        ssdBinData[SsdBinOffset['FWRevision']     :SsdBinOffset['FWRevision']    + len(SsdBinFmt['FWRevision'])]    = map(ord, SsdBinFmt['FWRevision'])
        ssdBinData[SsdBinOffset['OEMFWRevision']  :SsdBinOffset['OEMFWRevision'] + len(SsdBinFmt['OEMFWRevision'])] = map(ord, SsdBinFmt['OEMFWRevision'])

        date = datetime.datetime.now()

        ssdBinData[SsdBinOffset['Date']  :SsdBinOffset['Date'] + 3] = map(ord, month_str[date.month - 1])
        ssdBinData[SsdBinOffset['Date']+4:SsdBinOffset['Date'] + 6] = map(ord, str(date.day).zfill(2))
        ssdBinData[SsdBinOffset['Date']+7:SsdBinOffset['Date'] + 11] = map(ord, str(date.year))

        size_list = model['Capacity'].split(' ')

        for size in size_list:
            ssdBinData[SsdBinOffset['Capacity'] + size_list.index(size) * 2  :SsdBinOffset['Capacity'] + (size_list.index(size) + 1) * 2] = unpack("2B", pack("H", int(size)))

        if len(ssdBinData) > self.ssdBinSize:
            self.BuildFail(ErrorCode.EcSsdBinSizeOverflow)

        data += ssdBinData

        file.seek(0)
        file.write(data)
        file.close()

    def DataPadding(self, src, dest, align, bundsize, fnt):
        file = open(src, "rb")
        data = bytearray(file.read())
        file.close()

        if (fnt == "crc"):
            padData  = 0xFF
            currBinSize  = len(data)

            if (currBinSize > bundsize):
                print("Error FW Size [%d] Max Size[%d]" % (currBinSize, bundsize))
                self.BuildFail(ErrorCode.EcBinSizeOverflow)

            padSize = (align - 4 - currBinSize % align) % align
            print("Firmware Padding: %d Bytes" % padSize)

            for i in range(0, padSize):
                data.append(padData)

            crcValue = (binascii.crc32(data) & 0xFFFFFFFF)
            data += bytearray(unpack("4B", pack("I",crcValue)))

        if (fnt == "tag"):
            fwtag = bytearray(self.mainCodeTagSize)
            fwtag[0:self.mainCodeTagSize] = [0xFF] * self.mainCodeTagSize
            fwtag += data
            data = fwtag

        if (fnt == "OPROM"):
            file = open(self.oprom_dir + os.environ["_OPROM_"] +'_OPROM.bin', "rb")
            opRomdata = bytearray(file.read())
            data += opRomdata

        file = open(dest, "wb")
        file.write(data)
        file.close()

    def CheckStackSize(self, src, dest, stack_limit, depth_limit):
        if os.path.isfile(src):
            match_target = 'function'
            pattern_func  = re.compile(r"\<\/a\>(\w+)\<\/STRONG\> \((Thumb|ARM), (\d+) bytes, Stack size (\d+) bytes, (\w+.o)")
            pattern_depth = re.compile(r"\S+Max Depth = (\d+)")
            single_entry  = None
            group_list = []
            for i, line in enumerate(open(src, "r")):

                match = re.search(pattern_func, line)
                if match:
                    if single_entry != None:
                        if len(single_entry) == 4:
                            single_entry.append(0)
                        group_list.append(single_entry)
                    single_entry = [match.group(1), match.group(2), int(match.group(3)), int(match.group(4))]
                else:
                    match = re.search(pattern_depth, line)
                    if match:
                        single_entry.append(int(match.group(1)))

            group_list.sort(key=itemgetter(0), reverse=True)

            file_out = open(dest, "w")
            index = 0
            print (dest+':')
            for l in group_list:
                if l[3] > stack_limit:
                    index = index + 1
                    Color.prRed ("  (%d) %35s => Stack: %4s Bytes" % (index, l[0].rjust(35,'-'), l[3]))

                if l[4] > depth_limit:
                    index = index + 1
                    Color.prYellow ("  (%d) %35s => MaxDepth: %4s Bytes" % (index, l[0].rjust(35,'-'), l[4]))

                file_out.write("[%40s] => Code Size(%5s): %4s Bytes, Stack Size: %3s Bytes Max Stack Depth: %4s Bytes\n" % (l[0], l[1], l[2], l[3], l[4]))
            file_out.close()

    def CheckObjectRef(self, src):
        if os.path.isfile(src):
            sys_obj  = None
            curr_obj = {'name':None, 'ref':None}
            fp = open(src, 'r')
            lines = fp.readlines()
            fp.close()

            ref_net = []
            pattern = re.compile( r'(.*) refers to (.*)');
            for line in lines:
                match = re.match(pattern, line)
                if match:
                    src_obj_name = match.group(1).lstrip().split('(')[0]
                    ref_obj_name = match.group(2).lstrip().split('(')[0]

                    ref_obj = None
                    for obj in ref_net:
                        if obj['name'] == ref_obj_name:
                            ref_obj = obj
                            break

                    if ref_obj == None:
                        ref_obj = {'name':ref_obj_name, 'ref':[]}
                        ref_net.append(ref_obj)

                    if curr_obj['name'] != src_obj_name:
                        curr_obj = None
                        for obj in ref_net:
                            if obj['name'] == src_obj_name:
                                curr_obj = obj
                                break

                        if curr_obj == None:
                            curr_obj = {'name':src_obj_name, 'ref':[ref_obj]}
                            ref_net.append(curr_obj)
                        if src_obj_name == 'HalSys.o':
                            sys_obj = curr_obj
                    else:
                        curr_obj['ref'].append(ref_obj)

            tmp_list = []
            self.RetriveRefObj(sys_obj, tmp_list, 0, 2)
            ref_list = list(set(tmp_list))

            watch_list = []
            pattern = re.compile(r"^(\s+\d+){4}(\s+\d+){2}\s+(\w+\.o)");
            for line in lines:
                match = re.match(pattern, line)
                if match:
                    if int(match.group(1)) != 0:
                        watch_list.append(match.group(3))

            print (src + ':')
            index = 0;
            for fname in watch_list:
                for ref_name in ref_list:
                    if fname == ref_name:
                        index = index + 1
                        Color.prYellow ('  (%d) %30s' % (index, fname.rjust(30, '-')))

    def RetriveRefObj(self, obj, obj_list, curr_level, final_level):
        #print ('%s' % (obj['name'].rjust(curr_level*20, '_')))
        obj_list.append(obj['name'])
        if curr_level < final_level:
            for next_obj in obj['ref']:
                self.RetriveRefObj(next_obj, obj_list, curr_level + 1, final_level)

    def CheckBinarySize(self, src, size, tag):
        fileSize  = os.path.getsize(src)

        if fileSize > size:
            print ("%s File Size Overlimit, Curr=%d Max=%d" %(tag, fileSize, size))
            return 1
        else:
            #Get Real Binary Size
            if tag ==  "Boot Code":
                self.bootCodeSize = fileSize
                print("%s Real Size %d byte\n" %(tag, self.bootCodeSize))
            if tag == "Main Code":
                self.mainCodeSize = fileSize
                print("%s Real Size %d byte\n" %(tag, self.mainCodeSize))
            if tag == "OPROM Code":
                self.opRomCodeSize = fileSize
                print("%s Real Size %d byte\n" %(tag, self.opRomCodeSize))
        return 0

    def PrintSummery(self, src):
        if os.path.isfile(src):
            f = open(src, 'r')

            data = f.read()
            lines = data.splitlines()
            for i in lines:
                m = re.search('Total RO  Size', i)
                if m:
                    print(i)
            f.close()

    def CheckHostId(self, option):
        if platform.system() == 'Linux':
            if option == 'modify':
                find = False
                adapterList = []

                for net in os.listdir("/sys/class/net"):
                    file = open('/sys/class/net/'+net+'/address', 'r')
                    macaddr = file.read().rstrip()
                    file.close()

                    if re.search('pci', os.path.realpath('/sys/class/net/'+net)):
                        adapterList.append({'name':net, 'mac':macaddr})

                        if macaddr == '00:1f:c6:d1:d1:d2':
                            find = True
                            break

                if find == False:
                    for adapter in adapterList:
                        print ('[%s:%s]' % (adapter['name'], adapter['mac']))

                        if os.system('sudo ifconfig '+adapter['name']+' hw ether 00:1f:c6:d1:d1:d2') == 0:
                            self.backAdapter = adapter
                            break

            elif option == 'restore':
                if self.backAdapter != None:
                    os.system('sudo ifconfig '+self.backAdapter['name']+' hw ether '+self.backAdapter['mac'])

    def CleanAll(self):
        if os.path.isdir(self.out_dir):
            shutil.rmtree(self.out_dir)
        if os.path.isfile(self.bin_dir+'/'+self.bin_prefix+'.bin'):
            os.remove(self.bin_dir+'/'+self.bin_prefix+'.bin')

    def CheckCode(self):
        if platform.system() == 'Linux':
            if os.path.isdir(self.out_dir) == 0:
                os.mkdir(self.out_dir)
            if os.system("cppcheck --enable=all --force --inconclusive -j"+str(multiprocessing.cpu_count())+" . 2> "+self.out_dir+"/CppCheck"):
                os.system('sudo apt install cppcheck')

    def ParseCmd(self, argc, argv):
        if argc > 0:
            if argv[1] == "ramdisk":
                self.hidden_option = "ramdisk"
                self.ReleaseBuild()
            elif argv[1] == "uart":
                self.hidden_option = "uart"
                self.ReleaseBuild()
            elif argv[1] == "pcie":
                self.hidden_option = "pcie"
                self.ReleaseBuild()
            elif argv[1] == "help":
                self.Usage()
            elif argv[1] == 'test':
                self.TestBuild()
            elif argv[1] == 'check':
                self.CheckCode()
            elif argv[1] == 'quick':
                self.hidden_option = "quick"
                self.ReleaseBuild()
            else:
                print('Invalid arguement!')
        else:
            self.ReleaseBuild()

    def BuildFail(self, errorCode):
        s = ErrorCode(errorCode)
        print(s)
        BuildFirmwareObj.CheckHostId('restore')
        sys.exit(1)

    def SigCtrlCHandler(self, signum, frame):
        BuildFirmwareObj.CheckHostId('restore')
        sys.exit(1)

    def Usage(self):
        print ("Liteon Build Firmware Tool")
        print ("Usage: build.py [options]")
        print ("Options")
        print ("    ramdisk        build ramdisk firmware")
        print ("    release        build release firmware")
        print ("    uart           build uart download firmware")
        print ("    pcie           build pcie download firmware")
        print ("    test           build all generations firmware")
        print ("    quick          quick build firmware based on previous setting")
        print ("    check          run cppcheck")

class FWName():
    Generation  = { 'CX2':'4',
                    'EP2':'9',
                    'EP3':'D'}

    FormFactor  = { 'M.2_22110' :'K',
                    'M.2_2280'  :'G',
                    'M.2_2260'  :'J',
                    'M.2_2242'  :'S',
                    'M.2_2280SS':'8',
                    'M.2_2260SS':'6',
                    'M.2_2242SS':'4',
                    'U.2'       :'2'}

    Customer    = { 'RD'        :'RD',
                    'AG'        :'AG',
                    'AP'        :'AP',
                    'DQ'        :'DQ',
                    'MICROSOFT' :'28',
                    'DELL'      :'11',
                    'LENOVO'    :'22',
                    'HP'        :'17'}

    Capacity    = { 'Auto'      :'#'}

class ModelName():
    Prefix      = { 'LITEON'    :'LITEON ',
                    'PLEXTOR'   :'PLEXTOR ',
                    'INTEL'     :'INTEL ',
                    'SAMSUNG'   :'SAMSUNG ',
                    'TOSHIBA'   :'TOSHIBA ',
                    'SNADISK'   :'SNADISK ',
                    'NONE'      :''}

    FormFactor  = FWName.FormFactor

    Nand        = { 'TSB_15NM_CMLC' : 'B',
                    'TSB_3D_TLC'    : 'W'}

    Capacity    = FWName.Capacity

    Postfix     = { 'DELL'  :'-Q11 NVMe LITEON #',
                    'HP'    :'-HP',
                    'NONE'  :''}

class Color():
    if platform.system() == 'Linux':
        RED          = '\033[91m'
        GREEN        = '\033[92m'
        YELLOW       = '\033[93m'
        LIGHT_PURPLE = '\033[94m'
        PURPLE       = '\033[95m'
        CYAN         = '\033[96m'
        GRAY         = '\033[97m'
        BLACK        = '\033[98m'
        END          = '\033[0m'

        @staticmethod
        def prRed(prt):
            print("\033[91m {}\033[00m" .format(prt))

        @staticmethod
        def prGreen(prt):
            print("\033[92m {}\033[00m" .format(prt))

        @staticmethod
        def prYellow(prt):
            print("\033[93m {}\033[00m" .format(prt))

        @staticmethod
        def prLightPurple(prt):
            print("\033[94m {}\033[00m" .format(prt))

        @staticmethod
        def prPurple(prt):
            print("\033[95m {}\033[00m" .format(prt))

        @staticmethod
        def prCyan(prt):
            print("\033[96m {}\033[00m" .format(prt))

        @staticmethod
        def prGray(prt):
            print("\033[97m {}\033[00m" .format(prt))

        @staticmethod
        def prBlack(prt):
            print("\033[98m {}\033[00m" .format(prt))

    elif platform.system() == 'Windows':
        RED          = ''
        GREEN        = ''
        YELLOW       = ''
        LIGHT_PURPLE = ''
        PURPLE       = ''
        CYAN         = ''
        GRAY         = ''
        BLACK        = ''
        END          = ''

        @staticmethod
        def prRed(prt):
            print("{}".format(prt))

        @staticmethod
        def prGreen(prt):
            print("{}".format(prt))

        @staticmethod
        def prYellow(prt):
            print("{}".format(prt))

        @staticmethod
        def prLightPurple(prt):
            print("{}".format(prt))

        @staticmethod
        def prPurple(prt):
            print("{}".format(prt))

        @staticmethod
        def prCyan(prt):
            print("{}".format(prt))

        @staticmethod
        def prGray(prt):
            print("{}".format(prt))

        @staticmethod
        def prBlack(prt):
            print("{}".format(prt))

class ErrorCode():
    EcNoError             = 0
    EcGenerationError     = 1
    EcModelError          = 2
    EcMakeError           = 3
    EcCompileError        = 4
    EcCodeSizeOverflow    = 5
    EcBinSizeOverflow     = 6
    EcSsdBinTagNotFound   = 7
    EcSsdBinSizeOverflow  = 8
    EcBuildCommonNotFound = 9
    EcBootCodeAddrNotFound= 10

    def __init__(self, value):
        self.value = value
    def __str__(self):
        if self.value == ErrorCode.EcNoError:
            return "No Error"

        if self.value == ErrorCode.EcGenerationError:
            return "Incorrect Generation Number"

        if self.value == ErrorCode.EcModelError:
            return "Incorrect Model Number"

        if self.value == ErrorCode.EcMakeError:
            return "Makefile Error"

        if self.value == ErrorCode.EcCompileError:
            return "Compiling Error"

        if self.value == ErrorCode.EcCodeSizeOverflow:
            return "Code Size Overflow Error"

        if self.value == ErrorCode.EcBinSizeOverflow:
            return "Binary Size Overflow Error"

        if self.value == ErrorCode.EcSsdBinTagNotFound:
            return "Unable to find SsdBinTag"

        if self.value == ErrorCode.EcSsdBinSizeOverflow:
            return "SsdBin Size Overflow"

        if self.value == ErrorCode.EcBuildCommonNotFound:
            return "BuildCommon.h not found"

        if self.value == ErrorCode.EcBootCodeAddrNotFound:
            return "Unable to find boot code addr"

#=============================================================================
#  MAIN
#=============================================================================
if __name__ == "__main__":
    BuildFirmwareObj = BuildFirmwareClass()
    BuildFirmwareObj.ParseCmd(len(sys.argv)-1, sys.argv)
