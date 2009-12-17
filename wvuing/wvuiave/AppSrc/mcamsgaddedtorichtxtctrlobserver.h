/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface to observe events in text control

*
*/




#ifndef MCAMSGADDEDTORICHTXTCTRLOBSERVER

#define MCAMSGADDEDTORICHTXTCTRLOBSERVER



#include <e32def.h>



/**

 *  Interface to observe events from text controls

 *  @lib chatng.exe

 *  @since S60 v3.2

 */

class MCAMsgAddedToRichTxtCtrlObserver

    {

    public:


        /**

         * This method is called when new message is inserted to rich text container

         * @since S60 v3.2

         * @return none

         */

        virtual void HandleMessageAddedL( const TInt aMessageCount ) = 0;



    protected:



        ~MCAMsgAddedToRichTxtCtrlObserver() {}

    };





#endif // MCAMSGADDEDTORICHTXTCTRLOBSERVER

