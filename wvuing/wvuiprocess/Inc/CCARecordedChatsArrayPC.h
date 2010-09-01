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
* Description:  Recorded chats Array PC for recorded chats view
*
*/



#ifndef CCARECORDEDCHATSARRAYPC_H__
#define CCARECORDEDCHATSARRAYPC_H__

#include    "MCARecordedChatsArrayPC.h"

//Forward Declaration
class MCALoggerHeadersInterface;

// CLASS DECLARATION
/**
*  Process component class for recorded chats (saved conversations)
*
*  @lib wvuiprocessng.lib
*  @since 3.2
*/
class CCARecordedChatsArrayPC: public CBase, public MCARecordedChatsArrayPC
    {
    public:

        /**
        * Static Constructor
        * @param aLoggerHeadersInterface: Reference to the Logger component
        */
        static CCARecordedChatsArrayPC* NewL( MCALoggerHeadersInterface&
                                              aLoggerHeadersInterface );

        /*
        * Destructor
        */
        ~CCARecordedChatsArrayPC();

        /**
        * To intialize the recorded chats array PC
        * The string loader class is used to load the defined strings
        * This requires the construction of CCAAppUi to be complete.
        * The Recorded chats array PC gets constructed before the completion of
        * app ui. Hence we call this FinalizeRecChatsArrayPCConstructionL()
        * method from the FinalizeChatConstruction method of CCAAppUI!
        */
        virtual void FinalizeRecChatsArrayPCConstructionL();

    public:

        /**
         * Recorded chat index by name.
         * @since series 60  v3.2
         * @param aName Name of recorded chat.
         * @return Index of chat or KErrNotFound if
         *         it was not found.
         */
        TInt Index( const TDesC& aName ) const;

        /**
        * To return to the item at the specified index in the array
        * @param aIndex: The index of item in the array
        * @return TPtrC: pointer to the item at aIndex
        */
        virtual TPtrC16 MdcaPoint( TInt aIndex ) const ;

        /**
        * To return to the total number of items in the array
        * @return integer: count of items in the array
        */
        virtual TInt MdcaCount( ) const ;

        /**
        * To Get the current index of an item in the array
        * @return integer: current index
        */
        inline TInt GetCurrentItemIndex() const
            {
            return iCurrentItemIndex;
            }
        /**
        * To set the current index of an item in the array
        * @param aCurIndex : integer - current index
        */
        inline void SetCurrentItemIndex( TInt aCurIndex )
            {
            iCurrentItemIndex = aCurIndex;
            }

        /**
        * Set the next index of the current index of an array
        * @param aCurIndex : integer value which is the current index
        */
        inline void SetNextIndex( TInt aCurIndex )
            {
            iNextIndex = aCurIndex;
            }
        /**
        * Get the next index of the current index of an array
        * @return integer: index
        */
        inline TInt GetNextIndex() const
            {
            return iNextIndex;
            }
        /**
        * Get progress count of given index of an array
        * @return integer: progress count of the array
        */
        inline TInt GetProgressCount() const
            {
            return iProgressCount;
            }
        /**
        * Set progress count of given index of an array
        * @param aProgressCount: integer count to be set
        */
        inline void SetProgressCount( TInt aProgressCount )
            {
            iProgressCount = aProgressCount;
            }
        /**
        * Returns name of recorded chat
        * @param aIndex is index of recorded chat
        * @return Reference to name of recorded chat
        */
        virtual const TDesC& Name( TInt aIndex ) const;

        /**
        * Returns the type of the chat data.
        * @param aIndex is the index to the header array.
        * @return Message type.
        */
        virtual TEnumsPC::TChatType ChatType( TInt aIndex ) const;

        /**
        * Returns time when history save started.
        * @param aIndex is the index to the header array.
        * @return Start time
        */
        virtual TTime StartTime( TInt aIndex ) const;

        /**
        * Returns time when history save ended.
        * @param aIndex is the header index.
        * @return End time.
        */
        virtual TTime EndTime( TInt aIndex ) const;

        /**
         * Returns identification information of history data
         * @param aIndex is the header index
         * @return Identication information of history data.
         */
        virtual const TDesC& Identification( TInt aIndex ) const;

        /*
        * Return the formatted time of saved conversation
        * @return pointer to the time
        */
        virtual TPtr GetDateFormat() ;

        /*
        * Return the formatted start time of saved conversation
        * @return pointer to the time
        */
        virtual TPtr GetStartDateFormat() ;

        /*
        * Return the formatted end time of saved conversation
        * @return pointer to the time
        */
        virtual TPtr GetEndDateFormat();

        /*
        * Return the time of saved conversation
        * @return pointer to the time
        */
        virtual TPtr Time() const;

    private:

        /**
        * Default constructor
        * @param aLoggerHeadersInterface: Reference to the logger component
        */
        CCARecordedChatsArrayPC( MCALoggerHeadersInterface& aLoggerHeadersInterface );

    private://Data members


        // Owns. This is for formatting start time for sendMTM
        HBufC* iStartDateFormat;

        // Owns. This is for formatting start time for sendMTM
        HBufC* iEndDateFormat;

        // Owns This is where time is written.
        HBufC*	iTime;

        // Owns This is for formatting date and time.
        HBufC* iDateFormat;

        // Next marked item for deletion.
        TInt iNextIndex;

        // Curren item for recorded chat view.
        TInt iCurrentItemIndex;

        // Number of delete items.
        TInt iProgressCount;

        // Owns. Buffer that hold's information about fetched item
        HBufC* iData;

        // Does not Own. Reference to the Logger component
        MCALoggerHeadersInterface& iLoggerHeadersInterface;

    };

#endif      // CCARECORDEDCHATSARRAYPC_H__

// End of File

