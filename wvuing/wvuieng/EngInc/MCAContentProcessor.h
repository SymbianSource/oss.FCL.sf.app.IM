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
* Description:  Interface for content processors
*
*/


#ifndef __MCACONTENTPROCESSOR_H__
#define __MCACONTENTPROCESSOR_H__

//	FORWARD CLASS DECLERATIONS
class MCAMessage;

// CLASS DECLARATION

/**
 *  Interface for content processors
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */
class MCAContentProcessor
    {
    public: // Interface

        /**
         * Request processing of content.
         * @aMessage Message which to process.
         * @aProcessState. This is updated during processing.
         */
        virtual void RequestProcessingL( MCAMessage& aMessage ) = 0;

        /**
         * Start processing of content.
         * @aMessage Message which to process.
         * Leaves with KErrInuUse, if another item is processed at the moment.
         */
        virtual void StartProcessingL( MCAMessage& aMessage ) = 0;

        /**
         * Request cancelling of processing.
         * @aMessage To identify which message asks canceling
         */
        virtual void CancelProcessing( MCAMessage& aMessage ) = 0;

        /**
         * Request processing remove. If processing is already started. Cancels processing.
         * @aMessage To identify which message asks canceling
         */
        virtual void RemoveProcessingL( MCAMessage& aMessage ) = 0;

    protected: // For protection.

        /**
         * Destructor
         */
        virtual ~MCAContentProcessor() {}
    };

#endif      // __MCACONTENTPROCESSOR_H__

// End of File
