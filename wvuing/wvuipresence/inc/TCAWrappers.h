/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Presence-list handling interface for clients
*
*/


#ifndef TCAWRAPPERS_H
#define TCAWRAPPERS_H

// INCLUDES
#include <bamdesca.h>

// FORWARD DECLARATIONS
class MPEngContactList2;
class CPEngContactListStore2;
class CPEngTrackedPresenceID2;
class CPEngTrackedPresenceIDs2;

// CLASS DECLARATION

class TContactListsWrapper : public MDesCArray
    {
    public:
        IMPORT_C TContactListsWrapper();

        IMPORT_C TContactListsWrapper( const MDesCArray& aCntLists );

    public: // from MDesC16Array
        IMPORT_C TInt MdcaCount() const;

        IMPORT_C TPtrC16 MdcaPoint( TInt aIndex ) const;

    private: // data
        const MDesCArray*           iCntLists;
        TInt                        iWatcherIndex;
    };

class TOneContactListWrapper : public MDesCArray
    {
    public:
        IMPORT_C TOneContactListWrapper( const MPEngContactList2& aCntList );

    public: // from MDesC16Array
        IMPORT_C TInt MdcaCount() const;

        IMPORT_C TPtrC16 MdcaPoint( TInt aIndex ) const;

    private: // data
        const MPEngContactList2&    iList;
    };


class TMultiPecListWrapper : public MDesCArray
    {
    public: // New functions
        IMPORT_C TMultiPecListWrapper();

        IMPORT_C void InitializeLC( MDesCArray& aLists,
                                    CPEngContactListStore2& aListStore );
        IMPORT_C void Close();


    public: // from MDesC16Array
        IMPORT_C TInt MdcaCount() const;

        IMPORT_C TPtrC16 MdcaPoint( TInt aIndex ) const;

    private: // data
        RPointerArray<MPEngContactList2>      iLists;
        RArray<TInt>                          iListCounts;
        TInt                                  iCount;
    };

class TOneDesWrapper : public MDesCArray
    {
    public: // New functions
        IMPORT_C TOneDesWrapper( const TDesC& aDes );

    public: // from MDesC16Array
        IMPORT_C TInt MdcaCount() const;

        IMPORT_C TPtrC16 MdcaPoint( TInt aIndex ) const;

    private: // data
        const TDesC&                        iDes;
    };


NONSHARABLE_CLASS( TTrackedPresenceIds ): public MDesCArray
    {
public:
    IMPORT_C TTrackedPresenceIds();
    IMPORT_C void InitializeLC( CPEngTrackedPresenceIDs2& aTrackedIds );
    IMPORT_C void Close();

public: // from MDesC16Array
    IMPORT_C TInt MdcaCount() const;

    IMPORT_C TPtrC16 MdcaPoint( TInt aIndex ) const;

private:
    RPointerArray<CPEngTrackedPresenceID2>	iTrackedIds;
    };


#endif      // TCAWRAPPERS_H

// End of File

