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
* Description:  Recorded chats Array PC interface for recorded chats view
*
*/



#ifndef MCARECORDEDCHATSARRAYPC_H__
#define MCARECORDEDCHATSARRAYPC_H__

//  INCLUDES
#include <e32base.h>
#include "TEnumsPC.h"

class MCARecordedChatsArrayPC
    {
    public:
        /**
        * To intialize the recorded chats array PC
        * The string loader class is used to load the defined strings
        * This requires the construction of CCAAppUi to be complete.
        * The Recorded chats array PC gets constructed before the completion of
        * app ui. Hence we call this FinalizeRecChatsArrayPCConstructionL()
        * method from the FinalizeChatConstruction method of CCAAppUI!
        */
        virtual void FinalizeRecChatsArrayPCConstructionL() = 0;

        /**
        * To return to the item at the specified index in the array
        * @param aIndex: The index of item in the array
        * @return TPtrC: pointer to the item at aIndex
        */
        virtual TPtrC16 MdcaPoint( TInt aIndex ) const = 0;

        /**
        * To return to the total number of items in the array
        * @return integer: count of items in the array
        */
        virtual TInt MdcaCount( ) const = 0;

        /**
        * To Get the current index of an item in the array
        * @return integer: current index
        */
        virtual TInt GetCurrentItemIndex() const = 0;

        /**
        * To set the current index of an item in the array
        * @param aCurIndex : integer - current index
        */
        virtual void SetCurrentItemIndex( TInt aCurIndex ) = 0;

        /**
        * Set the next index of the current index of an array
        * @param aCurIndex : integer value which is the current index
        */
        virtual void SetNextIndex( TInt aCurIndex ) = 0;

        /**
        * Get the next index of the current index of an array
        * @return integer: index
        */
        virtual TInt GetNextIndex() const = 0;

        /**
        * Get progress count of given index of an array
        * @return integer count to be set
        */
        virtual TInt GetProgressCount() const = 0;

        /**
        * Set progress count of given index of an array
        * @param aProgressCount: integer count to be set
        */
        virtual void SetProgressCount( TInt aProgressCount ) = 0;

        /**
        * Returns name of recorded chat
        * @param aIndex is index of recorded chat
        * @return Reference to name of recorded chat
        */
        virtual const TDesC& Name( TInt aIndex ) const = 0;

        /**
        * Returns the type of the chat data.
        * @param aIndex is the index to the header array.
        * @return Message type.
        */
        virtual TEnumsPC::TChatType ChatType( TInt aIndex ) const = 0;

        /**
        * Returns time when history save started.
        * @param aIndex is the index to the header array.
        * @return Start time
        */
        virtual TTime StartTime( TInt aIndex ) const = 0;

        /**
        * Returns time when history save ended.
        * @param aIndex is the header index.
        * @return End time.
        */
        virtual TTime EndTime( TInt aIndex ) const = 0;

        /**
        * Returns identification information of history data
        * @param aIndex is the header index
        * @return Identication information of history data.
        */
        virtual const TDesC& Identification( TInt aIndex ) const = 0;

        /*
        * Return the formatted time of saved conversation
        * @return pointer to the time
        */
        virtual TPtr GetDateFormat() = 0 ;

        /*
        * Return the formatted start time of saved conversation
        * @return pointer to the time
        */
        virtual TPtr GetStartDateFormat() = 0;

        /*
        * Return the formatted end time of saved conversation
        * @return pointer to the time
        */
        virtual TPtr GetEndDateFormat() = 0;

        /*
        * Return the time of saved conversation
        * @return pointer to the time
        */
        virtual TPtr Time() const = 0;

        /**
         * Recorded chat index by name.
         * @since series 60  v3.2
         * @param aName Name of recorded chat.
         * @return Index of chat or KErrNotFound if
         *         it was not found.
         */
        virtual TInt Index( const TDesC& aName ) const = 0;

    public:
        virtual ~MCARecordedChatsArrayPC()
            {

            };
    };

#endif      // MCARECORDEDCHATSARRAYPC_H__

// End of File