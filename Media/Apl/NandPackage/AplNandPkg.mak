#------------------------------------------------------------------------------
# Include path
#------------------------------------------------------------------------------
INCLUDE_DIR  += -IMedia/Apl/NandPackage
INCLUDE_DIR  += -IMedia/Apl/NandPackage/$(_VENDOR_)

#------------------------------------------------------------------------------
# Object file list
#------------------------------------------------------------------------------
MEDIA_APL_NANDPKG_OBJS =

MEDIA_APL_NANDPKG_OBJS += $(OBJ_DIR)/PkgTable.o
MEDIA_APL_NANDPKG_OBJS += $(OBJ_DIR)/Pkg_$(_VENDOR_)_$(_NAND_).o

ifeq ($(_VENDOR_), TSB)
    ifeq ($(_NAND_), MLC)
        MEDIA_APL_NANDPKG_OBJS += Media/Apl/NandPackage/$(_VENDOR_)/Pkg_$(_VENDOR_)_$(_NAND_)_Addons.o
    endif
endif

#------------------------------------------------------------------------------
# Include component make file
#------------------------------------------------------------------------------

