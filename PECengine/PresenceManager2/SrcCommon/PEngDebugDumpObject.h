/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Object dumpper.
*
*/

#ifndef PENGDEBUGDUMPOBJECT_H__
#define PENGDEBUGDUMPOBJECT_H__

#if defined (_DEBUG)

//  INCLUDES
#include <E32Std.h>
#include <MPEngPresenceAttrModel2.h>
#include <MPEngTransactionStatus2.h>



//FORWARD DECLARATION
NONSHARABLE_CLASS( PEngDebugDumpObject )
    {
public:
    static void Dump( MPEngPresenceAttrModel2& aModel );
    static void Dump( RPointerArray<MPEngPresenceAttrModel2>& aModels );
    static void Dump( MPEngTransactionStatus2& aStatus );

private:  //constructor / destructor
    PEngDebugDumpObject();
    ~PEngDebugDumpObject();
    };



#include <PEngDebugDumpObject.inl>
#endif      //  _DEBUG

#endif      //  PENGDEBUGDUMPOBJECT_H__


//  End of File

