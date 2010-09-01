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
* Description:  Message wrapper for MCAConversationMessage
*
*/



// INCLUDE FILES
#include    "CCAMessageWrapper.h"
#include    "MCAConversationMessage.h"
#include    "MCAMessageContainer.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAMessageWrapper::CCAMessageWrapper
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCAMessageWrapper::CCAMessageWrapper( MCAConversationMessage& aWrappedMessage,
                                      MCAMessageContainer& aContainer )
        : iMessage( &aWrappedMessage ),
        iContainer( aContainer ),
        iInserted( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CCAMessageWrapper::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCAMessageWrapper* CCAMessageWrapper::NewL( MCAConversationMessage& aWrappedMessage,
                                            MCAMessageContainer& aContainer )
    {
    CCAMessageWrapper* self =
        new( ELeave ) CCAMessageWrapper( aWrappedMessage, aContainer );

    return self;
    }


// Destructor
CCAMessageWrapper::~CCAMessageWrapper()
    {
    //remove thumbnail from message
    if ( !iContainer.IsDeleted() && iMessage )
        {
        iMessage->ReleaseTemporaryContentData();
        }

    delete iMessage;

    iHighlights.Reset();
    iTypes.Reset();
    }



// -----------------------------------------------------------------------------
// CCAMessageWrapper::ThumbPos
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TCursorSelection& CCAMessageWrapper::ThumbPos()
    {
    return iThumbPosition;
    }

// -----------------------------------------------------------------------------
// CCAMessageWrapper::Selection
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TCursorSelection& CCAMessageWrapper::Selection()
    {
    return iSelection;
    }

// -----------------------------------------------------------------------------
// CCAMessageWrapper::WholeSelection
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TCursorSelection& CCAMessageWrapper::WholeSelection()
    {
    return iWholeSelection;
    }

// -----------------------------------------------------------------------------
// CCAMessageWrapper::WholeSelection
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TCursorSelection& CCAMessageWrapper::MessageSelection()
    {
    return iMessageSelection;
    }

// -----------------------------------------------------------------------------
// CCAMessageWrapper::Highlights
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RArray<TCursorSelection>& CCAMessageWrapper::Highlights()
    {
    return iHighlights;
    }

// -----------------------------------------------------------------------------
// CCAMessageWrapper::HighlightTypes
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RArray<TInt>& CCAMessageWrapper::HighlightTypes()
    {
    return iTypes;
    }

// -----------------------------------------------------------------------------
// CCAMessageWrapper::Message
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MCAConversationMessage& CCAMessageWrapper::Message()
    {
    return *iMessage;
    }

// -----------------------------------------------------------------------------
// CCAMessageWrapper::SetInserted
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCAMessageWrapper::SetInserted( TBool aInserted )
    {
    iInserted = aInserted;
    }

// -----------------------------------------------------------------------------
// CCAMessageWrapper::IsInserted
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCAMessageWrapper::IsInserted() const
    {
    return iInserted;
    }

//  End of File
