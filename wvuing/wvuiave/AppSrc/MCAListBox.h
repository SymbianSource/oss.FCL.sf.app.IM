/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Creator class for views.
*
*/



#ifndef MCALISTBOX_H
#define MCALISTBOX_H

#include <aknlists.h>

/**
*  Common interface of any list box
*
*  @lib chat.app
*  @since 2.1
*/

class MCAListBox
    {


    };
class CAknSingle2GraphicStyleListBox;

enum TListBoxType
    {
    ESingleStyleListBox = 0,
    ESingle2GraphicStyleListBox,
    EDoubleStyleListBox,
    ESingleLargeStyleListBox,
    ESettingStyleListBox,
    ESingleHeadingPopupMenuStyleListBox,
    ESingleGraphicStyleListBox
    };


class CCASingleStyleListBox: public CAknSingleStyleListBox,
            public MCAListBox
    {

    };
/*class CCASingle2GraphicStyleListBox: public CAknSingle2GraphicStyleListBox,
							 public MCAListBox
 {

 }; */

class CCADoubleStyleListBox: public CAknDoubleStyleListBox,
            public MCAListBox
    {

    };

class CCASingleLargeStyleListBox: public CAknSingleLargeStyleListBox,
            public MCAListBox
    {

    };

class CCASettingStyleListBox: public CAknSettingStyleListBox,
            public MCAListBox
    {

    };

class CCASingleHeadingPopupMenuStyleListBox: public CAknSingleHeadingPopupMenuStyleListBox,
            public MCAListBox
    {

    };
class CCASingleGraphicStyleListBox: public CAknSingleGraphicStyleListBox,
            public MCAListBox
    {

    };

#endif      // MCALISTBOX_H
