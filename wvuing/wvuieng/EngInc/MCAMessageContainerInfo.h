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
* Description:  Message container information provider
*
*/


#ifndef __MCAMESSAGECONTAINERINFO_H__
#define __MCAMESSAGECONTAINERINFO_H__

//	FORWARD CLASS DECLERATIONS

// CLASS DECLARATION

/**
 *  Message container information provider
 *
 *  @lib CAEngine.dll
 *  @since 3.0
 */
class MCAMessageContainerInfo
    {

    public: // Interface

        /**
         * Part of Id of container
         * @return Server address of container.
         */
        virtual const TDesC& ServerAddress() const = 0;

        /**
         * Part of Id of container
         * @return User id of container.
         */
        virtual const TDesC& UserId() const = 0;

        /**
         * Part of Id of container
         * @return Target id of container.
         */
        virtual const TDesC& Target() const = 0;

        /**
         * Part of identification of container
         * @return Screen name of container owner.
         */
        virtual const TDesC& ScreenName() const = 0;

        /**
         * Sets if add to contacts is asked.
         * @param aIsAsked.
         */
        virtual void SetAddToContactAsked( TBool aIsAsked ) = 0;

        /**
         * Is add to contacts asked.
         * @return ETrue if asked already otherwise EFalse.
         */
        virtual TBool IsAddToContactAsked() const = 0;

    protected: // For protection.

        /**
         * Destructor
         */
        virtual ~MCAMessageContainerInfo() {}
    };

#endif      // __MCAMESSAGECONTAINERINFO_H__

// End of File
