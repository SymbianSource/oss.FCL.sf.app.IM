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
* Description:  Base class for WV attribute models.
*
*/

#ifndef CPENGWVATTRIBUTEMODELBASE_H
#define CPENGWVATTRIBUTEMODELBASE_H

//  INCLUDES
#include <E32Base.h>
#include "MPEngPresenceAttrModelTypeImp.h"



/**
 * Base class for WV attribute models.
 *
 * @lib PEngAttrLib
 * @since 3.0
 */
NONSHARABLE_CLASS( CPEngWVAttributeModelBase ) : public CBase,
        public MPEngPresenceAttrModelTypeImp
    {
public:  // Constructors and destructor

    /**
     * C++ constructor.
     */
    CPEngWVAttributeModelBase( TBool aUserOwnAttribute );


    /**
     * Destructor.
     */
    virtual ~CPEngWVAttributeModelBase();




public: // Functions from MPEngPresenceAttrModelTypeImp

    /**
     * From MPEngPresenceAttrModelTypeImp.
     *
     * Empty base implementation for such virtual functions
     * that most WV attribute do not support.
     *
     * @since 3.0
     */
    TBool SetDataL( TInt* aIntData,
                    const TDesC8* a8Data,
                    const TDesC16* a16Data,
                    TInt aField,
                    TInt aGroup );


    TBool SetDataAsyncL( const TDesC8* a8Data,
                         const TDesC16* a16Data,
                         TRequestStatus& aRequestStatus,
                         TInt aField,
                         TInt aGroup );

    void CancelSetDataAsync();
    TAny* TypeExtension( TUint32 aType );
    void HandleNetworkSessionStateChangeL( TBool aSessionOpen );


protected: //Helpers


    /**
     * Assertion helpers.
     *
     * @since 3.0
     */
    void __AssertAttrEnumValueOutOfBoundsL() const;
    void __AssertAttrEnumValueOutOfBounds() const;



protected: //Data

    //OWN: Is instance user own attribute or not?
    const TBool   iUserOwnModel;

    };

#endif      //CPENGWVATTRIBUTEMODELBASE_H

//  End of File

