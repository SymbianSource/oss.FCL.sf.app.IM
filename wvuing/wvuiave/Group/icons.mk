#
# Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: Icons makefile for UI
#

# ----------------------------------------------------------------------------
# z root
# ----------------------------------------------------------------------------
ifeq (WINS,$(findstring WINS, $(PLATFORM)))
  ZDIR=\epoc32\release\$(PLATFORM)\$(CFG)\z
else
  ZDIR=\epoc32\data\z
endif

# ----------------------------------------------------------------------------
# targets
# ----------------------------------------------------------------------------
MIF_HEADERDIR=\epoc32\include
MIF_TARGETDIR=$(ZDIR)\resource\apps
AIF_TARGETDIR=$(ZDIR)\resource\apps

MIF_HEADERFILENAME=$(MIF_HEADERDIR)\chatNG.mbg
MIF_TARGETFILENAME=$(MIF_TARGETDIR)\chatNG.mif
AIF_TARGETFILENAME=$(AIF_TARGETDIR)\chatNG_aif.mif

# ----------------------------------------------------------------------------
# icons that need to have different names when using bitmaps
# ----------------------------------------------------------------------------
ifeq ($(SCALABLE_ICONS), off)
  AIF_APPLICATION_ICON=/c8,8 qgn_menu_im_lst /c8,8 qgn_menu_im_cxt
  APPLICATION_CONTEXT_ICONS=\
        /c8,8 qgn_menu_im_on_cxt\
        /c8,8 qgn_menu_im_off_cxt\
        /c8,8 qgn_menu_im_away_cxt\
        /c8,8 qgn_menu_im_invisible_cxt\
        /c8,8 qgn_menu_im_busy_cxt
else
  AIF_APPLICATION_ICON=/c8,8 qgn_menu_im
  APPLICATION_CONTEXT_ICONS=\
        /c8,8 qgn_menu_im_on\
        /c8,8 qgn_menu_im_off\
        /c8,8 qgn_menu_im_away\
        /c8,8 qgn_menu_im_invisible\
        /c8,8 qgn_menu_im_busy
endif

# ----------------------------------------------------------------------------
# empty rules 
# ----------------------------------------------------------------------------
do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : 
	if exist $(MIF_HEADERFILENAME) erase $(MIF_HEADERFILENAME)
	if exist $(MIF_TARGETFILENAME) erase $(MIF_TARGETFILENAME)
	if exist $(AIF_TARGETFILENAME) erase $(AIF_TARGETFILENAME)


LIB : do_nothing

CLEANLIB : do_nothing

# ----------------------------------------------------------------------------
# NOTE 1: DO NOT DEFINE MASK FILE NAMES! They are included automatically by
# MifConv if the mask detph is defined.
#
# NOTE 2: Usually, source paths should not be included in the bitmap
# definitions. MifConv searches for the icons in all icon directories in a
# predefined order, which is currently \s60\icons, \s60\bitmaps2, \s60\bitmaps.
# The directory \s60\icons is included in the search only if the feature flag
# __SCALABLE_ICONS is defined.
# ----------------------------------------------------------------------------

RESOURCE : AIFICONS MIFICONS

AIFICONS :
	mifconv $(AIF_TARGETFILENAME) \
		$(AIF_APPLICATION_ICON)

MIFICONS :
	mifconv $(MIF_TARGETFILENAME) /h$(MIF_HEADERFILENAME) \
		/c8 qgn_stat_im_uni\
		$(APPLICATION_CONTEXT_ICONS)\
		/c8,8 qgn_prop_im_friends\
		/c8,8 qgn_prop_im_ibox\
		/c8,8 qgn_prop_im_ibox_new\
		/c8,8 qgn_prop_im_saved\
		/c8,8 qgn_prop_im_user_created_active\
		/c8,8 qgn_prop_im_user_created_active_unread\
		/c8,8 qgn_prop_im_nonuser_created_saved_active\
		/c8,8 qgn_prop_im_nonuser_created_saved_active_unread\
		/c8,8 qgn_prop_im_nonuser_created_saved_inactive\
		/c8,8 qgn_prop_im_joined_not_saved\
		/c8,8 qgn_prop_im_joined_not_saved_unread\
		/c8,8 qgn_prop_im_list\
		/c8,8 qgn_prop_im_list_unread\
		/c8,1 qgn_prop_im_imsg_new\
		/c8,1 qgn_indi_im_watch_add\
		/c8,1 qgn_indi_phone_add\
		/c8,1 qgn_prop_im_friend_off\
		/c8,1 qgn_prop_im_friend_on\
		/c8,1 qgn_prop_im_on_mobile_title\
		/c8,1 qgn_prop_im_friend_away\
		/c8,1 qgn_prop_im_friend_busy\
		/c8,1 qgn_prop_im_friend_invisible\
		/c8,1 qgn_prop_im_from\
		/c8,1 qgn_prop_im_saved_chat_tab3\
		/c8,1 qgn_prop_im_saved_chat_tab4\
		/c8,1 qgn_prop_im_friend_tab3\
		/c8,1 qgn_prop_im_friend_tab4\
		/c8,1 qgn_prop_im_ibox_tab3\
		/c8,1 qgn_prop_im_ibox_tab4\
		/c8,1 qgn_prop_im_list_tab4\
		/c8,1 qgn_prop_im_saved_conv\
		/c8,1 qgn_prop_im_saved_chat\
		/c8,1 qgn_indi_marked_add\
		/c8,1 qgn_prop_im_user_created_inactive\
		/c8,1 qgn_prop_im_chats_collapsed\
		/c8,1 qgn_prop_im_chats_expanded\
		/c8,1 qgn_prop_im_friends_collapsed\
		/c8,1 qgn_prop_im_friends_expanded\
		/c8,1 qgn_prop_im_new_invit\
		/c8,1 qgn_prop_im_user_blocked\
		/c8,1 qgn_prop_im_imsg\
		/c8,1 qgn_prop_im_many\
		/c8,1 qgn_prop_im_tome\
		/c8,1 qgn_indi_sett_protected_add\
		/c8,1 qgn_prop_image_corrupted\
		/c8,1 qgn_prop_unknown\
  		/c8,8 qgn_prop_im_on_title\
  		/c8,8 qgn_prop_im_off_title\
  		/c8,8 qgn_prop_im_away_title\
  		/c8,8 qgn_prop_im_invisible_title\
  		/c8,8 qgn_prop_im_busy_title\
		\
		/c8,1 qgn_prop_im_smileys_angry\
		/c8,1 qgn_prop_im_smileys_bored\
		/c8,1 qgn_prop_im_smileys_crying\
		/c8,1 qgn_prop_im_smileys_glasses\
		/c8,1 qgn_prop_im_smileys_happy\
		/c8,1 qgn_prop_im_smileys_indif\
		/c8,1 qgn_prop_im_smileys_kiss\
		/c8,1 qgn_prop_im_smileys_laugh\
		/c8,1 qgn_prop_im_smileys_robot\
		/c8,1 qgn_prop_im_smileys_sad\
		/c8,1 qgn_prop_im_smileys_shock\
		/c8,1 qgn_prop_im_smileys_skeptical\
		/c8,1 qgn_prop_im_smileys_sleepy\
		/c8,1 qgn_prop_im_smileys_sunglasses\
		/c8,1 qgn_prop_im_smileys_surprise\
		/c8,1 qgn_prop_im_smileys_tired\
		/c8,1 qgn_prop_im_smileys_veryhappy\
		/c8,1 qgn_prop_im_smileys_verysad\
		/c8,1 qgn_prop_im_smileys_wickedsmile\
		/c8,1 qgn_prop_im_smileys_wink\
		/c8,1 qgn_graf_im_splash_screen\
		/c8,1 qgn_graf_im_signin_screen

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(MIF_HEADERFILENAME) && \
	@echo $(MIF_TARGETFILENAME) && \
	@echo $(AIF_TARGETFILENAME)

FINAL : do_nothing
