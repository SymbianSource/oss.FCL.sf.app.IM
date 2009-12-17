/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface for observing content processors
*
*/


#ifndef MCACONTENTPROCESSOBSERVER_H
#define MCACONTENTPROCESSOBSERVER_H

//	FORWARD CLASS DECLERATIONS
class MCAContentProcessor;
class MCAMessage;

// CLASS DECLARATION

/**
 *  Interface for observing content processors
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */
class MCAContentProcessObserver
    {
    public: // Interface

        /**
         * Handle processing completion. Called by content processor when
         * processing is ready.
         * @param aProcessor Processor which made processing.
         * @param aStatus, Status of processing. KErrNone -> OK.
         */
        virtual void HandleProcessingComplete( MCAContentProcessor& aProcessor,
                                               MCAMessage& aMessage,
                                               TInt aStatus ) = 0;

    protected: // For protection.

        /**
         * Destructor
         */
        virtual ~MCAContentProcessObserver() {}
    };

#endif      // MCACONTENTPROCESSOBSERVER_H

// End of File
