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
* Description:  Custom listbox component for the contact list
*
*/


#include "ccacontactlistbox.h"
#include "ccacontactlistboxmodel.h"
#include "MCAMainViewArrayPC.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CCAContactListBox::CreateModelL
// Here we change the listbox model
// ---------------------------------------------------------------------------
//
void CCAContactListBox::CreateModelL()
    {
    // we have to make sure that the model is ours,
    // so we can leave if the model creation fails.
    // (just make sure that the pointer is NULL)
    delete iModel;
    iModel = NULL;
    iModel = new ( ELeave ) CCAContactListBoxModel;
    }

// ---------------------------------------------------------------------------
// CCAContactListBox::SetContactListModel
// ConstructL has to be called before calling this
// ---------------------------------------------------------------------------
//
void CCAContactListBox::SetContactListModel( MCAMainViewArrayPC&
                                             aMainViewArrayPC )
    {
    if ( iModel )
        {
        static_cast<CCAContactListBoxModel*>( iModel )->SetContactListModel( aMainViewArrayPC );
        }
    }

