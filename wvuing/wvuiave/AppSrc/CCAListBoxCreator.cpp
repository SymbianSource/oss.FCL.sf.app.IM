/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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


// INCLUDES
#include    "MCAListBox.h"
#include    "CCAListBoxCreator.h"
#include    "CCAContactListBox.h"

#include    <Chatng.rsg>

// -----------------------------------------------------------------------------
// CCAListBoxCreator::CreateListBoxL
// Create all needed list box
//but here need to have a base class to return the corect type
// -----------------------------------------------------------------------------
//
MCAListBox* CCAListBoxCreator::CreateListBoxL( TInt aTypeID )
    {
    MCAListBox* singlelist = NULL;
    switch ( aTypeID )
        {
        case ESingleStyleListBox:
            singlelist = new ( ELeave ) CCASingleStyleListBox;
            break;
        case ESingle2GraphicStyleListBox :
            singlelist = new ( ELeave ) CCAContactListBox;
        case EDoubleStyleListBox:
            singlelist = new ( ELeave ) CCADoubleStyleListBox;
            break;
        case ESingleLargeStyleListBox:
            singlelist = new ( ELeave ) CCASingleLargeStyleListBox;
            break;
        case ESettingStyleListBox:
            singlelist = new ( ELeave ) CCASettingStyleListBox;
            break;
        case ESingleHeadingPopupMenuStyleListBox:
            singlelist = new ( ELeave ) CCASingleHeadingPopupMenuStyleListBox;
            break;
        case ESingleGraphicStyleListBox:
            singlelist = new ( ELeave ) CCASingleGraphicStyleListBox;
            break;
        default:
            break;

        }

    return singlelist;

    }



CCAContactListBox* CCAListBoxCreator::CreateSingle2GraphicListBoxL( TInt /*aTypeID*/ )
    {
    //case ESingle2GraphicStyleListBox :
    CCAContactListBox* self = new ( ELeave ) CCAContactListBox;
    return self;
    }



// end of file
