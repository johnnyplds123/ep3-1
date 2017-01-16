#  ============================================================================
#
#      Copyright (c) Marvell Corporation 2000-2012  -  All rights reserved
#
#   This computer program contains confidential and proprietary information,
#   and  may NOT  be reproduced or transmitted, in whole or in part,  in any
#   form,  or by any means electronic, mechanical, photo-optical, or  other-
#   wise, and  may NOT  be translated  into  another  language  without  the
#   express written permission from Marvell Corporation.
#
#  ============================================================================
#  =      C O M P A N Y      P R O P R I E T A R Y      M A T E R I A L       =
#  ============================================================================
#------------------------------------------------------------------------------
# Include component make file
#------------------------------------------------------------------------------
include Share/Share.mak                 # build share
include Core/Apl/Cache/Cache.mak        # build cache
include Core/Apl/Fio/Fio.mak            # build fio job component
include Core/Hal/Bcm/Bcm.mak            # build bcm
include Host/Host.mak                   # build host

BUILD_OBJS += $(SHARE_OBJS)
BUILD_OBJS += $(CACHE_OBJS)
BUILD_OBJS += $(FIO_OBJS)
BUILD_OBJS += $(BCM_OBJS)
BUILD_OBJS += $(HOST_OBJS)
