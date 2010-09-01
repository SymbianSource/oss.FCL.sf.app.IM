/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is interface between chat view and conversation view
*
*/



#ifndef MCAINDICATORTERMINATOR_H
#define MCAINDICATORTERMINATOR_H


// CLASS DECLARATION

/**
*  This is interface between chat view and conversation view
*  When received PTOP message is read indicator is terminated via this interface
*
*  @lib chat.app
*  @since 1.2S
*/
class MCAIndicatorTerminator
    {
    public: // New functions

        /**
        * Terminates indicator
        * @since 1.2
        */
        virtual void TerminateIndication() = 0;


    protected:  // Functions from base classes

        /**
        * Destructor
        */
        virtual ~MCAIndicatorTerminator() {};
    };

#endif      // MCAINDICATORTERMINATOR_H   

// End of File
