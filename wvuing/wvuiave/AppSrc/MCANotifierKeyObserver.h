/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Observes when key values are changed
*
*/


#ifndef MCANOTIFIERKEYOBSERVER_H
#define MCANOTIFIERKEYOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <bldvariant.hrh>

class TUpdatedKey
    {
    public:
        TUpdatedKey() {};
    public:
        TUid    iUid;
        TUint32 iKey;
        TInt    iValue;
    };

/**
*  Notifier inteface for Pub&Sub or shared data key changes
*  (depends on which system is used in the platform)
*
*  @since Series 60 2.6
*/
class MCANotifierKeyObserver
    {
    public: // New functions

        /**
        * This function is called when a PS or shared data key value is changed
        * @param aUpdatedValues
        */
        virtual void HandleKeyChange( TUpdatedKey& aUpdatedValues ) = 0;

    protected: // Destructor
        virtual ~MCANotifierKeyObserver() {};
    };
#endif      // MCANOTIFIERKEYOBSERVER_H 

// End of File

