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
* Description:  Message utils.
*
*/


#ifndef __MCAMESSAGEUTILS_H__
#define __MCAMESSAGEUTILS_H__

//	FORWARD CLASS DECLERATIONS
class MCAMessageCreator;
class MCAMessageErrorInformer;
class MCAContentProcessor;
class MCABufferMemoryHandler;

// CLASS DECLARATION

/**
 *  Message utils.
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */
class MCAMessageUtils
    {
    public: // Interface

        /**
         * Access to message creator
         * @return Access to message creator
         */
        virtual MCAMessageCreator& MessageCreator() const = 0;

        /**
         * Access to message error informer
         * @return Access to message error informer
         */
        virtual MCAMessageErrorInformer& MessageErrorInformer() const = 0;

        /**
         * Access to image scaler processor.
         * @return Access to image scaler processor
         */
        virtual MCAContentProcessor& ImageScaler() const = 0;

        /**
         * Access to memory handler.
         */
        virtual MCABufferMemoryHandler& MemoryHandler() const = 0;

    protected:
        /**
         * Virtual destructor
         */
        virtual ~MCAMessageUtils() {}
    };

#endif      // __MCAMESSAGEUTILS_H__

// End of File
